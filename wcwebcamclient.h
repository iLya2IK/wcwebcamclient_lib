/*! \file wcwebcamclient.h
 */

#ifndef WCWEBCAMCLIENT_H
#define WCWEBCAMCLIENT_H

#define COMMON_FUNC_ONLY
#ifdef COMMON_FUNC_ONLY
#include "wcCommonCallbacks.h"
#else
#include "wcCallbacks.h"
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef _ANDROID_COMPILER_
#define DLLEXPORT
#else
#ifdef DLL_BUILD
#if defined(_MSC_VER)
    //  Microsoft
    #define DLLEXPORT __declspec(dllexport)
#elif defined(__GNUC__)
    //  GCC
    #ifdef __WINDOWS__
    #define DLLEXPORT __attribute__ ((dllexport))
    #else
    #define DLLEXPORT __attribute__ ((visibility ("default")))
    #endif
#endif
#else
#ifdef __cplusplus
#include <cstddef>
#endif
#if defined(_MSC_VER)
    //  Microsoft
    #define DLLEXPORT __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define DLLEXPORT
#endif
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup wcReturn Library typedefs and consts for returned results
 * @{
 */
//! \brief Boolean operation completion codes.
/*!
 *  \sa WC_TRUE, WC_FALSE
 */
typedef int wcStateVal;

//! \brief Operation completion codes.
/*!
 *  \sa wcStateVal, WC_OK, WC_BAD_TARGET, WC_BAD_PARAM, WC_BAD_VALUE, WC_CLIENT_NOT_STREAMING, WC_NOT_A_TASK, WC_TASK_MISMATCH, WC_NOT_ENOUGH_MEM
 */
typedef int wcRCode;

/** The result is 'true' for this operation. */
const wcRCode WC_TRUE = 1;

/** The result is 'false' for this operation. */
const wcRCode WC_FALSE = 0;

/** No error. */
const wcRCode WC_OK = 0;

/** Invalid handle is specified. */
const wcRCode WC_BAD_TARGET = -1;

/** The enumeration value is invalid. */
const wcRCode WC_BAD_PARAM = -2;

/** The parameter value is invalid. */
const wcRCode WC_BAD_VALUE = -3;

/** This client is not streaming. */
const wcRCode WC_CLIENT_NOT_STREAMING = -4;

/** An invalid pointer to the task was passed. */
const wcRCode WC_NOT_A_TASK = -5;

/** This operation is not allowed for the selected task. */
const wcRCode WC_TASK_MISMATCH = -6;

/** There is not enough memory to complete the operation successfully. */
const wcRCode WC_NOT_ENOUGH_MEM  = -7;
/*!@}*/

//! \brief Client handle
typedef int wcHandle;

//! \brief Pointer to task
typedef wcCallbackTask wcTask;

//! \brief Task class
typedef uint32_t wcTaskClass;

//! \brief The Base task class
const wcTaskClass WC_BASE_TASK = 0;
//! \brief The Simple task class (for POST requests)
const wcTaskClass WC_TASK = 1;
//! \brief The Input-stream task class (for GET requests)
const wcTaskClass WC_IN_STREAM_TASK = 2;
//! \brief The Output-stream task class (for UPLOAD requests)
const wcTaskClass WC_OUT_STREAM_TASK = 4;
//! \brief All tasks classes (mask)
const wcTaskClass WC_ALL_TASKS = 0xffffffff;

//! \brief The size of frame header (6 bytes)
const uint16_t WC_STREAM_FRAME_HEADER_SIZE = sizeof(uint16_t) + sizeof(uint32_t);
//! \brief The frame header sequence
const uint16_t WC_FRAME_START_SEQ = 0xaaaa;

/*!
 * \defgroup wcClientCallbacks Library functions and typedefs to set callbacks for client
 * @{
 */

#ifdef _MSC_VER
__pragma( pack(push, 1) )
#endif

//! \brief The list of callbacks
typedef enum wcCallback {
    /* maintaining block */
    wccbkInitCURL=0,               /*!< Successful initialization of the multiCURL handle. \sa wcSetNotifyCallback */
    wccbkSuccessAuth,              /*!< Successful authorization. This callback is called during \ref wcClientTasksProceed. \sa wcSetTaskCallback */
    wccbkConnected,                /*!< The connection state has been changed. \sa wcSetConnCallback */
    wccbkDisconnect,               /*!< Client has been disconnected. \sa wcSetNotifyCallback */
    wccbkSIDSetted,                /*!< The session id has been changed. \sa wcSetCStringCallback */
    wccbkAddLog,                   /*!< Added new log entry. \sa wcSetCStringCallback */
    wccbkSynchroUpdateTask,        /*!< The IO streaming task signals that a change has occurred. This callback is called during \ref wcClientTasksProceed. \sa wcSetTaskCallback */

    /* streams block */
    wccbkAfterLaunchInStream,      /*!< Incoming stream started. \sa wcSetTaskCallback */
    wccbkAfterLaunchOutStream,     /*!< Outgoing stream started. \sa wcSetTaskCallback */
    wccbkSuccessIOStream,          /*!< IO stream terminated for some reason. This callback is called during \ref wcClientTasksProceed. \sa wcSetTaskCallback */

    /* data blobs block */
    wccbkSuccessSaveRecord,        /*!< The request to save the media record has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetTaskCallback */
    wccbkSuccessRequestRecord,     /*!< The request to get the media record has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetDataCallback, wcSetAltDataCallback */

    /* JSON block */
    wccbkSuccessUpdateRecords,     /*!< The request to update list of records has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetJSONStrCallback, wcSetcJSONCallback, wcSetJSONCallback */
    wccbkSuccessUpdateDevices,     /*!< The request to update list of online devices has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetJSONStrCallback, wcSetcJSONCallback, wcSetJSONCallback  */
    wccbkSuccessUpdateStreams,     /*!< The request to update list of streaming devices has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetJSONStrCallback, wcSetcJSONCallback, wcSetJSONCallback  */
    wccbkSuccessUpdateMsgs,        /*!< The request to update list of messages has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetJSONStrCallback, wcSetcJSONCallback, wcSetJSONCallback  */
    wccbkSuccessSendMsg,           /*!< The request to send message has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetJSONStrCallback, wcSetcJSONCallback, wcSetJSONCallback  */
    wccbkSuccessRequestRecordMeta, /*!< The request to get metadata for the media record has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetJSONStrCallback, wcSetcJSONCallback, wcSetJSONCallback  */
    wccbkSuccessGetConfig,         /*!< The request to get actual config has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetJSONStrCallback, wcSetcJSONCallback, wcSetJSONCallback  */
    wccbkSuccessDeleteRecords,     /*!< The request to delete records has been completed. The response has arrived. This callback is called during \ref wcClientTasksProceed. \sa wcSetJSONStrCallback, wcSetcJSONCallback, wcSetJSONCallback  */

    wccbkLast}
#ifdef __GNUC__
__attribute__((__packed__))
#endif
wcCallback;
/*!@}*/

/*!
 * \defgroup wcClientGettersSetters Library functions and typedefs to get or set state values for client and tasks
 * @{
 */
typedef enum wcStateId {
    wcstConnection = 0,
    wcstVerifyTLS,
    wcstError,
    wcstLog,
    wcstStreaming,
    wcstStreams,
    wcstDevices,
    wcstRecords,
    wcstRecordsStamp,
    wcstMsgs,
    wcstMsgsStamp,
    wcstMetaData,
    wcstDeviceName,
    wcstSID,
    wcstHostName,
    wcstProxy,
    wcstProxyAuth,
    wcstProxyProtocol,
    wcstProxyHost,
    wcstProxyPort,
    wcstProxyUser,
    wcstProxyPwrd  }
#ifdef __GNUC__
__attribute__((__packed__))
#endif
wcStateId;

typedef enum wcTaskStateId {
    wctstError = 0,
    wctstPath,
    wctstSubProto,
    wctstDeviceName}
#ifdef __GNUC__
__attribute__((__packed__))
#endif
wcTaskStateId;
/*!@}*/

#ifdef _MSC_VER
__pragma( pack(pop))
#endif

/*!
 * \addtogroup wcClientCallbacks
 * @{
 */
/*! \brief Notification callback.
 *  \param client The client handle.
 */
typedef void (*EmptyNotifyLibFunc) (wcHandle client);
/*! \brief Notification callback with an additional data.
 *  \param client   The client handle.
 *  \param data     The additional data.
 */
typedef void (*NotifyEventLibFunc) (wcHandle client, void * data);
/*! \brief Notification callback from the specific task.
 *  \param client   The client handle.
 *  \param tsk      The task that is the source of the notification.
 */
typedef void (*TaskNotifyLibFunc) (wcHandle client, wcTask tsk);
/*! \brief Notification callback that the connection status has been changed.
 *  \param client   The client handle.
 *  \param state    The connection state value.
 */
typedef void (*ConnNotifyEventLibFunc) (wcHandle client, int state);
/*! \brief Notification callback that returned a C-style string.
 *  \param client   The client handle.
 *  \param value    The incoming C-style string value.
 */
typedef void (*CStringNotifyLibFunc) (wcHandle client, const char * value);
/*! \brief Notification callback that returned a wcCustomStream.
 *  \param client   The client handle.
 *  \param tsk      The task that is the source of the notification.
 *  \param strm     Pointer to the incoming wcCustomStream object.
 */
typedef void (*DataNotifyEventLibFunc) (wcHandle client, wcTask tsk, void * strm);
/*! \brief Notification callback that returned a sized data buffer.
 *  \param client   The client handle.
 *  \param tsk      The task that is the source of the notification.
 *  \param data     Pointer to the incoming buffer.
 *  \param sz       The size of the buffer
 */
typedef void (*DataAltNotifyEventLibFunc) (wcHandle client, wcTask tsk, void* data, size_t sz);
/*! \brief Notification callback that task is completed and returns a C-style string with a response in JSON format.
 *  \param client   The client handle.
 *  \param tsk      The task that is the source of the notification.
 *  \param jsonStr  The C-style string with a response in JSON format.
 */
typedef void (*JSONStrNotifyEventLibFunc) (wcHandle client, wcTask tsk, const char * jsonStr);

#ifndef COMMON_FUNC_ONLY
/*! \brief Notification callback that task is completed and returns a pointer to the cJSON object with response.
 *  \param client   The client handle.
 *  \param tsk      The task that is the source of the notification.
 *  \param jsonObj  Pointer to the cJSON object with response.
 */
typedef void (*cJSONNotifyEventLibFunc) (wcHandle client, wcTask tsk, const cJSON * jsonObj);;
/*! \brief Notification callback that returned a reference to std::string.
 *  \param client   The client handle.
 *  \param value    The incoming std::string value.
 */
typedef void (*StringNotifyLibFunc) (wcHandle client, const std::string& value);
/*! \brief Notification callback that task is completed and returns a reference to wcJSON object with response.
 *  \param client   The client handle.
 *  \param tsk      The task that is the source of the notification.
 *  \param jsonObj  Reference to the wcJSON object.
 */
typedef void (*JSONNotifyEventLibFunc) (wcHandle client, wcTask tsk, wcJSON& jsonObj);
#endif
/*!@}*/

/*!
 * \defgroup wcClientLifeCircle Library functions to manage client
 * @{
 */
//! \brief Create \a client
/*!
 * \return wcHandle The value of the client handle.
 *
 * \sa wcClientDestroy
 */
wcHandle DLLEXPORT wcClientCreate();
//! \brief Launch \a client.
/*! The function initializes and starts the client's working thread.
 *
 * \param client        The client handle.
 * \return \ref wcRCode \ref WC_OK or error code.
 *
 * \sa wcRCode
 */
wcRCode DLLEXPORT wcClientStart(wcHandle client);
//! \brief Authorize \a client on the server host.
/*! See protocol request \ref authorize.
 *
 * \param client            The client handle.
 * \param aLogin            The name of the user on the server.
 * \param aPwrd             The password of the user on the server.
 * \return \ref wcRCode     \ref WC_OK or error code.
 *
 * \sa wcRCode, wcClientDisconnect
 */
wcRCode DLLEXPORT wcClientAuth(wcHandle client, const char * aLogin, const char * aPwrd);
//! \brief Call the asynchronous \a client update stage.
/*! Thread safe. The function can be called from a separate thread.
 *
 * \param client         The client handle.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode
 */
wcRCode DLLEXPORT wcClientProceed(wcHandle client);
//! \brief Call the synchronous \a client update stage.
/*! \warning Not thread safe! The function must be called from the main or UI thread.
 *
 * \param client         The client handle.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcSetTaskCallback, wccbkSynchroUpdateTask
 */
wcRCode DLLEXPORT wcClientTasksProceed(wcHandle client);
//! \brief Disconnect \a client from the server host.
/*!
 * \param client         The client handle.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcClientCreate, wcClientStart, wcClientAuth, wcClientDestroy
 */
wcRCode DLLEXPORT wcClientDisconnect(wcHandle client);
//! \brief Destroy \a client.
/*!
 * \param client         The client handle.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcClientCreate
 */
wcRCode DLLEXPORT wcClientDestroy(wcHandle client);
/*!@}*/

/*!
 * \addtogroup wcClientGettersSetters
 * @{
 */
//! \brief Reset the selected client state.
/*! Acceptable values of the state param are:<br>
 * \ref wcstLog - clear the log, <br>
 * \ref wcstError - delete information about the last error,<br>
 * \ref wcstDevices - update the list of devices (see also \ref getDevicesOnline),<br>
 * \ref wcstRecords - update the list of media records (see also \ref getRecordCount - according to the results of the request, the wcstRecordsStamp state will be updated automatically),<br>
 * \ref wcstRecordsStamp - clear the timestamp for records (the *stamp* parameter in \ref getRecordCount request),<br>
 * \ref wcstMsgs - update the list of messages (see also \ref getMsgs - according to the results of the request, the wcstMsgsStamp state will be updated automatically),<br>
 * \ref wcstMsgsStamp - clear the timestamp for messages (the *stamp* parameter in \ref getMsgs request),<br>
 * \ref wcstStreams - update the list of streams (see also \ref getStreams),<br>
 * \ref wcstMetaData - clear the metadata,<br>
 * \ref wcstDeviceName - clear the device name,<br>
 * \ref wcstHostName - clear the server address,<br>
 * \ref wcstProxy - clear proxy settings. <br>
 * Resetting wcstDevices, wcstRecords, wcstMsgs, wcstStreams states will update the selected state when \ref wcClientProceed is called.
 * During the execution of \ref wcClientProceed, requests \ref getDevicesOnline, \ref getRecordCount, \ref getMsgs and \ref getStreams will be generated and launched, respectively.
 *
 * \param client          The client handle.
 * \param aStateId        The selected client state.
 * \return \ref wcRCode   \ref WC_OK or error code.
 *
 * \sa wcStateId, wcRCode
 */
wcRCode DLLEXPORT wcClientInvalidateState(wcHandle client, wcStateId aStateId);
//! \brief Get a integer value for the selected client state.
/*! Acceptable values of the state param are:<br>
 * \ref wcstError - get a code of the last error, <br>
 * \ref wcstLog - get a length of the log.<br>
 *
 * \param client          The client handle.
 * \param aStateId        The selected client state.
 * \param aStateVal       Pointer to a variable to which the selected client state will be written.
 * \return \ref wcRCode   \ref WC_OK or error code.
 *
 * \b Example<br>
 * \code
 * int res;
 * wcRCode aCode = wcClientGetIntState(client, wcstError, &res);
 * if (aCode == WC_OK) {
 *    cout << "last error code is " << res << endl;
 * } else {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 *
 * \sa wcStateId, wcRCode, wcStateVal
 */
wcRCode DLLEXPORT wcClientGetIntState(wcHandle client, wcStateId aStateId, int * aStateVal);
//! \brief Get a boolean value for the selected client state.
/*! Acceptable values of the state param are:<br>
 * \ref wcstConnection - is client connected, <br>
 * \ref wcstVerifyTLS - should client verify TLS certificate,  <br>
 * \ref wcstError - an error has occurred, <br>
 * \ref wcstStreaming - has the client started the media stream,<br>
 * \ref wcstLog - does the log contain entries,<br>
 * \ref wcstDevices - should client update the list of devices,<br>
 * \ref wcstRecords - should client update the list of media records,<br>
 * \ref wcstMsgs - should client update the list of messages,<br>
 * \ref wcstStreams - should client update the list of streams.<br>
 *
 * \param client          The client handle.
 * \param aStateId        The selected client state.
 * \return \ref wcRCode   \ref wcStateVal or error code.
 *
 * \sa wcStateId, wcRCode, wcStateVal
 */
wcRCode DLLEXPORT wcClientGetBoolState(wcHandle client, wcStateId aStateId);
//! \brief Set the boolean value to the selected client state.
/*! Acceptable values of the state param are:<br>
 * \ref wcstConnection - disconnect ( \ref WC_FALSE, see also \ref wcClientDisconnect) the client ( \ref WC_TRUE not allowed, use \ref wcClientAuth instead), <br>
 * \ref wcstVerifyTLS - should client verify TLS certificate ( \ref WC_TRUE/ \ref WC_FALSE allowed),  <br>
 * \ref wcstStreaming - stop streaming ( \ref WC_FALSE) for the client ( \ref WC_TRUE not allowed, use \ref wcLaunchOutStream instead),<br>
 * \ref wcstDevices - the client should update the list of devices (only \ref WC_TRUE value allowed),<br>
 * \ref wcstRecords - the client should update the list of media records (only \ref WC_TRUE value allowed),<br>
 * \ref wcstMsgs - the client should update the list of messages (only \ref WC_TRUE value allowed),<br>
 * \ref wcstStreams - the client should update the list of streams (only \ref WC_TRUE value allowed).<br>
 *
 * \param client          The client handle.
 * \param aStateId        The selected client state.
 * \param aStateVal       The boolean value.
 * \return \ref wcRCode   \ref WC_OK or error code.
 *
 * \sa wcStateId, wcRCode, wcStateVal
 */
wcRCode DLLEXPORT wcClientSetBoolState(wcHandle client, wcStateId aStateId, wcStateVal aStateVal);
//! \brief Get a C-style string value for the selected client state.
/*! As a result of successful execution of the function, the \a aStateVal parameter will contain a
 * pointer to NULL-terminated string. (Don't forget to free up memory after using the \a aStateVal).
 * Acceptable values of the state param are:<br>
 * \ref wcstLog - get the first log entry and delete it from the clients log (see also \ref wcSetCStringCallback, \ref wcSetStringCallback), <br>
 * \ref wcstSID - the client session id, <br>
 * \ref wcstRecordsStamp - current timestamp value for records (the *stamp* parameter retrieved from the last \ref getRecordCount response),<br>
 * \ref wcstMsgsStamp - current timestamp value for messages (the *stamp* parameter retrieved from the last \ref getMsgs response),<br>
 * \ref wcstMetaData - the client metadata,  <br>
 * \ref wcstDeviceName - the client device name,  <br>
 * \ref wcstHostName - the client host name, <br>
 * \ref wcstProxy - the client proxy server address in format \code [[protocol://]address[:port]] \endcode ,<br>
 * \ref wcstProxyAuth - the client proxy server authorization data in format \code [username[:password]] \endcode . <br>
 *
 * \param client         The client handle.
 * \param aStateId       The selected client state.
 * \param aStateVal      The pointer to the variable of char * type initialized with NULL value
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \b Example<br>
 * \code
 * char * res = NULL;
 * wcRCode aCode = wcClientGetStrValue(client, wcstSID, &res);
 * if (aCode == WC_OK) {
 *    cout << "client SID is " << res << endl;
 *    free(res);
 * } else {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 *
 * \sa wcStateId, wcRCode, wcStateVal, wcClientGetStrNValue
 */
wcRCode DLLEXPORT wcClientGetStrValue(wcHandle client, wcStateId aStateId, char ** aStateVal);
//! \brief Get a C-style string value for the selected client state.
/*! As a result of successful execution of the function, the requested value will be copied to the passed array \a aStateVal.
 * If the size of the passed array is less than the requested state value, the function returns an \ref WC_NOT_ENOUGH_MEM error.
 * Acceptable values of the state param are listed in \ref wcClientGetStrValue
 *
 * \param client         The client handle.
 * \param aStateId       The selected client state.
 * \param aStateVal      The pointer to allocated char array with at least sz+1 chars.
 * \param sz             The size of the given char array.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \b Example<br>
 * \code
 * static const uint32_t LEN = 128;
 * char res [LEN];
 * wcRCode aCode = wcClientGetStrNValue(client, wcstSID, LEN-1, res);
 * if (aCode == WC_OK) {
 *    cout << "client SID is " << res << endl;
 * } else {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 *
 * \sa wcStateId, wcRCode, wcStateVal, wcClientGetStrValue
 */
wcRCode DLLEXPORT wcClientGetStrNValue(wcHandle client, wcStateId aStateId, uint32_t sz, char * aStateVal);
//! \brief Set a C-style string value to the selected client state.
/*! Acceptable values of the state param are:<br>
 * \ref wcstMetaData - set the metadata for client (the *meta* parameter in \ref authorize request),<br>
 * \ref wcstDeviceName - set the client device name,  <br>
 * \ref wcstHostName - set the client host name, <br>
 * \ref wcstMsgsStamp - set the timestamp value for messages (the *stamp* parameter in \ref getMsgs request),<br>
 * \ref wcstRecordsStamp - set the timestamp value for records (the *stamp* parameter in \ref getRecordCount request),<br>
 * \ref wcstProxy - set the client proxy server authorization@address in format \code [[username[:password]@][protocol://]address[:port]] \endcode ,<br>
 * \ref wcstProxyProtocol - set the client proxy server protocol ("http://", "https://", etc), <br>
 * \ref wcstProxyHost - set the client proxy server host, <br>
 * \ref wcstProxyPort - set the client proxy server port, <br>
 * \ref wcstProxyUser - set the client proxy server user name, <br>
 * \ref wcstProxyPwrd - set the client proxy server user password. <br>
 *
 * \param client         The client handle.
 * \param aStateId       The selected client state.
 * \param aStateVal      The C-style string value to be setted.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \b Example<br>
 * \code
 * wcRCode aCode = wcClientSetStrValue(client, wcstProxy, "user:pw%26rd@192.168.0.1:3118");
 * if (aCode == WC_OK) {
 *    cout << "Proxy was successfully setted" << endl;
 * } else {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 *
 * \sa wcStateId, wcRCode, wcStateVal, wcClientGetStrValue, wcClientGetStrNValue
 */
wcRCode DLLEXPORT wcClientSetStrValue(wcHandle client, wcStateId aStateId, const char * aStateVal);
/*!@}*/

/*!
 * \addtogroup wcClientCallbacks
 * @{
 */
//! \brief Set specified notify callback for \a client.
/*! Acceptable values of the callback id are:<br>
 * \ref wccbkInitCURL,  <br>
 * \ref wccbkDisconnect. <br>
 *
 * \param client         The client handle.
 * \param callbackId     The specified callback id.
 * \param func           Pointer to callback.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcCallback
 */
wcRCode DLLEXPORT wcSetNotifyCallback(wcHandle client, wcCallback callbackId, NotifyEventLibFunc func);
//! \brief Set specified task notify callback for \a client.
/*! Acceptable values of the callback id are:<br>
 * \ref wccbkSuccessAuth,  <br>
 * \ref wccbkSynchroUpdateTask, <br>
 * \ref wccbkAfterLaunchInStream, <br>
 * \ref wccbkAfterLaunchOutStream, <br>
 * \ref wccbkSuccessIOStream, <br>
 * \ref wccbkSuccessSaveRecord. <br>
 *
 * \param client         The client handle.
 * \param callbackId     The specified callback id.
 * \param func           Pointer to callback.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcCallback, wcClientTasksProceed
 */
wcRCode DLLEXPORT wcSetTaskCallback(wcHandle client, wcCallback callbackId, TaskNotifyLibFunc func);
//! \brief Set specified connection notify callback for \a client.
/*!
 * \param client         The client handle.
 * \param func           Pointer to callback.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcCallback, wccbkConnected
 */
wcRCode DLLEXPORT wcSetConnCallback(wcHandle client, ConnNotifyEventLibFunc func);
//! \brief Set specified notify callback for \a client to handling the C-style string values.
/*! Acceptable values of the callback id are:<br>
 * \ref wccbkAddLog,  <br>
 * \ref wccbkSIDSetted. <br>
 *
 * \param client         The client handle.
 * \param callbackId     The specified callback id.
 * \param func           Pointer to callback.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcCallback
 */
wcRCode DLLEXPORT wcSetCStringCallback(wcHandle client, wcCallback callbackId, CStringNotifyLibFunc func);
#ifndef COMMON_FUNC_ONLY
//! \brief Set specified notify callback for \a client to handling the std::string values.
/*! \copydetails wcSetCStringCallback
 */
wcRCode DLLEXPORT wcSetStringCallback(wcHandle client, wcCallback callbackId, StringNotifyLibFunc func);
#endif
//! \brief Set specified notify callback for \a client to handling the sized data values.
/*! Acceptable value of the callback id is:<br>
 * \ref wccbkSuccessRequestRecord.  <br>
 *
 * \param client         The client handle.
 * \param callbackId     The specified callback id.
 * \param func           Pointer to callback.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcCallback, wcClientTasksProceed
 */
wcRCode DLLEXPORT wcSetAltDataCallback(wcHandle client, wcCallback callbackId, DataAltNotifyEventLibFunc func);
#ifndef COMMON_FUNC_ONLY
//! \brief Set specified notify callback for \a client to handling the wcCustomStream object.
/*! \copydetails wcSetAltDataCallback
 */
wcRCode DLLEXPORT wcSetDataCallback(wcHandle client, wcCallback callbackId, DataNotifyEventLibFunc func);
#endif
//! \brief Set specified notify callback for \a client to process the JSON-formatted response results represented as a C-style string.
/*! Acceptable values of the callback id are:<br>
 * \ref wccbkSuccessUpdateRecords,  <br>
 * \ref wccbkSuccessUpdateDevices, <br>
 * \ref wccbkSuccessUpdateStreams, <br>
 * \ref wccbkSuccessUpdateMsgs, <br>
 * \ref wccbkSuccessRequestRecordMeta, <br>
 * \ref wccbkSuccessGetConfig, <br>
 * \ref wccbkSuccessDeleteRecords. <br>
 *
 * \param client         The client handle.
 * \param callbackId     The specified callback id.
 * \param func           Pointer to callback.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcCallback, wcClientTasksProceed
 */
wcRCode DLLEXPORT wcSetJSONStrCallback(wcHandle client, wcCallback callbackId, JSONStrNotifyEventLibFunc func);
#ifndef COMMON_FUNC_ONLY
//! \brief Set specified notify callback for \a client to process the JSON-formatted response results represented as a cJSON object.
/*! \copydetails wcSetJSONStrCallback
 */
wcRCode DLLEXPORT wcSetcJSONCallback(wcHandle client,   wcCallback callbackId, cJSONNotifyEventLibFunc func);
//! \brief Set specified notify callback for \a client to process the JSON-formatted response results represented as a wcJSON object.
/*! \copydetails wcSetJSONStrCallback
 */
wcRCode DLLEXPORT wcSetJSONCallback(wcHandle client,    wcCallback callbackId, JSONNotifyEventLibFunc func);
#endif
/*!@}*/

/*!
 * \defgroup wcClientRequests Library functions to send common requests by client
 * @{
 */
//! \brief Get configuration from the server for authorized \a client.
/*! See protocol request \ref getConfig.
 *
 * \param client         The client handle.
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData ).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcSetJSONStrCallback, wccbkSuccessGetConfig, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcGetConfig(wcHandle client, void * data);
//! \brief Send configuration of authorized \a client to the server.
/*! See protocol request \ref setConfig.
 *
 * \param client         The client handle.
 * \param cfg            The C-style JSON-formatted string with new configuration.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode
 */
wcRCode DLLEXPORT wcSetConfig(wcHandle client, const char * cfg);
//! \brief Delete specified media records for authorized \a client.
/*! See protocol request \ref deleteRecords.
 *
 * \param client         The client handle.
 * \param indices        Specified media records as C-style JSON-formatted string.
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData ).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcSetJSONStrCallback, wccbkSuccessDeleteRecords, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcDeleteRecords(wcHandle client, const char * indices, void * data);
//! \brief Send message from authorized \a client.
/*! See protocol request \ref addMsgs.
 *
 * \param client         The client handle.
 * \param msg            Specified parameterized message as cJSON object or array of such messages ( \ref addMsgs ).
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData ).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcSendMsg(wcHandle client, const char * msg, void * data);
#ifndef COMMON_FUNC_ONLY
//! \brief Send message (cJSON) from authorized \a client.
/*! See protocol request \ref addMsgs.
 *
 * \param client         The client handle.
 * \param msg            Specified parameterized message as cJSON object or array of such messages ( \ref addMsgs ).
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData ).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcSendMsg_cJson(wcHandle client, cJSON * msg, void * data);
#endif
//! \brief Request specified media record from server for authorized \a client.
/*! See protocol request \ref getRecordData.
 *
 * \param client         The client handle.
 * \param rid            Specified media record id ( \ref getRecordData ).
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcSetDataCallback, wcSetAltDataCallback, wccbkSuccessRequestRecord, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcRequestRecord(wcHandle client, int rid, void * data);
//! \brief Request metadata of specified media record from server for authorized \a client.
/*! See protocol request \ref getRecordMeta.
 *
 * \param client         The client handle.
 * \param rid            Specified media record id ( \ref getRecordMeta ).
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wccbkSuccessRequestRecordMeta, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcRequestRecordMeta(wcHandle client, int rid, void * data);
//! \brief Send a media record to server from authorized \a client.
/*! See protocol request \ref addRecord.
 *
 * \param client         The client handle.
 * \param aBuf           Pointer to record data  ( \ref  addRecord ).
 * \param sz             The record size in bytes.
 * \param meta           Metadata for the media record  ( \ref  addRecord ).
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData ).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcSetTaskCallback, wccbkSuccessSaveRecord, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcSaveRecord(wcHandle client, const void * aBuf, size_t  sz, const char * meta, void * data);
//! \brief Launch output stream for authorized \a client.
/*! See protocol request \ref input.
 *
 * \param client         The client handle.
 * \param subProtocol    The sub protocol description (C-style string, not NULL).
 * \param delta          The delta time between frames in ms.
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData ).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcSetTaskCallback, wccbkAfterLaunchOutStream, wccbkSynchroUpdateTask, wccbkSuccessIOStream, wcClientFramePushData, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcLaunchOutStream(wcHandle client, const char * subProtocol, int delta, void * data);
//! \brief Launch task to consume incoming stream from the specified device for authorized \a client.
/*! See protocol request \ref output.
 *
 * \param client         The client handle.
 * \param deviceName     C-style string containing the specified device name.
 * \param data           Additional user data that passed to the new task ( see also \ref wcTaskGetUserData ).
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcSetTaskCallback, wccbkAfterLaunchInStream, wccbkSynchroUpdateTask, wccbkSuccessIOStream, wcInTaskPopFrame, wcTaskGetUserData
 */
wcRCode DLLEXPORT wcLaunchInStream(wcHandle client, const char * deviceName, void * data);
/*!@}*/

/*!
 * \defgroup wcFramesFuncs Library functions to manage with output stream frames
 * @{
 */
//! \brief Get the ID of the current output frame.
/*! Related to request \ref input.
 *
 * \param client         The client handle.
 * \param id             Pointer to the long type variable.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode
 */
wcRCode DLLEXPORT wcClientGetFrameID(wcHandle client, long * id);
//! \brief Lock the frame object for threadsafe access to the output data stack.
/*! Related to request \ref input.
 *
 * \param client         The client handle.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcClientFrameUnLock
 */
wcRCode DLLEXPORT wcClientFrameLock(wcHandle client);
//! \brief Unlock the frame object.
/*! Related to request \ref input.
 *
 * \param client         The client handle.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcClientFrameLock
 */
wcRCode DLLEXPORT wcClientFrameUnLock(wcHandle client);
//! \brief Add a new frame to the outgoing data stack to send.
/*! Related to request \ref input. The sent frames counter is automatically incremented by one.
 * It is strongly recommended to lock the frame object using the \ref wcClientFrameLock / \ref wcClientFrameUnLock
 * procedures when accessing the outgoing data stack. To add a new frame after sending the previous one,
 * use the \ref wccbkSynchroUpdateTask callback.
 *
 * \param client         The client handle.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wccbkSynchroUpdateTask, wcSetTaskCallback, wcClientTasksProceed
 */
wcRCode DLLEXPORT wcClientFramePushData(wcHandle client, const void * data, size_t len);
//! \brief Get access to the last frame in the outgoing data stack.
/*! Related to request \ref input.
 * It is strongly recommended to lock the frame object using the \ref wcClientFrameLock / \ref wcClientFrameUnLock
 * procedures when accessing the outgoing data stack.
 *
 * \param client         The client handle.
 * \param data           The pointer to the variable of void * type initialized with NULL value to return frame data.
 * \param len            The pointer to the variable of size_t type to return frame size.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \b Example<br>
 * \code
 * void * data = NULL;
 * size_t len = 0;
 * wcRCode aCode = wcClientFrameLock(client);
 * if (aCode == WC_OK) {
 *    aCode = wcClientFrameGetData(client, &data, &len);
 *    if (aCode == WC_OK) {
 *       // work with data and len
 *       // do not free this data! it will be deleted automatically by library
 *    } else {
 *       cout << "error occurred " << aCode << endl;
 *    }
 *    aCode = wcClientFrameUnLock(client);
 * }
 * if (aCode != WC_OK)
 * {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 * \sa wcRCode
 */
wcRCode DLLEXPORT wcClientFrameGetData(wcHandle client, void ** data, size_t * len);
/*!@}*/

/*!
 * \defgroup wcTasksFuncs Library functions to manage with background tasks
 * @{
 */
//! \brief Get the class of the \a task.
/*! Possible results for the \a id parameter:<br>
 *  \ref WC_TASK,<br>
 *  \ref WC_IN_STREAM_TASK,<br>
 *  \ref WC_OUT_STREAM_TASK.<br>
 *
 * \param task          Pointer to the task object.
 * \param id            Pointer to the variable of \ref wcTaskClass type to return the task class.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode
 */
wcRCode DLLEXPORT wcTaskGetClass(wcTask task, wcTaskClass * id);
//! \brief Get the user data for the \a task.
/*!
 * \param task          Pointer to the task object.
 * \param data          Pointer to the variable of \ref void * type to return the user data.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcTaskSetUserData, wcClientRequests
 */
wcRCode DLLEXPORT wcTaskGetUserData(wcTask task, void ** data);
//! \brief Set the user data for the \a task.
/*! Data is not deleted by library.
 *
 * \param task          Pointer to the task object.
 * \param data          The user data.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcTaskGetUserData, wcClientRequests
 */
wcRCode DLLEXPORT wcTaskSetUserData(wcTask task, void * data);
//! \brief Lock the task object for threadsafe access.
/*!
 * \param task           Pointer to the task object.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcTaskUnLock
 */
wcRCode DLLEXPORT wcTaskLock(wcTask task);
//! \brief Unlock the task object.
/*!
 * \param task           Pointer to the task object.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \sa wcRCode, wcTaskLock
 */
wcRCode DLLEXPORT wcTaskUnLock(wcTask task);
//! \brief Get a C-style string value for the selected task state.
/*! As a result of successful execution of the function, the \a aStateVal parameter will contain a
 * pointer to NULL-terminated string. (Don't forget to free up memory after using the \a aStateVal).
 * Acceptable values of the state param are:<br>
 * \ref wctstError - get the last error for the task - acceptable for \ref WC_ALL_TASKS, <br>
 * \ref wctstPath - get the target path for the task - acceptable for \ref WC_ALL_TASKS, <br>
 * \ref wctstSubProto - get the specified protocol for the output streaming task - acceptable for \ref WC_OUT_STREAM_TASK - \ref wcLaunchOutStream, <br>
 * \ref wctstDeviceName - get the specified device name for the input streaming task - acceptable for \ref WC_IN_STREAM_TASK - \ref wcLaunchInStream.<br>
 *
 * \param task           Pointer to the task object.
 * \param aStateId       The selected task state.
 * \param aStateVal      The pointer to the variable of char * type initialized with NULL value
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \b Example<br>
 * \code
 * char * res = NULL;
 * wcRCode aCode = wcTaskGetStrValue(client, wctstPath, &res);
 * if (aCode == WC_OK) {
 *    cout << "task path is " << res << endl;
 *    free(res);
 * } else {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 *
 * \sa wcTaskStateId, wcRCode, wcStateVal, wcTaskGetStrNValue
 */
wcRCode DLLEXPORT wcTaskGetStrValue(wcTask task, wcTaskStateId aStateId, char ** aStateVal);
//! \brief Get a C-style string value for the selected task state.
/*! As a result of successful execution of the function, the requested value will be copied to the passed array \a aStateVal.
 * If the size of the passed array is less than the requested state value, the function returns an \ref WC_NOT_ENOUGH_MEM error.
 * Acceptable values of the state param are listed in \ref wcTaskGetStrValue
 *
 * \param task           Pointer to the task object.
 * \param aStateId       The selected task state.
 * \param aStateVal      The pointer to allocated char array with at least sz+1 chars.
 * \param sz             The size of the given char array.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \b Example<br>
 * \code
 * static const uint32_t LEN = 128;
 * char res [LEN];
 * wcRCode aCode = wcTaskGetStrNValue(client, wctstPath, LEN-1, res);
 * if (aCode == WC_OK) {
 *    cout << "task path is " << res << endl;
 * } else {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 *
 * \sa wcTaskStateId, wcRCode, wcStateVal, wcTaskGetStrValue
 */
wcRCode DLLEXPORT wcTaskGetStrNValue(wcTask task, wcTaskStateId aStateId, uint32_t sz, char * aStateVal);
/*!@}*/

/*!
 * \defgroup wcInTasksFuncs Library functions to manage with incoming streams
 * @{
 */
//! \brief Get access to the first frame in the incoming data stack.
/*! Related to request \ref output. A signal that a new frame has been received is
 * triggered by a \a task with the \ref wccbkSynchroUpdateTask callback.
 *
 * \param task           Pointer to the task object of \ref WC_IN_STREAM_TASK class.
 * \param data           The pointer to the variable of void * type to return frame data.
 * \param len            The pointer to the variable of size_t type to return frame size.
 * \return \ref wcRCode  \ref WC_OK or error code.
 *
 * \b Example 1<br>
 * \code
 * void * data = NULL;
 * size_t len = 0;
 *
 * aCode = wcInTaskPopFrame(client, &data, &len);
 * if (aCode == WC_OK) {
 *    if (data && (len > 0)) {
 *       // work with incoming data and len (draw, save, etc.)
 *       free(data);
 *    }
 * } else {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 * \b Example 2<br>
 * \code
 * // The data can be initialized in advance.
 * // Then the len variable should contain the size of the
 * // allocated memory for the data variable.
 * static const size_t BUFFER_SIZE = 0xffff;
 * size_t len = BUFFER_SIZE;
 * void * data = malloc(len);
 *
 * aCode = wcInTaskPopFrame(client, &data, &len);
 * if (aCode == WC_OK) {
 *    // now the len variable contains the actual length of the data
 *    if (data && (len > 0)) {
 *       // work with incoming data and len (draw, save, etc.)
 *    }
 * } else {
 *    cout << "error occurred " << aCode << endl;
 * }
 * \endcode
 * \sa wcRCode, wccbkSynchroUpdateTask, wcSetTaskCallback, wcClientTasksProceed
 */
wcRCode DLLEXPORT wcInTaskPopFrame(wcTask task, void ** data, size_t * len);
/*!@}*/


#ifdef __cplusplus
}
#endif

/** \include commline_tools.h
    \example lib_test/main.cpp
 * This is an example of how to use the wcWebCamClient library.
 * In this example, a client is created, authorized on the server, and downloads a list of active devices.
 */

/** \include commline_tools.h
    \example theora_test/output_strm/main.cpp
 * This is an example of how to use the wcWebCamClient library.
 * A simple test program to demonstrate the streaming of output data.
 */

/** \include commline_tools.h
    \example theora_test/input_strm/main.cpp
 * This is an example of how to use the wcWebCamClient library.
 * A simple test program to demonstrate input streaming.
 */

#endif // WCWEBCAMCLIENT_H
