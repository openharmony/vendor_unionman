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

/* @} UPnPAPI */
