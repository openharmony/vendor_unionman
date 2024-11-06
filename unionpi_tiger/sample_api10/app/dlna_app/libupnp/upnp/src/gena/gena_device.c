/*******************************************************************************
 *
 * Copyright (c) 2000-2003 Intel Corporation
 * All rights reserved.
 * Copyright (c) 2012 France Telecom All rights reserved.
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
 * \file
 */

#include "config.h"

#include "gena_device.h"

#if EXCLUDE_GENA == 0
	#ifdef INCLUDE_DEVICE_APIS

		#include <assert.h>

		#include "gena.h"
		#include "httpreadwrite.h"
		#include "parsetools.h"
		#include "ssdplib.h"
		#include "statcodes.h"
		#include "sysdep.h"
		#include "unixutil.h"
		#include "upnpapi.h"
		#include "uuid.h"
		#include "posix_overwrites.h"

		#define STALE_JOBID (INVALID_JOB_ID - 1)

/*!
 * \brief Unregisters a device.
 *
 * \return UPNP_E_SUCCESS on success, GENA_E_BAD_HANDLE on failure.
 */
int genaUnregisterDevice(
	/*! [in] Device handle. */
	UpnpDevice_Handle device_handle)
{
	int ret = 0;
	struct Handle_Info *handle_info;

	HandleLock();
	if (GetHandleInfo(device_handle, &handle_info) != HND_DEVICE) {
		UpnpPrintf(UPNP_CRITICAL,
			GENA,
			__FILE__,
			__LINE__,
			"genaUnregisterDevice: BAD Handle: %d\n",
			device_handle);
		ret = GENA_E_BAD_HANDLE;
	} else {
		freeServiceTable(&handle_info->ServiceTable);
		ret = UPNP_E_SUCCESS;
	}
	HandleUnlock();

	return ret;
}

/*!
 * \brief Generates XML property set for notifications.
 *
 * \return UPNP_E_SUCCESS if successful else returns GENA_E_BAD_HANDLE.
 *
 * \note The XML_VERSION comment is NOT sent due to interoperability issues
 * 	with other UPnP vendors.
 */
static int GeneratePropertySet(
	/*! [in] Array of variable names (go in the event notify). */
	char **names,
	/*! [in] Array of variable values (go in the event notify). */
	char **values,
	/*! [in] number of variables. */
	int count,
	/*! [out] PropertySet node in the string format. */
	DOMString *out)
{
	char *buffer;
	int counter = 0;
	size_t size = 0;

	/*size += strlen(XML_VERSION);*/
	size += strlen(XML_PROPERTYSET_HEADER);
	size += strlen("</e:propertyset>\n\n");
	for (counter = 0; counter < count; counter++) {
		size += strlen("<e:property>\n</e:property>\n");
		size += 2 * strlen(names[counter]) + strlen(values[counter]) +
			strlen("<></>\n");
	}

	buffer = (char *)malloc(size + 1);
	if (buffer == NULL)
		return UPNP_E_OUTOF_MEMORY;
	memset(buffer, 0, size + 1);
	/*
	strcpy(buffer,XML_VERSION);
	strcat(buffer, XML_PROPERTYSET_HEADER);
	*/
	strcpy(buffer, XML_PROPERTYSET_HEADER);
	for (counter = 0; counter < count; counter++) {
		strcat(buffer, "<e:property>\n");
		sprintf(&buffer[strlen(buffer)],
			"<%s>%s</%s>\n</e:property>\n",
			names[counter],
			values[counter],
			names[counter]);
	}
	strcat(buffer, "</e:propertyset>\n\n");
	*out = ixmlCloneDOMString(buffer);
	free(buffer);

	return XML_SUCCESS;
}

/*!
 * \brief Frees memory used in notify_threads if the reference count is 0,
 * otherwise decrements the refrence count.
 */
static void free_notify_struct(
	/*! [in] Notify structure. */
	notify_thread_struct *input)
{
	(*input->reference_count)--;
	if (*input->reference_count == 0) {
		free(input->headers);
		ixmlFreeDOMString(input->propertySet);
		free(input->servId);
		free(input->UDN);
		free(input->reference_count);
	}
	free(input);
}

/*!
 * \brief Sends the notify message and returns a reply.
 *
 * \return on success returns UPNP_E_SUCCESS, otherwise returns a UPNP error.
 *
 * \note called by genaNotify
 */
static UPNP_INLINE int notify_send_and_recv(
	/*! [in] subscription callback URL (URL of the control point). */
	uri_type *destination_url,
	/*! [in] Common HTTP headers. */
	membuffer *mid_msg,
	/*! [in] The evented XML. */
	char *propertySet,
	/*! [out] The response from the control point. */
	http_parser_t *response)
{
	uri_type url;
	SOCKET conn_fd;
	membuffer start_msg;
	int ret_code;
	int err_code;
	int timeout;
	SOCKINFO info;
	const char *CRLF = "\r\n";

	/* connect */
	UpnpPrintf(UPNP_ALL,
		GENA,
		__FILE__,
		__LINE__,
		"gena notify to: %.*s\n",
		(int)destination_url->hostport.text.size,
		destination_url->hostport.text.buff);

	conn_fd = http_Connect(destination_url, &url);
	if (conn_fd < 0)
		/* return UPNP error */
		return UPNP_E_SOCKET_CONNECT;
	ret_code = sock_init(&info, conn_fd);
	if (ret_code) {
		sock_destroy(&info, SD_BOTH);
		return ret_code;
	}
	/* make start line and HOST header */
	membuffer_init(&start_msg);
	if (http_MakeMessage(&start_msg,
		    1,
		    1,
		    "q"
		    "s",
		    HTTPMETHOD_NOTIFY,
		    &url,
		    mid_msg->buf) != 0) {
		membuffer_destroy(&start_msg);
		sock_destroy(&info, SD_BOTH);
		return UPNP_E_OUTOF_MEMORY;
	}
	timeout = GENA_NOTIFICATION_SENDING_TIMEOUT;
	/* send msg (note: end of notification will contain "\r\n" twice) */
	ret_code = http_SendMessage(&info,
		&timeout,
		"bbb",
		start_msg.buf,
		start_msg.length,
		propertySet,
		strlen(propertySet),
		CRLF,
		strlen(CRLF));
	if (ret_code) {
		membuffer_destroy(&start_msg);
		sock_destroy(&info, SD_BOTH);
		return ret_code;
	}
	timeout = GENA_NOTIFICATION_ANSWERING_TIMEOUT;
	ret_code = http_RecvMessage(
		&info, response, HTTPMETHOD_NOTIFY, &timeout, &err_code);
	if (ret_code) {
		membuffer_destroy(&start_msg);
		sock_destroy(&info, SD_BOTH);
		httpmsg_destroy(&response->msg);
		return ret_code;
	}
	/* should shutdown completely when closing socket */
	sock_destroy(&info, SD_BOTH);
	membuffer_destroy(&start_msg);

	return UPNP_E_SUCCESS;
}

/*!
 * \brief Function to Notify a particular subscription of a particular event.
 *
 * In general the service should NOT be blocked around this call (this may
 * cause deadlock with a client).
 *
 * NOTIFY http request is sent and the reply is processed.
 *
 * \return GENA_SUCCESS if the event was delivered, otherwise returns the
 * 	appropriate error code.
 */
static int genaNotify(
	/*! [in] Null terminated, includes all headers (including \\r\\n) except
	   SID and SEQ. */
	char *headers,
	/*! [in] The evented XML. */
	char *propertySet,
	/*! [in] subscription to be Notified, assumes this is valid for life of
	   function. */
	subscription *sub)
{
	size_t i;
	membuffer mid_msg;
	uri_type *url;
	http_parser_t response;
	int return_code = -1;

	membuffer_init(&mid_msg);
	if (http_MakeMessage(&mid_msg,
		    1,
		    1,
		    "s"
		    "ssc"
		    "sdcc",
		    headers,
		    "SID: ",
		    sub->sid,
		    "SEQ: ",
		    sub->ToSendEventKey) != 0) {
		membuffer_destroy(&mid_msg);
		return UPNP_E_OUTOF_MEMORY;
	}
	/* send a notify to each url until one goes thru */
	for (i = 0; i < sub->DeliveryURLs.size; i++) {
		url = &sub->DeliveryURLs.parsedURLs[i];
		return_code = notify_send_and_recv(
			url, &mid_msg, propertySet, &response);
		if (return_code == UPNP_E_SUCCESS)
			break;
	}
	membuffer_destroy(&mid_msg);
	if (return_code == UPNP_E_SUCCESS) {
		if (response.msg.status_code == HTTP_OK)
			return_code = GENA_SUCCESS;
		else {
			if (response.msg.status_code ==
				HTTP_PRECONDITION_FAILED)
				/*Invalid SID gets removed */
				return_code =
					GENA_E_NOTIFY_UNACCEPTED_REMOVE_SUB;
			else
				return_code = GENA_E_NOTIFY_UNACCEPTED;
		}
		httpmsg_destroy(&response.msg);
	}

	return return_code;
}

/*!
 * \brief Thread job to Notify a control point.
 *
 * It validates the subscription and copies the subscription. Also make sure
 * that events are sent in order.
 *
 * \note calls the genaNotify to do the actual work.
 */
static void genaNotifyThread(
	/*! [in] notify thread structure containing all the headers and property
	   set info. */
	void *input)
{
	subscription *sub;
	service_info *service;
	subscription sub_copy;
	notify_thread_struct *in = (notify_thread_struct *)input;
	int return_code;
	struct Handle_Info *handle_info;

	/* This should be a HandleLock and not a HandleReadLock otherwise if
	 * there is a lot of notifications, then multiple threads will acquire a
	 * read lock and the thread which sends the notification will be blocked
	 * forever on the HandleLock at the end of this function. */
	/*HandleReadLock(); */
	HandleLock();
	/* validate context */

	if (GetHandleInfo(in->device_handle, &handle_info) != HND_DEVICE) {
		free_notify_struct(in);
		HandleUnlock();
		return;
	}

	if (!(service = FindServiceId(
		      &handle_info->ServiceTable, in->servId, in->UDN)) ||
		!service->active ||
		!(sub = GetSubscriptionSID(in->sid, service)) ||
		copy_subscription(sub, &sub_copy) != HTTP_SUCCESS) {
		free_notify_struct(in);
		HandleUnlock();
		return;
	}

	HandleUnlock();

	/* send the notify */
	return_code = genaNotify(in->headers, in->propertySet, &sub_copy);
	freeSubscription(&sub_copy);
	HandleLock();
	if (GetHandleInfo(in->device_handle, &handle_info) != HND_DEVICE) {
		free_notify_struct(in);
		HandleUnlock();
		return;
	}
	/* validate context */
	if (!(service = FindServiceId(
		      &handle_info->ServiceTable, in->servId, in->UDN)) ||
		!service->active ||
		!(sub = GetSubscriptionSID(in->sid, service))) {
		free_notify_struct(in);
		HandleUnlock();
		return;
	}
	sub->ToSendEventKey++;
	if (sub->ToSendEventKey < 0)
		/* wrap to 1 for overflow */
		sub->ToSendEventKey = 1;

	/* Remove head of event queue. Possibly activate next */
	{
		ListNode *node = ListHead(&sub->outgoing);
		if (node)
			ListDelNode(&sub->outgoing, node, 1);
		if (ListSize(&sub->outgoing) > 0) {
			ThreadPoolJob *job;
			ListNode *node = ListHead(&sub->outgoing);
			job = (ThreadPoolJob *)node->item;
			/* The new head of queue should not have already been
			   added to the pool, else something is very wrong */
			assert(job->jobId != STALE_JOBID);

			ThreadPoolAdd(&gSendThreadPool, job, NULL);
			job->jobId = STALE_JOBID;
		}
	}

	if (return_code == GENA_E_NOTIFY_UNACCEPTED_REMOVE_SUB)
		RemoveSubscriptionSID(in->sid, service);
	free_notify_struct(in);

	HandleUnlock();
}

/*!
 * \brief Allocates the GENA header.
 *
 * \note The header must be destroyed after with a call to free(), otherwise
 * there will be a memory leak.
 *
 * \return The constructed header.
 */
static char *AllocGenaHeaders(
	/*! [in] The property set string. */
	const DOMString propertySet)
{
	static const char *HEADER_LINE_1 =
		"CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n";
	static const char *HEADER_LINE_2A = "CONTENT-LENGTH: ";
	static const char *HEADER_LINE_2B = "\r\n";
	static const char *HEADER_LINE_3 = "NT: upnp:event\r\n";
	static const char *HEADER_LINE_4 = "NTS: upnp:propchange\r\n";
	char *headers = NULL;
	size_t headers_size = 0;
	int line = 0;
	int rc = 0;

	headers_size = strlen(HEADER_LINE_1) + strlen(HEADER_LINE_2A) +
		       MAX_CONTENT_LENGTH + strlen(HEADER_LINE_2B) +
		       strlen(HEADER_LINE_3) + strlen(HEADER_LINE_4) + 1;
	headers = (char *)malloc(headers_size);
	if (headers == NULL) {
		line = __LINE__;
		goto ExitFunction;
	}
	rc = snprintf(headers,
		headers_size,
		"%s%s%" PRIzu "%s%s%s",
		HEADER_LINE_1,
		HEADER_LINE_2A,
		//(unsigned long)strlen(propertySet) + 2,
		(unsigned int)strlen(propertySet) + 2,
		HEADER_LINE_2B,
		HEADER_LINE_3,
		HEADER_LINE_4);

ExitFunction:
	if (headers == NULL || rc < 0 || (unsigned int)rc >= headers_size) {
		UpnpPrintf(UPNP_ALL,
			GENA,
			__FILE__,
			line,
			"AllocGenaHeaders(): Error UPNP_E_OUTOF_MEMORY\n");
	}
	return headers;
}

void freeSubscriptionQueuedEvents(subscription *sub)
{
	if (ListSize(&sub->outgoing) > 0) {
		/* The first event is discarded without dealing
		   notify_thread_struct: there is a mirror ThreadPool entry for
		   this one, and it will take care of the refcount etc. Other
		   entries must be fully cleaned-up here */
		int first = 1;
		ListNode *node = ListHead(&sub->outgoing);
		while (node) {
			ThreadPoolJob *job = (ThreadPoolJob *)node->item;
			if (first) {
				first = 0;
			} else {
				free_notify_struct(
					(notify_thread_struct *)job->arg);
			}
			free(node->item);
			ListDelNode(&sub->outgoing, node, 0);
			node = ListHead(&sub->outgoing);
		}
	}
}

/* We take ownership of propertySet and will free it */
static int genaInitNotifyCommon(UpnpDevice_Handle device_handle,
	char *UDN,
	char *servId,
	DOMString propertySet,
	const Upnp_SID sid)
{
	int ret = GENA_SUCCESS;
	int line = 0;

	int *reference_count = NULL;
	char *UDN_copy = NULL;
	char *servId_copy = NULL;
	char *headers = NULL;
	notify_thread_struct *thread_struct = NULL;

	subscription *sub = NULL;
	service_info *service = NULL;
	struct Handle_Info *handle_info;
	ThreadPoolJob *job = NULL;

	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		__LINE__,
		"GENA BEGIN INITIAL NOTIFY COMMON\n");

	job = (ThreadPoolJob *)malloc(sizeof(ThreadPoolJob));
	if (job == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}
	memset(job, 0, sizeof(ThreadPoolJob));

	reference_count = (int *)malloc(sizeof(int));
	if (reference_count == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}
	*reference_count = 0;

	UDN_copy = strdup(UDN);
	if (UDN_copy == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}

	servId_copy = strdup(servId);
	if (servId_copy == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}

	HandleLock();

	if (GetHandleInfo(device_handle, &handle_info) != HND_DEVICE) {
		line = __LINE__;
		ret = GENA_E_BAD_HANDLE;
		goto ExitFunction;
	}

	service = FindServiceId(&handle_info->ServiceTable, servId, UDN);
	if (service == NULL) {
		line = __LINE__;
		ret = GENA_E_BAD_SERVICE;
		goto ExitFunction;
	}
	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		__LINE__,
		"FOUND SERVICE IN INIT NOTFY: UDN %s, ServID: %s",
		UDN,
		servId);

	sub = GetSubscriptionSID(sid, service);
	if (sub == NULL || sub->active) {
		line = __LINE__;
		ret = GENA_E_BAD_SID;
		goto ExitFunction;
	}
	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		__LINE__,
		"FOUND SUBSCRIPTION IN INIT NOTIFY: SID %s",
		sid);
	sub->active = 1;

	headers = AllocGenaHeaders(propertySet);
	if (headers == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}

	/* schedule thread for initial notification */

	thread_struct =
		(notify_thread_struct *)malloc(sizeof(notify_thread_struct));
	if (thread_struct == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
	} else {
		*reference_count = 1;
		thread_struct->servId = servId_copy;
		thread_struct->UDN = UDN_copy;
		thread_struct->headers = headers;
		thread_struct->propertySet = propertySet;
		memset(thread_struct->sid, 0, sizeof(thread_struct->sid));
		strncpy(thread_struct->sid,
			sid,
			sizeof(thread_struct->sid) - 1);
		thread_struct->ctime = time(0);
		thread_struct->reference_count = reference_count;
		thread_struct->device_handle = device_handle;

		TPJobInit(job, (start_routine)genaNotifyThread, thread_struct);
		TPJobSetFreeFunction(job, (free_routine)free_notify_struct);
		TPJobSetPriority(job, MED_PRIORITY);

		ret = ThreadPoolAdd(&gSendThreadPool, job, NULL);
		if (ret != 0) {
			if (ret == EOUTOFMEM) {
				line = __LINE__;
				ret = UPNP_E_OUTOF_MEMORY;
			}
		} else {
			ListNode *node = ListAddTail(&sub->outgoing, job);
			if (node != NULL) {
				((ThreadPoolJob *)node->item)->jobId =
					STALE_JOBID;
				line = __LINE__;
				ret = GENA_SUCCESS;
			} else {
				line = __LINE__;
				ret = UPNP_E_OUTOF_MEMORY;
			}
		}
	}

ExitFunction:
	if (ret != GENA_SUCCESS) {
		free(job);
		free(thread_struct);
		free(headers);
		ixmlFreeDOMString(propertySet);
		free(servId_copy);
		free(UDN_copy);
		free(reference_count);
	}

	HandleUnlock();

	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		line,
		"GENA END INITIAL NOTIFY COMMON, ret = %d\n",
		ret);

	return ret;
}

int genaInitNotify(UpnpDevice_Handle device_handle,
	char *UDN,
	char *servId,
	char **VarNames,
	char **VarValues,
	int var_count,
	const Upnp_SID sid)
{
	int ret = GENA_SUCCESS;
	int line = 0;
	DOMString propertySet = NULL;

	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		__LINE__,
		"GENA BEGIN INITIAL NOTIFY\n");

	if (var_count <= 0) {
		line = __LINE__;
		ret = GENA_SUCCESS;
		goto ExitFunction;
	}

	ret = GeneratePropertySet(VarNames, VarValues, var_count, &propertySet);
	if (ret != XML_SUCCESS) {
		line = __LINE__;
		goto ExitFunction;
	}
	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		__LINE__,
		"GENERATED PROPERTY SET IN INIT NOTIFY: %s",
		propertySet);

	ret = genaInitNotifyCommon(
		device_handle, UDN, servId, propertySet, sid);

ExitFunction:

	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		line,
		"GENA END INITIAL NOTIFY, ret = %d\n",
		ret);

	return ret;
}

int genaInitNotifyExt(UpnpDevice_Handle device_handle,
	char *UDN,
	char *servId,
	IXML_Document *PropSet,
	const Upnp_SID sid)
{
	int ret = GENA_SUCCESS;
	int line = 0;

	DOMString propertySet = NULL;

	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		__LINE__,
		"GENA BEGIN INITIAL NOTIFY EXT\n");

	if (PropSet == 0) {
		line = __LINE__;
		ret = GENA_SUCCESS;
		goto ExitFunction;
	}

	propertySet = ixmlPrintNode((IXML_Node *)PropSet);
	if (propertySet == NULL) {
		line = __LINE__;
		ret = UPNP_E_INVALID_PARAM;
		goto ExitFunction;
	}
	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		__LINE__,
		"GENERATED PROPERTY SET IN INIT EXT NOTIFY: %s",
		propertySet);

	ret = genaInitNotifyCommon(
		device_handle, UDN, servId, propertySet, sid);

ExitFunction:

	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		line,
		"GENA END INITIAL NOTIFY EXT, ret = %d\n",
		ret);

	return ret;
}

/*
 * This gets called before queuing a new event.
 * - The list size can never go over MAX_SUBSCRIPTION_QUEUED_EVENTS so we
 *   discard the oldest non-active event if it is already at the max
 * - We also discard any non-active event older than MAX_SUBSCRIPTION_EVENT_AGE.
 * non-active: any but the head of queue, which is already copied to
 * the thread pool
 */
static void maybeDiscardEvents(LinkedList *listp)
{
	time_t now = time(0L);
	notify_thread_struct *ntsp;

	while (ListSize(listp) > 1) {
		ListNode *node = ListHead(listp);
		/* The first candidate is the second event: first non-active */
		if (node == 0 || (node = node->next) == 0) {
			/* Major inconsistency, really, should abort here. */
			fprintf(stderr,
				"gena_device: maybeDiscardEvents: "
				"list is inconsistent\n");
			break;
		}

		ntsp = (notify_thread_struct *)(((ThreadPoolJob *)node->item)
							->arg);
		if (ListSize(listp) > g_UpnpSdkEQMaxLen ||
			now - ntsp->ctime > g_UpnpSdkEQMaxAge) {
			free_notify_struct(ntsp);
			free(node->item);
			ListDelNode(listp, node, 0);
		} else {
			/* If the list is smaller than the max and the oldest
			 * task is young enough, stop pruning */
			break;
		}
	}
}

/* We take ownership of propertySet and will free it */
static int genaNotifyAllCommon(UpnpDevice_Handle device_handle,
	char *UDN,
	char *servId,
	DOMString propertySet)
{
	int ret = GENA_SUCCESS;
	int line = 0;

	int *reference_count = NULL;
	char *UDN_copy = NULL;
	char *servId_copy = NULL;
	char *headers = NULL;
	notify_thread_struct *thread_s = NULL;

	subscription *finger = NULL;
	service_info *service = NULL;
	struct Handle_Info *handle_info;

	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		__LINE__,
		"GENA BEGIN NOTIFY ALL COMMON\n");

	/* Keep this allocation first */
	reference_count = (int *)malloc(sizeof(int));
	if (reference_count == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}
	*reference_count = 0;

	UDN_copy = strdup(UDN);
	if (UDN_copy == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}

	servId_copy = strdup(servId);
	if (servId_copy == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}

	headers = AllocGenaHeaders(propertySet);
	if (headers == NULL) {
		line = __LINE__;
		ret = UPNP_E_OUTOF_MEMORY;
		goto ExitFunction;
	}

	HandleLock();

	if (GetHandleInfo(device_handle, &handle_info) != HND_DEVICE) {
		line = __LINE__;
		ret = GENA_E_BAD_HANDLE;
	} else {
		service =
			FindServiceId(&handle_info->ServiceTable, servId, UDN);
		if (service != NULL) {
			finger = GetFirstSubscription(service);
			while (finger) {
				ThreadPoolJob *job = NULL;
				ListNode *node;

				thread_s = (notify_thread_struct *)malloc(
					sizeof(notify_thread_struct));
				if (thread_s == NULL) {
					line = __LINE__;
					ret = UPNP_E_OUTOF_MEMORY;
					break;
				}

				(*reference_count)++;
				thread_s->reference_count = reference_count;
				thread_s->UDN = UDN_copy;
				thread_s->servId = servId_copy;
				thread_s->headers = headers;
				thread_s->propertySet = propertySet;
				strncpy(thread_s->sid,
					finger->sid,
					sizeof thread_s->sid);
				thread_s->sid[sizeof thread_s->sid - 1] = 0;
				thread_s->ctime = time(0);
				thread_s->device_handle = device_handle;

				maybeDiscardEvents(&finger->outgoing);
				job = (ThreadPoolJob *)malloc(
					sizeof(ThreadPoolJob));
				if (!job) {
					free(thread_s);
					line = __LINE__;
					ret = UPNP_E_OUTOF_MEMORY;
					break;
				}
				memset(job, 0, sizeof(ThreadPoolJob));
				TPJobInit(job,
					(start_routine)genaNotifyThread,
					thread_s);
				TPJobSetFreeFunction(
					job, (free_routine)free_notify_struct);
				TPJobSetPriority(job, MED_PRIORITY);
				node = ListAddTail(&finger->outgoing, job);

				/* If there is only one element on the list
				   (which we just
				   added), need to kickstart the threadpool */
				if (ListSize(&finger->outgoing) == 1) {
					ret = ThreadPoolAdd(
						&gSendThreadPool, job, NULL);
					if (ret != 0) {
						line = __LINE__;
						if (ret == EOUTOFMEM) {
							line = __LINE__;
							ret = UPNP_E_OUTOF_MEMORY;
						}
						break;
					}
					if (node) {
						((ThreadPoolJob *)(node->item))
							->jobId = STALE_JOBID;
					}
				}
				finger = GetNextSubscription(service, finger);
			}
		} else {
			line = __LINE__;
			ret = GENA_E_BAD_SERVICE;
		}
	}

ExitFunction:
	/* The only case where we want to free memory here is if the
	   struct was never queued. Else, let the normal cleanup take place.
	   reference_count is allocated first so it's ok to do nothing if it's 0
	*/
	if (reference_count && *reference_count == 0) {
		free(headers);
		ixmlFreeDOMString(propertySet);
		free(servId_copy);
		free(UDN_copy);
		free(reference_count);
	}

	HandleUnlock();

	UpnpPrintf(UPNP_INFO,
		GENA,
		__FILE__,
		line,
		"GENA END NOTIFY ALL COMMON, ret = %d\n",
		ret);

	return ret;
}
	#endif /* INCLUDE_DEVICE_APIS */
#endif	       /* EXCLUDE_GENA */
