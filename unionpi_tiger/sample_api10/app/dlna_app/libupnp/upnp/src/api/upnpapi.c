/*******************************************************************************
 *
 * Copyright (c) 2000-2003 Intel Corporation
 * All rights reserved.
 * Copyright (C) 2011-2012 France Telecom All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*!
 * \addtogroup UPnPAPI
 *
 * @{
 *
 * \file
 */

#include "config.h"

#include "upnpapi.h"

#include "ThreadPool.h"
#include "UpnpStdInt.h"
#include "UpnpUniStd.h" /* for close() */
#include "httpreadwrite.h"
#include "membuffer.h"
#include "soaplib.h"
#include "ssdplib.h"
#include "sysdep.h"
#include "uuid.h"

/* Needed for GENA */
#include "gena.h"
#include "miniserver.h"
#include "service_table.h"

#ifdef INTERNAL_WEB_SERVER
	#include "VirtualDir.h"
	#include "urlconfig.h"
	#include "webserver.h"
#endif /* INTERNAL_WEB_SERVER */

#include <sys/stat.h>

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "posix_overwrites.h"

#ifdef _WIN32
/* Do not include these files */
#else
	#include <ifaddrs.h>
	#include <sys/ioctl.h>
	#include <sys/param.h>
	#include <sys/types.h>
#endif

// ifr_netmask is not defined on eg OmniOS/Solaris, but since
// ifru_netmask/ifru_addr are all just union members, this should work
#ifndef ifr_netmask // it's a define if it exists
	#define ifr_netmask ifr_addr
#endif

#ifdef UPNP_ENABLE_OPEN_SSL
	#include <openssl/ssl.h>
#endif

#ifndef IN6_IS_ADDR_GLOBAL
	#define IN6_IS_ADDR_GLOBAL(a) \
		((((__const uint32_t *)(a))[0] & \
			 htonl((uint32_t)0x70000000)) == \
			htonl((uint32_t)0x20000000))
#endif /* IS ADDR GLOBAL */

#ifndef IN6_IS_ADDR_ULA
	#define IN6_IS_ADDR_ULA(a) \
		((((__const uint32_t *)(a))[0] & \
			 htonl((uint32_t)0xfe000000)) == \
			htonl((uint32_t)0xfc000000))
#endif /* IS ADDR ULA */

/*! This structure is for virtual directory callbacks */
struct VirtualDirCallbacks virtualDirCallback;

/*! Pointer to the virtual directory list. */
virtualDirList *pVirtualDirList;

#ifdef INCLUDE_CLIENT_APIS
/*! Mutex to synchronize the subscription handling at the client side. */
ithread_mutex_t GlobalClientSubscribeMutex;
#endif /* INCLUDE_CLIENT_APIS */

/*! rwlock to synchronize handles (root device or control point handle). */
ithread_rwlock_t GlobalHndRWLock;

/*! Mutex to synchronize the uuid creation process. */
ithread_mutex_t gUUIDMutex;

/*! Initialization mutex. */
ithread_mutex_t gSDKInitMutex = PTHREAD_MUTEX_INITIALIZER;

/*! Global timer thread. */
TimerThread gTimerThread;

/*! Send thread pool. */
ThreadPool gSendThreadPool;

/*! Receive thread pool. */
ThreadPool gRecvThreadPool;

/*! Mini server thread pool. */
ThreadPool gMiniServerThreadPool;

/*! Flag to indicate the state of web server */
WebServerState bWebServerState = WEB_SERVER_DISABLED;

/*! webCallback for HOST validation. */
WebCallback_HostValidate gWebCallback_HostValidate = 0;

/*! Cookie to the webCallback for HOST validation. */
void *gWebCallback_HostValidateCookie = 0;

/*! Allow literal host names redirection to numeric host names. */
int gAllowLiteralHostRedirection = 0;

/*! Static buffer to contain interface name. (extern'ed in upnp.h) */
char gIF_NAME[LINE_SIZE] = {'\0'};

/*! Static buffer to contain interface IPv4 address. (extern'ed in upnp.h) */
char gIF_IPV4[INET_ADDRSTRLEN] = {'\0'};

/*! Static buffer to contain interface IPv4 netmask. (extern'ed in upnp.h) */
char gIF_IPV4_NETMASK[INET_ADDRSTRLEN] = {'\0'};

/*! Static buffer to contain interface IPv6 link-local address (LLA).
 * (extern'ed in upnp.h) */
char gIF_IPV6[INET6_ADDRSTRLEN] = {'\0'};

/*! IPv6 LLA prefix length. (extern'ed in upnp.h) */
unsigned gIF_IPV6_PREFIX_LENGTH = 0;

/*! Static buffer to contain interface IPv6 unique-local or globally-unique
 * address (ULA or GUA). (extern'ed in upnp.h) */
char gIF_IPV6_ULA_GUA[INET6_ADDRSTRLEN] = {'\0'};

/*! IPv6 ULA or GUA prefix length. (extern'ed in upnp.h) */
unsigned gIF_IPV6_ULA_GUA_PREFIX_LENGTH = 0;

/*! Contains interface index. (extern'ed in upnp.h) */
unsigned gIF_INDEX = (unsigned)-1;

/*! local IPv4 port for the mini-server */
unsigned short LOCAL_PORT_V4;

/*! IPv6 LLA port for the mini-server */
unsigned short LOCAL_PORT_V6;

/*! IPv6 ULA or GUA port for the mini-server */
unsigned short LOCAL_PORT_V6_ULA_GUA;

/*! UPnP device and control point handle table  */
static void *HandleTable[NUM_HANDLE];

/*! a local dir which serves as webserver root */
extern membuffer gDocumentRootDir;

/*! Maximum content-length (in bytes) that the SDK will process on an incoming
 * packet. Content-Length exceeding this size will be not processed and
 * error 413 (HTTP Error Code) will be returned to the remote end point. */
size_t g_maxContentLength = DEFAULT_SOAP_CONTENT_LENGTH;

/*! Global variable to determines the maximum number of
 *  events which can be queued for a given subscription before events begin
 *  to be discarded. This limits the amount of memory used for a
 *  non-responding subscribed entity. */
int g_UpnpSdkEQMaxLen = MAX_SUBSCRIPTION_QUEUED_EVENTS;

/*! Global variable to determine the maximum number of
 *  seconds which an event can spend on a subscription queue (waiting for the
 *  event at the head of the queue to be communicated). This parameter will
 *  have no effect in most situations with the default (low) value of
 *  MAX_SUBSCRIPTION_QUEUED_EVENTS. However, if MAX_SUBSCRIPTION_QUEUED_EVENTS
 *  is set to a high value, the AGE parameter will allow pruning the queue in
 *  good conformance with the UPnP Device Architecture standard, at the
 *  price of higher potential memory use. */
int g_UpnpSdkEQMaxAge = MAX_SUBSCRIPTION_EVENT_AGE;

/*! Global variable to denote the state of Upnp SDK == 0 if uninitialized,
 * == 1 if initialized. */
int UpnpSdkInit = 0;

/*! Global variable to denote the state of Upnp SDK client registration.
 * == 0 if unregistered, >= 1 if registered - registered clients count. */
int UpnpSdkClientRegistered = 0;

/*! Global variable to denote the state of Upnp SDK IPv4 device registration.
 * == 0 if unregistered, == 1 if registered. */
int UpnpSdkDeviceRegisteredV4 = 0;

/*! Global variable to denote the state of Upnp SDK IPv6 device registration.
 * == 0 if unregistered, == 1 if registered. */
int UpnpSdkDeviceregisteredV6 = 0;

#ifdef UPNP_HAVE_OPTSSDP
/*! Global variable used in discovery notifications. */
Upnp_SID gUpnpSdkNLSuuid;
#endif /* UPNP_HAVE_OPTSSDP */

/*! Global variable used as to store the OpenSSL context object
 * to be used for all SSL/TLS connections
 */
#ifdef UPNP_ENABLE_OPEN_SSL
SSL_CTX *gSslCtx = NULL;
#endif

typedef union
{
	struct
	{
		int handle;
		int eventId;
		void *Event;
	} advertise;
	struct UpnpNonblockParam action;
} job_arg;

/*!
 * \brief Free memory associated with advertise job's argument
 */
static void free_advertise_arg(job_arg *arg)
{
	if (arg->advertise.Event) {
		free(arg->advertise.Event);
	}
	free(arg);
}

/*!
 * \brief Free memory associated with an action job's argument
 */
static void free_action_arg(job_arg *arg)
{
	if (arg->action.Header) {
		ixmlDocument_free(arg->action.Header);
	}
	if (arg->action.Act) {
		ixmlDocument_free(arg->action.Act);
	}
	free(arg);
}

/*!
 * \brief (Windows Only) Initializes the Windows Winsock library.
 *
 * \return UPNP_E_SUCCESS on success, UPNP_E_INIT_FAILED on failure.
 */
static int WinsockInit(void)
{
	int retVal = UPNP_E_SUCCESS;
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		retVal = UPNP_E_INIT_FAILED;
		goto exit_function;
	}
	/* Confirm that the WinSock DLL supports 2.2.
	 * Note that if the DLL supports versions greater
	 * than 2.2 in addition to 2.2, it will still return
	 * 2.2 in wVersion since that is the version we
	 * requested. */
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable
		 * WinSock DLL. */
		WSACleanup();
		retVal = UPNP_E_INIT_FAILED;
		goto exit_function;
	}
	/* The WinSock DLL is acceptable. Proceed. */
exit_function:
#else
#endif
	return retVal;
}

/*!
 * \brief Initializes the global mutexes used by the UPnP SDK.
 *
 * \return UPNP_E_SUCCESS on success or UPNP_E_INIT_FAILED if a mutex could not
 * 	be initialized.
 */
static int UpnpInitMutexes(void)
{
#ifdef __CYGWIN__
	/* On Cygwin, pthread_mutex_init() fails without this memset. */
	/* TODO: Fix Cygwin so we don't need this memset(). */
	memset(&GlobalHndRWLock, 0, sizeof(GlobalHndRWLock));
#endif
	if (ithread_rwlock_init(&GlobalHndRWLock, NULL) != 0) {
		return UPNP_E_INIT_FAILED;
	}

	if (ithread_mutex_init(&gUUIDMutex, NULL) != 0) {
		return UPNP_E_INIT_FAILED;
	}
	/* initialize subscribe mutex. */
#ifdef INCLUDE_CLIENT_APIS
	if (ithread_mutex_init(&GlobalClientSubscribeMutex, NULL) != 0) {
		return UPNP_E_INIT_FAILED;
	}
#endif
	return UPNP_E_SUCCESS;
}

/*!
 * \brief Initializes the global threadm pools used by the UPnP SDK.
 *
 * \return UPNP_E_SUCCESS on success or UPNP_E_INIT_FAILED if a mutex could not
 * 	be initialized.
 */
static int UpnpInitThreadPools(void)
{
	int ret = UPNP_E_SUCCESS;
	ThreadPoolAttr attr;

	TPAttrInit(&attr);
	TPAttrSetMaxThreads(&attr, MAX_THREADS);
	TPAttrSetMinThreads(&attr, MIN_THREADS);
	TPAttrSetStackSize(&attr, THREAD_STACK_SIZE);
	TPAttrSetJobsPerThread(&attr, JOBS_PER_THREAD);
	TPAttrSetIdleTime(&attr, THREAD_IDLE_TIME);
	TPAttrSetMaxJobsTotal(&attr, MAX_JOBS_TOTAL);

	if (ThreadPoolInit(&gSendThreadPool, &attr) != UPNP_E_SUCCESS) {
		ret = UPNP_E_INIT_FAILED;
		goto exit_function;
	}

	if (ThreadPoolInit(&gRecvThreadPool, &attr) != UPNP_E_SUCCESS) {
		ret = UPNP_E_INIT_FAILED;
		goto exit_function;
	}

	if (ThreadPoolInit(&gMiniServerThreadPool, &attr) != UPNP_E_SUCCESS) {
		ret = UPNP_E_INIT_FAILED;
		goto exit_function;
	}

exit_function:
	if (ret != UPNP_E_SUCCESS) {
		UpnpSdkInit = 0;
		UpnpFinish();
	}

	return ret;
}

/*!
 * \brief Performs the initial steps in initializing the UPnP SDK.
 *
 * \li Winsock library is initialized for the process (Windows specific).
 * \li The logging (for debug messages) is initialized.
 * \li Mutexes, Handle table and thread pools are allocated and initialized.
 * \li Callback functions for SOAP and GENA are set, if they're enabled.
 * \li The SDK timer thread is initialized.
 *
 * \return UPNP_E_SUCCESS on success.
 */
static int UpnpInitPreamble(void)
{
	int retVal = UPNP_E_SUCCESS;
	int i;
#ifdef UPNP_HAVE_OPTSSDP
	uuid_upnp nls_uuid;
#endif /* UPNP_HAVE_OPTSSDP */

	retVal = WinsockInit();
	if (retVal != UPNP_E_SUCCESS) {
		return retVal;
	}

	/* needed by SSDP or other parts. */
	srand((unsigned int)time(NULL));

	/* Initialize debug output. */
	retVal = UpnpInitLog();
	if (retVal != UPNP_E_SUCCESS) {
		/* UpnpInitLog does not return a valid UPNP_E_*. */
		return UPNP_E_INIT_FAILED;
	}

	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpInitPreamble\n");

	/* Initialize SDK global mutexes. */
	retVal = UpnpInitMutexes();
	if (retVal != UPNP_E_SUCCESS) {
		return retVal;
	}

#ifdef UPNP_HAVE_OPTSSDP
	/* Create the NLS uuid. */
	uuid_create(&nls_uuid);
	upnp_uuid_unpack(&nls_uuid, gUpnpSdkNLSuuid);
#endif /* UPNP_HAVE_OPTSSDP */

	/* Initializes the handle list. */
	HandleLock();
	for (i = 0; i < NUM_HANDLE; ++i) {
		HandleTable[i] = NULL;
	}
	HandleUnlock();

	/* Initialize SDK global thread pools. */
	retVal = UpnpInitThreadPools();
	if (retVal != UPNP_E_SUCCESS) {
		return retVal;
	}

#ifdef INCLUDE_DEVICE_APIS
	#if EXCLUDE_SOAP == 0
	SetSoapCallback(soap_device_callback);
	#endif
#endif /* INCLUDE_DEVICE_APIS */

#ifdef INTERNAL_WEB_SERVER
	#if EXCLUDE_GENA == 0
	SetGenaCallback(genaCallback);
	#endif
#endif /* INTERNAL_WEB_SERVER */

	/* Initialize the SDK timer thread. */
	retVal = TimerThreadInit(&gTimerThread, &gSendThreadPool);
	if (retVal != UPNP_E_SUCCESS) {
		UpnpFinish();

		return retVal;
	}

	return UPNP_E_SUCCESS;
}

/*!
 * \brief Finishes initializing the UPnP SDK.
 *	\li The MiniServer is started, if enabled.
 *	\li The WebServer is started, if enabled.
 *
 * \return UPNP_E_SUCCESS on success or  UPNP_E_INIT_FAILED if a mutex could not
 * 	be initialized.
 */
static int UpnpInitStartServers(
	/*! [in] Local Port to listen for incoming connections. */
	unsigned short DestPort)
{
#if EXCLUDE_MINISERVER == 0 || EXCLUDE_WEB_SERVER == 0
	int retVal = 0;
#endif

	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"Entering UpnpInitStartServers\n");

#if EXCLUDE_MINISERVER == 0
	LOCAL_PORT_V4 = DestPort;
	LOCAL_PORT_V6 = DestPort;
	LOCAL_PORT_V6_ULA_GUA = DestPort;
	retVal = StartMiniServer(
		&LOCAL_PORT_V4, &LOCAL_PORT_V6, &LOCAL_PORT_V6_ULA_GUA);
	if (retVal != UPNP_E_SUCCESS) {
		UpnpPrintf(UPNP_CRITICAL,
			API,
			__FILE__,
			__LINE__,
			"Miniserver failed to start\n");
		UpnpFinish();
		return retVal;
	}
#endif

#if EXCLUDE_WEB_SERVER == 0
	membuffer_init(&gDocumentRootDir);
	retVal = UpnpEnableWebserver(WEB_SERVER_ENABLED);
	if (retVal != UPNP_E_SUCCESS) {
		UpnpFinish();
		return retVal;
	}
#endif

	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"Exiting UpnpInitStartServers\n");

	return UPNP_E_SUCCESS;
}

int UpnpInit2(const char *IfName, unsigned short DestPort)
{
	int retVal;

	/* Initializes the ithread library */
	ithread_initialize_library();

	ithread_mutex_lock(&gSDKInitMutex);

	/* Check if we're already initialized. */
	if (UpnpSdkInit == 1) {
		retVal = UPNP_E_INIT;
		goto exit_function;
	}

	/* Set the UpnpSdkInit flag to 1 to indicate we're successfully
	 * initialized. */
	UpnpSdkInit = 1;

	/* Perform initialization preamble. */
	retVal = UpnpInitPreamble();
	if (retVal != UPNP_E_SUCCESS) {
		goto exit_function;
	}

	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"UpnpInit2 with IfName=%s, DestPort=%d.\n",
		IfName ? IfName : "NULL",
		DestPort);

	/* Retrieve interface information (Addresses, index, etc). */
	retVal = UpnpGetIfInfo(IfName);
	if (retVal != UPNP_E_SUCCESS) {
		goto exit_function;
	}

	/* Finish initializing the SDK. */
	retVal = UpnpInitStartServers(DestPort);
	if (retVal != UPNP_E_SUCCESS) {
		goto exit_function;
	}

exit_function:
	if (retVal != UPNP_E_SUCCESS && retVal != UPNP_E_INIT) {
		UpnpFinish();
	}
	ithread_mutex_unlock(&gSDKInitMutex);

	return retVal;
}

#ifdef UPNP_ENABLE_OPEN_SSL
int UpnpInitSslContext(int initOpenSslLib, const SSL_METHOD *sslMethod)
{
	if (gSslCtx)
		return UPNP_E_INIT;
	if (initOpenSslLib) {
		SSL_load_error_strings();
		SSL_library_init();
		OpenSSL_add_all_algorithms();
	}
	gSslCtx = SSL_CTX_new(sslMethod);
	if (!gSslCtx) {
		return UPNP_E_INIT_FAILED;
	}
	return UPNP_E_SUCCESS;
}
#endif

#ifdef DEBUG
/*!
 * \brief Prints thread pool statistics.
 */
void PrintThreadPoolStats(
	/*! [in] The thread pool. */
	ThreadPool *tp,
	/*! [in] The file name that called this function, use the macro
	 * __FILE__. */
	const char *DbgFileName,
	/*! [in] The line number that the function was called, use the macro
	 * __LINE__. */
	int DbgLineNo,
	/*! [in] The message. */
	const char *msg)
{
	ThreadPoolStats stats;
	ThreadPoolGetStats(tp, &stats);
	UpnpPrintf(UPNP_INFO,
		API,
		DbgFileName,
		DbgLineNo,
		"%s\n"
		"High Jobs pending: %d\n"
		"Med Jobs Pending: %d\n"
		"Low Jobs Pending: %d\n"
		"Average wait in High Q in milliseconds: %lf\n"
		"Average wait in Med Q in milliseconds: %lf\n"
		"Average wait in Low Q in milliseconds: %lf\n"
		"Max Threads Used: %d\n"
		"Worker Threads: %d\n"
		"Persistent Threads: %d\n"
		"Idle Threads: %d\n"
		"Total Threads: %d\n"
		"Total Work Time: %lf\n"
		"Total Idle Time: %lf\n",
		msg,
		stats.currentJobsHQ,
		stats.currentJobsMQ,
		stats.currentJobsLQ,
		stats.avgWaitHQ,
		stats.avgWaitMQ,
		stats.avgWaitLQ,
		stats.maxThreads,
		stats.workerThreads,
		stats.persistentThreads,
		stats.idleThreads,
		stats.totalThreads,
		stats.totalWorkTime,
		stats.totalIdleTime);
}
#else
	#define PrintThreadPoolStats(tp, DbgFileName, DbgLineNo, msg) \
		do { \
		} while (0)
#endif /* DEBUG */

int UpnpFinish(void)
{
#ifdef INCLUDE_DEVICE_APIS
	UpnpDevice_Handle device_handle;
#endif
#ifdef INCLUDE_CLIENT_APIS
	UpnpClient_Handle client_handle;
#endif
	struct Handle_Info *temp;
#ifdef UPNP_ENABLE_OPEN_SSL
	if (gSslCtx) {
		SSL_CTX_free(gSslCtx);
		gSslCtx = NULL;
	}
#endif
	if (UpnpSdkInit != 1)
		return UPNP_E_FINISH;
	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpFinish: UpnpSdkInit is %d\n",
		UpnpSdkInit);
	if (UpnpSdkInit == 1)
		UpnpPrintf(UPNP_INFO,
			API,
			__FILE__,
			__LINE__,
			"UpnpFinish: UpnpSdkInit is ONE\n");
	PrintThreadPoolStats(
		&gSendThreadPool, __FILE__, __LINE__, "Send Thread Pool");
	PrintThreadPoolStats(
		&gRecvThreadPool, __FILE__, __LINE__, "Recv Thread Pool");
	PrintThreadPoolStats(&gMiniServerThreadPool,
		__FILE__,
		__LINE__,
		"MiniServer Thread Pool");
#ifdef INCLUDE_DEVICE_APIS
	while (GetDeviceHandleInfo(0, AF_INET, &device_handle, &temp) ==
		HND_DEVICE) {
		UpnpUnRegisterRootDevice(device_handle);
	}
	while (GetDeviceHandleInfo(0, AF_INET6, &device_handle, &temp) ==
		HND_DEVICE) {
		UpnpUnRegisterRootDevice(device_handle);
	}
#endif
#ifdef INCLUDE_CLIENT_APIS
	while (HND_CLIENT == GetClientHandleInfo(&client_handle, &temp)) {
		UpnpUnRegisterClient(client_handle);
	}
#endif
	TimerThreadShutdown(&gTimerThread);
#if EXCLUDE_MINISERVER == 0
	StopMiniServer();
#endif
#if EXCLUDE_WEB_SERVER == 0
	web_server_destroy();
#endif
	ThreadPoolShutdown(&gMiniServerThreadPool);
	PrintThreadPoolStats(&gMiniServerThreadPool,
		__FILE__,
		__LINE__,
		"MiniServer Thread Pool");
	ThreadPoolShutdown(&gRecvThreadPool);
	PrintThreadPoolStats(
		&gSendThreadPool, __FILE__, __LINE__, "Send Thread Pool");
	ThreadPoolShutdown(&gSendThreadPool);
	PrintThreadPoolStats(
		&gRecvThreadPool, __FILE__, __LINE__, "Recv Thread Pool");
#ifdef INCLUDE_CLIENT_APIS
	ithread_mutex_destroy(&GlobalClientSubscribeMutex);
#endif
	ithread_rwlock_destroy(&GlobalHndRWLock);
	ithread_mutex_destroy(&gUUIDMutex);
	/* remove all virtual dirs */
	UpnpRemoveAllVirtualDirs();
	UpnpSdkInit = 0;
	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"Exiting UpnpFinish: UpnpSdkInit is :%d:\n",
		UpnpSdkInit);
	UpnpCloseLog();
	/* Clean-up ithread library resources */
	ithread_cleanup_library();

	return UPNP_E_SUCCESS;
}

unsigned short UpnpGetServerPort(void)
{
	if (UpnpSdkInit != 1)
		return 0u;

	return LOCAL_PORT_V4;
}

unsigned short UpnpGetServerPort6(void)
{
#ifdef UPNP_ENABLE_IPV6
	if (UpnpSdkInit != 1)
		return 0u;

	return LOCAL_PORT_V6;
#else
	return 0u;
#endif
}

unsigned short UpnpGetServerUlaGuaPort6(void)
{
#ifdef UPNP_ENABLE_IPV6
	if (UpnpSdkInit != 1)
		return 0u;

	return LOCAL_PORT_V6_ULA_GUA;
#else
	return 0u;
#endif
}

char *UpnpGetServerIpAddress(void)
{
	if (UpnpSdkInit != 1)
		return NULL;

	return gIF_IPV4;
}

int UpnpSetServerIpAddress(const char *ip)
{
	memcpy(gIF_IPV4, ip, strlen(ip));
	gIF_IPV4[strlen(ip)] = '\0';

	return 0;
}

char *UpnpGetServerIp6Address(void)
{
#ifdef UPNP_ENABLE_IPV6
	if (UpnpSdkInit != 1)
		return NULL;

	return gIF_IPV6;
#else
	return NULL;
#endif
}

char *UpnpGetServerUlaGuaIp6Address(void)
{
#ifdef UPNP_ENABLE_IPV6
	if (UpnpSdkInit != 1)
		return NULL;

	return gIF_IPV6_ULA_GUA;
#else
	return NULL;
#endif
}

/*!
 * \brief Get a free handle.
 *
 * \return On success, an integer greater than zero or UPNP_E_OUTOF_HANDLE on
 * 	failure.
 */
static int GetFreeHandle()
{
	/* Handle 0 is not used as NULL translates to 0 when passed as a handle
	 */
	int i = 1;

	while (i < NUM_HANDLE && HandleTable[i] != NULL)
		++i;
	if (i == NUM_HANDLE)
		return UPNP_E_OUTOF_HANDLE;
	else
		return i;
}

/*!
 * \brief Free handle.
 *
 * \return UPNP_E_SUCCESS if successful or UPNP_E_INVALID_HANDLE if not
 */
static int FreeHandle(
	/*! [in] Handle index. */
	int Upnp_Handle)
{
	int ret = UPNP_E_INVALID_HANDLE;

	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"FreeHandle: entering, Handle is %d\n",
		Upnp_Handle);
	if (Upnp_Handle < 1 || Upnp_Handle >= NUM_HANDLE) {
		UpnpPrintf(UPNP_CRITICAL,
			API,
			__FILE__,
			__LINE__,
			"FreeHandle: Handle %d is out of range\n",
			Upnp_Handle);
	} else if (HandleTable[Upnp_Handle] == NULL) {
		UpnpPrintf(UPNP_CRITICAL,
			API,
			__FILE__,
			__LINE__,
			"FreeHandle: HandleTable[%d] is NULL\n",
			Upnp_Handle);
	} else {
		free(HandleTable[Upnp_Handle]);
		HandleTable[Upnp_Handle] = NULL;
		ret = UPNP_E_SUCCESS;
	}
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"FreeHandle: exiting, ret = %d.\n",
		ret);

	return ret;
}

#ifdef INCLUDE_DEVICE_APIS
int UpnpRegisterRootDevice(const char *DescUrl,
	Upnp_FunPtr Fun,
	const void *Cookie,
	UpnpDevice_Handle *Hnd)
{
	struct Handle_Info *HInfo = NULL;
	int retVal = 0;
	#if EXCLUDE_GENA == 0
	int hasServiceTable = 0;
	#endif /* EXCLUDE_GENA */

	HandleLock();

	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpRegisterRootDevice\n");

	if (UpnpSdkInit != 1) {
		retVal = UPNP_E_FINISH;
		goto exit_function;
	}

	if (Hnd == NULL || Fun == NULL || DescUrl == NULL ||
		strlen(DescUrl) == (size_t)0) {
		retVal = UPNP_E_INVALID_PARAM;
		goto exit_function;
	}

	*Hnd = GetFreeHandle();
	if (*Hnd == UPNP_E_OUTOF_HANDLE) {
		retVal = UPNP_E_OUTOF_MEMORY;
		goto exit_function;
	}

	HInfo = (struct Handle_Info *)malloc(sizeof(struct Handle_Info));
	if (HInfo == NULL) {
		retVal = UPNP_E_OUTOF_MEMORY;
		goto exit_function;
	}
	memset(HInfo, 0, sizeof(struct Handle_Info));
	HandleTable[*Hnd] = HInfo;

	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Root device URL is %s\n",
		DescUrl);

	HInfo->aliasInstalled = 0;
	HInfo->HType = HND_DEVICE;
	strncpy(HInfo->DescURL, DescUrl, sizeof(HInfo->DescURL) - 1);
	strncpy(HInfo->LowerDescURL, DescUrl, sizeof(HInfo->LowerDescURL) - 1);
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Following Root Device URL will be used when answering to "
		"legacy CPs %s\n",
		HInfo->LowerDescURL);
	HInfo->Callback = Fun;
	HInfo->Cookie = (void *)Cookie;
	HInfo->MaxAge = DEFAULT_MAXAGE;
	HInfo->DeviceList = NULL;
	HInfo->ServiceList = NULL;
	HInfo->DescDocument = NULL;
	#ifdef INCLUDE_CLIENT_APIS
	ListInit(&HInfo->SsdpSearchList, NULL, NULL);
	HInfo->ClientSubList = NULL;
	#endif /* INCLUDE_CLIENT_APIS */
	HInfo->MaxSubscriptions = UPNP_INFINITE;
	HInfo->MaxSubscriptionTimeOut = UPNP_INFINITE;
	HInfo->DeviceAf = AF_INET;

	retVal = UpnpDownloadXmlDoc(HInfo->DescURL, &(HInfo->DescDocument));
	if (retVal != UPNP_E_SUCCESS) {
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice: error downloading Document: "
			"%d\n",
			retVal);
	#ifdef INCLUDE_CLIENT_APIS
		ListDestroy(&HInfo->SsdpSearchList, 0);
	#endif /* INCLUDE_CLIENT_APIS */
		FreeHandle(*Hnd);
		goto exit_function;
	}
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"UpnpRegisterRootDevice: Valid Description\n"
		"UpnpRegisterRootDevice: DescURL : %s\n",
		HInfo->DescURL);

	HInfo->DeviceList = ixmlDocument_getElementsByTagName(
		HInfo->DescDocument, "device");
	if (!HInfo->DeviceList) {
	#ifdef INCLUDE_CLIENT_APIS
		ListDestroy(&HInfo->SsdpSearchList, 0);
	#endif /* INCLUDE_CLIENT_APIS */
		ixmlDocument_free(HInfo->DescDocument);
		FreeHandle(*Hnd);
		UpnpPrintf(UPNP_CRITICAL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice: No devices found for "
			"RootDevice\n");
		retVal = UPNP_E_INVALID_DESC;
		goto exit_function;
	}

	HInfo->ServiceList = ixmlDocument_getElementsByTagName(
		HInfo->DescDocument, "serviceList");
	if (!HInfo->ServiceList) {
		UpnpPrintf(UPNP_CRITICAL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice: No services found for "
			"RootDevice\n");
	}

	#if EXCLUDE_GENA == 0
	/*
	 * GENA SET UP
	 */
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"UpnpRegisterRootDevice: Gena Check\n");
	memset(&HInfo->ServiceTable, 0, sizeof(HInfo->ServiceTable));
	hasServiceTable = getServiceTable((IXML_Node *)HInfo->DescDocument,
		&HInfo->ServiceTable,
		HInfo->DescURL);
	if (hasServiceTable) {
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice: GENA Service Table\n"
			"Here are the known services:\n");
		printServiceTable(&HInfo->ServiceTable, UPNP_ALL, API);
	} else {
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"\nUpnpRegisterRootDevice: Empty service table\n");
	}
	#endif /* EXCLUDE_GENA */

	UpnpSdkDeviceRegisteredV4 = 1;

	retVal = UPNP_E_SUCCESS;

exit_function:
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Exiting RegisterRootDevice, return value == %d\n",
		retVal);
	HandleUnlock();

	return retVal;
}
#endif /* INCLUDE_DEVICE_APIS */

/*!
 * \brief Fills the sockadr_in with miniserver information.
 */
static int GetDescDocumentAndURL(
	/* [in] pointer to server address structure. */
	Upnp_DescType descriptionType,
	/* [in] . */
	char *description,
	/* [in] . */
	int config_baseURL,
	/* [in] . */
	int AddressFamily,
	/* [out] . */
	IXML_Document **xmlDoc,
	/* [out] . */
	char descURL[LINE_SIZE]);

#ifdef INCLUDE_DEVICE_APIS
int UpnpRegisterRootDevice2(Upnp_DescType descriptionType,
	const char *description_const,
	size_t bufferLen, /* ignored */
	int config_baseURL,
	Upnp_FunPtr Fun,
	const void *Cookie,
	UpnpDevice_Handle *Hnd)
{
	struct Handle_Info *HInfo = NULL;
	int retVal = 0;
	#if EXCLUDE_GENA == 0
	int hasServiceTable = 0;
	#endif /* EXCLUDE_GENA */
	char *description = (char *)description_const;
	(void)bufferLen;

	HandleLock();

	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpRegisterRootDevice2\n");

	if (UpnpSdkInit != 1) {
		retVal = UPNP_E_FINISH;
		goto exit_function;
	}

	if (Hnd == NULL || Fun == NULL) {
		retVal = UPNP_E_INVALID_PARAM;
		goto exit_function;
	}

	*Hnd = GetFreeHandle();
	if (*Hnd == UPNP_E_OUTOF_HANDLE) {
		retVal = UPNP_E_OUTOF_MEMORY;
		goto exit_function;
	}

	HInfo = (struct Handle_Info *)malloc(sizeof(struct Handle_Info));
	if (HInfo == NULL) {
		retVal = UPNP_E_OUTOF_MEMORY;
		goto exit_function;
	}
	memset(HInfo, 0, sizeof(struct Handle_Info));
	HandleTable[*Hnd] = HInfo;

	/* prevent accidental removal of a non-existent alias */
	HInfo->aliasInstalled = 0;

	retVal = GetDescDocumentAndURL(descriptionType,
		description,
		config_baseURL,
		AF_INET,
		&HInfo->DescDocument,
		HInfo->DescURL);
	if (retVal != UPNP_E_SUCCESS) {
		FreeHandle(*Hnd);
		goto exit_function;
	}

	strncpy(HInfo->LowerDescURL,
		HInfo->DescURL,
		sizeof(HInfo->LowerDescURL) - 1);
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Following Root Device URL will be used when answering to "
		"legacy CPs %s\n",
		HInfo->LowerDescURL);
	HInfo->aliasInstalled = config_baseURL != 0;
	HInfo->HType = HND_DEVICE;
	HInfo->Callback = Fun;
	HInfo->Cookie = (void *)Cookie;
	HInfo->MaxAge = DEFAULT_MAXAGE;
	HInfo->DeviceList = NULL;
	HInfo->ServiceList = NULL;
	#ifdef INCLUDE_CLIENT_APIS
	ListInit(&HInfo->SsdpSearchList, NULL, NULL);
	HInfo->ClientSubList = NULL;
	#endif /* INCLUDE_CLIENT_APIS */
	HInfo->MaxSubscriptions = UPNP_INFINITE;
	HInfo->MaxSubscriptionTimeOut = UPNP_INFINITE;
	HInfo->DeviceAf = AF_INET;

	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"UpnpRegisterRootDevice2: Valid Description\n"
		"UpnpRegisterRootDevice2: DescURL : %s\n",
		HInfo->DescURL);

	HInfo->DeviceList = ixmlDocument_getElementsByTagName(
		HInfo->DescDocument, "device");
	if (!HInfo->DeviceList) {
	#ifdef INCLUDE_CLIENT_APIS
		ListDestroy(&HInfo->SsdpSearchList, 0);
	#endif /* INCLUDE_CLIENT_APIS */
		ixmlDocument_free(HInfo->DescDocument);
		FreeHandle(*Hnd);
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice2: No devices found for "
			"RootDevice\n");
		retVal = UPNP_E_INVALID_DESC;
		goto exit_function;
	}

	HInfo->ServiceList = ixmlDocument_getElementsByTagName(
		HInfo->DescDocument, "serviceList");
	if (!HInfo->ServiceList) {
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice2: No services found for "
			"RootDevice\n");
	}

	#if EXCLUDE_GENA == 0
	/*
	 * GENA SET UP
	 */
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"UpnpRegisterRootDevice2: Gena Check\n");
	memset(&HInfo->ServiceTable, 0, sizeof(HInfo->ServiceTable));
	hasServiceTable = getServiceTable((IXML_Node *)HInfo->DescDocument,
		&HInfo->ServiceTable,
		HInfo->DescURL);
	if (hasServiceTable) {
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice2: GENA Service Table\n"
			"Here are the known services: \n");
		printServiceTable(&HInfo->ServiceTable, UPNP_ALL, API);
	} else {
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"\nUpnpRegisterRootDevice2: Empty service table\n");
	}
	#endif /* EXCLUDE_GENA */

	UpnpSdkDeviceRegisteredV4 = 1;

	retVal = UPNP_E_SUCCESS;

exit_function:
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Exiting RegisterRootDevice2, return value == %d\n",
		retVal);
	HandleUnlock();

	return retVal;
}
#endif /* INCLUDE_DEVICE_APIS */

#ifdef INCLUDE_DEVICE_APIS
int UpnpRegisterRootDevice3(const char *DescUrl,
	Upnp_FunPtr Fun,
	const void *Cookie,
	UpnpDevice_Handle *Hnd,
	int AddressFamily)
{
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpRegisterRootDevice3\n");
	return UpnpRegisterRootDevice4(
		DescUrl, Fun, Cookie, Hnd, AddressFamily, NULL);
}
#endif /* INCLUDE_DEVICE_APIS */

#ifdef INCLUDE_DEVICE_APIS
int UpnpRegisterRootDevice4(const char *DescUrl,
	Upnp_FunPtr Fun,
	const void *Cookie,
	UpnpDevice_Handle *Hnd,
	int AddressFamily,
	const char *LowerDescUrl)
{
	struct Handle_Info *HInfo;
	int retVal = 0;
	#if EXCLUDE_GENA == 0
	int hasServiceTable = 0;
	#endif /* EXCLUDE_GENA */

	HandleLock();

	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpRegisterRootDevice4\n");
	if (UpnpSdkInit != 1) {
		retVal = UPNP_E_FINISH;
		goto exit_function;
	}
	if (Hnd == NULL || Fun == NULL || DescUrl == NULL ||
		strlen(DescUrl) == (size_t)0 ||
		(AddressFamily != AF_INET && AddressFamily != AF_INET6)) {
		retVal = UPNP_E_INVALID_PARAM;
		goto exit_function;
	}
	*Hnd = GetFreeHandle();
	if (*Hnd == UPNP_E_OUTOF_HANDLE) {
		retVal = UPNP_E_OUTOF_MEMORY;
		goto exit_function;
	}
	HInfo = (struct Handle_Info *)malloc(sizeof(struct Handle_Info));
	if (HInfo == NULL) {
		retVal = UPNP_E_OUTOF_MEMORY;
		goto exit_function;
	}
	memset(HInfo, 0, sizeof(struct Handle_Info));
	HandleTable[*Hnd] = HInfo;
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Root device URL is %s\n",
		DescUrl);
	HInfo->aliasInstalled = 0;
	HInfo->HType = HND_DEVICE;
	strncpy(HInfo->DescURL, DescUrl, sizeof(HInfo->DescURL) - 1);
	if (LowerDescUrl == NULL)
		strncpy(HInfo->LowerDescURL,
			DescUrl,
			sizeof(HInfo->LowerDescURL) - 1);
	else
		strncpy(HInfo->LowerDescURL,
			LowerDescUrl,
			sizeof(HInfo->LowerDescURL) - 1);
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Following Root Device URL will be used when answering to "
		"legacy CPs %s\n",
		HInfo->LowerDescURL);
	HInfo->Callback = Fun;
	HInfo->Cookie = (void *)Cookie;
	HInfo->MaxAge = DEFAULT_MAXAGE;
	HInfo->DeviceList = NULL;
	HInfo->ServiceList = NULL;
	HInfo->DescDocument = NULL;
	#ifdef INCLUDE_CLIENT_APIS
	ListInit(&HInfo->SsdpSearchList, NULL, NULL);
	HInfo->ClientSubList = NULL;
	#endif /* INCLUDE_CLIENT_APIS */
	HInfo->MaxSubscriptions = UPNP_INFINITE;
	HInfo->MaxSubscriptionTimeOut = UPNP_INFINITE;
	HInfo->DeviceAf = AddressFamily;
	retVal = UpnpDownloadXmlDoc(HInfo->DescURL, &(HInfo->DescDocument));
	if (retVal != UPNP_E_SUCCESS) {
	#ifdef INCLUDE_CLIENT_APIS
		ListDestroy(&HInfo->SsdpSearchList, 0);
	#endif /* INCLUDE_CLIENT_APIS */
		FreeHandle(*Hnd);
		goto exit_function;
	}
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"UpnpRegisterRootDevice4: Valid Description\n"
		"UpnpRegisterRootDevice4: DescURL : %s\n",
		HInfo->DescURL);

	HInfo->DeviceList = ixmlDocument_getElementsByTagName(
		HInfo->DescDocument, "device");
	if (!HInfo->DeviceList) {
	#ifdef INCLUDE_CLIENT_APIS
		ListDestroy(&HInfo->SsdpSearchList, 0);
	#endif /* INCLUDE_CLIENT_APIS */
		ixmlDocument_free(HInfo->DescDocument);
		FreeHandle(*Hnd);
		UpnpPrintf(UPNP_CRITICAL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice4: No devices found for "
			"RootDevice\n");
		retVal = UPNP_E_INVALID_DESC;
		goto exit_function;
	}

	HInfo->ServiceList = ixmlDocument_getElementsByTagName(
		HInfo->DescDocument, "serviceList");
	if (!HInfo->ServiceList) {
		UpnpPrintf(UPNP_CRITICAL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice4: No services found for "
			"RootDevice\n");
	}

	#if EXCLUDE_GENA == 0
	/*
	 * GENA SET UP
	 */
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"UpnpRegisterRootDevice4: Gena Check\n");
	memset(&HInfo->ServiceTable, 0, sizeof(HInfo->ServiceTable));
	hasServiceTable = getServiceTable((IXML_Node *)HInfo->DescDocument,
		&HInfo->ServiceTable,
		HInfo->DescURL);
	if (hasServiceTable) {
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"UpnpRegisterRootDevice4: GENA Service Table \n"
			"Here are the known services: \n");
		printServiceTable(&HInfo->ServiceTable, UPNP_ALL, API);
	} else {
		UpnpPrintf(UPNP_ALL,
			API,
			__FILE__,
			__LINE__,
			"\nUpnpRegisterRootDevice4: Empty service table\n");
	}
	#endif /* EXCLUDE_GENA */

	switch (AddressFamily) {
	case AF_INET:
		UpnpSdkDeviceRegisteredV4 = 1;
		break;
	default:
		UpnpSdkDeviceregisteredV6 = 1;
	}

	retVal = UPNP_E_SUCCESS;

exit_function:
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Exiting RegisterRootDevice4, return value == %d\n",
		retVal);
	HandleUnlock();

	return retVal;
}
#endif /* INCLUDE_DEVICE_APIS */

#ifdef INCLUDE_DEVICE_APIS
int UpnpUnRegisterRootDevice(UpnpDevice_Handle Hnd)
{
	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpUnRegisterRootDevice\n");
	return UpnpUnRegisterRootDeviceLowPower(Hnd, -1, -1, -1);
}

int UpnpUnRegisterRootDeviceLowPower(UpnpDevice_Handle Hnd,
	int PowerState,
	int SleepPeriod,
	int RegistrationState)
{
	int retVal = 0;
	struct Handle_Info *HInfo = NULL;

	if (UpnpSdkInit != 1)
		return UPNP_E_FINISH;
	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpUnRegisterRootDeviceLowPower\n");
	#if EXCLUDE_GENA == 0
	if (genaUnregisterDevice(Hnd) != UPNP_E_SUCCESS)
		return UPNP_E_INVALID_HANDLE;
	#endif

	HandleLock();
	switch (GetHandleInfo(Hnd, &HInfo)) {
	case HND_INVALID:
		HandleUnlock();
		return UPNP_E_INVALID_HANDLE;
	default:
		break;
	}
	HInfo->PowerState = PowerState;
	if (SleepPeriod < 0)
		SleepPeriod = -1;
	HInfo->SleepPeriod = SleepPeriod;
	HInfo->RegistrationState = RegistrationState;
	HandleUnlock();

	#if EXCLUDE_SSDP == 0
	retVal = AdvertiseAndReply(-1,
		Hnd,
		(enum SsdpSearchType)0,
		(struct sockaddr *)NULL,
		(char *)NULL,
		(char *)NULL,
		(char *)NULL,
		HInfo->MaxAge);
	#endif

	HandleLock();
	switch (GetHandleInfo(Hnd, &HInfo)) {
	case HND_INVALID:
		HandleUnlock();
		return UPNP_E_INVALID_HANDLE;
	default:
		break;
	}
	ixmlNodeList_free(HInfo->DeviceList);
	ixmlNodeList_free(HInfo->ServiceList);
	ixmlDocument_free(HInfo->DescDocument);
	#ifdef INCLUDE_CLIENT_APIS
	ListDestroy(&HInfo->SsdpSearchList, 0);
	#endif /* INCLUDE_CLIENT_APIS */
	#ifdef INTERNAL_WEB_SERVER
	if (HInfo->aliasInstalled)
		web_server_set_alias(NULL, NULL, 0, 0);
	#endif /* INTERNAL_WEB_SERVER */
	switch (HInfo->DeviceAf) {
	case AF_INET:
		UpnpSdkDeviceRegisteredV4 = 0;
		break;
	case AF_INET6:
		UpnpSdkDeviceregisteredV6 = 0;
		break;
	default:
		break;
	}
	FreeHandle(Hnd);
	HandleUnlock();

	UpnpPrintf(UPNP_INFO,
		API,
		__FILE__,
		__LINE__,
		"Exiting UpnpUnRegisterRootDeviceLowPower\n");

	return retVal;
}
#endif /* INCLUDE_DEVICE_APIS */

#ifdef INCLUDE_CLIENT_APIS
int UpnpRegisterClient(
	Upnp_FunPtr Fun, const void *Cookie, UpnpClient_Handle *Hnd)
{
	struct Handle_Info *HInfo;

	if (UpnpSdkInit != 1)
		return UPNP_E_FINISH;
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpRegisterClient \n");
	if (Fun == NULL || Hnd == NULL)
		return UPNP_E_INVALID_PARAM;

	HandleLock();
	if ((NUM_HANDLE - 1) <=
		(UpnpSdkClientRegistered + UpnpSdkDeviceRegisteredV4 +
			UpnpSdkDeviceregisteredV6)) {
		HandleUnlock();
		return UPNP_E_ALREADY_REGISTERED;
	}
	if ((*Hnd = GetFreeHandle()) == UPNP_E_OUTOF_HANDLE) {
		HandleUnlock();
		return UPNP_E_OUTOF_MEMORY;
	}
	HInfo = (struct Handle_Info *)malloc(sizeof(struct Handle_Info));
	if (HInfo == NULL) {
		HandleUnlock();
		return UPNP_E_OUTOF_MEMORY;
	}
	HInfo->HType = HND_CLIENT;
	HInfo->Callback = Fun;
	HInfo->Cookie = (void *)Cookie;
	HInfo->ClientSubList = NULL;
	ListInit(&HInfo->SsdpSearchList, NULL, NULL);
	#ifdef INCLUDE_DEVICE_APIS
	HInfo->MaxAge = 0;
	HInfo->MaxSubscriptions = UPNP_INFINITE;
	HInfo->MaxSubscriptionTimeOut = UPNP_INFINITE;
	#endif
	HandleTable[*Hnd] = HInfo;
	UpnpSdkClientRegistered += 1;
	HandleUnlock();

	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Exiting UpnpRegisterClient \n");

	return UPNP_E_SUCCESS;
}
#endif /* INCLUDE_CLIENT_APIS */

#ifdef INCLUDE_CLIENT_APIS
int UpnpUnRegisterClient(UpnpClient_Handle Hnd)
{
	struct Handle_Info *HInfo;
	ListNode *node = NULL;
	SsdpSearchArg *searchArg = NULL;

	if (UpnpSdkInit != 1)
		return UPNP_E_FINISH;
	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Inside UpnpUnRegisterClient \n");

	HandleLock();
	if (!UpnpSdkClientRegistered) {
		HandleUnlock();
		return UPNP_E_INVALID_HANDLE;
	}
	HandleUnlock();

	#if EXCLUDE_GENA == 0
	if (genaUnregisterClient(Hnd) != UPNP_E_SUCCESS)
		return UPNP_E_INVALID_HANDLE;
	#endif
	HandleLock();
	switch (GetHandleInfo(Hnd, &HInfo)) {
	case HND_INVALID:
		HandleUnlock();
		return UPNP_E_INVALID_HANDLE;
	default:
		break;
	}
	/* clean up search list */
	node = ListHead(&HInfo->SsdpSearchList);
	while (node != NULL) {
		searchArg = (SsdpSearchArg *)node->item;
		if (searchArg) {
			free(searchArg->searchTarget);
			free(searchArg);
		}
		ListDelNode(&HInfo->SsdpSearchList, node, 0);
		node = ListHead(&HInfo->SsdpSearchList);
	}
	ListDestroy(&HInfo->SsdpSearchList, 0);
	FreeHandle(Hnd);
	UpnpSdkClientRegistered -= 1;
	HandleUnlock();

	UpnpPrintf(UPNP_ALL,
		API,
		__FILE__,
		__LINE__,
		"Exiting UpnpUnRegisterClient \n");

	return UPNP_E_SUCCESS;
}
#endif /* INCLUDE_CLIENT_APIS */
