#include "wcwebcamclient.h"
#include "wcCURLClient.h"
#include "wcJSON.h"

class wcInternalClient
{
public:
    wcCURLClient* client;
    wcHandle handle;
    std::vector<void*> callbacks;

	wcInternalClient(wcHandle aHandle) : callbacks(wccbkLast) {
	    client = new wcCURLClient();
	    handle = aHandle;
	};
	void setCallback(wcCallback cbk, void* val) { callbacks.at((int)cbk) = val; }
	void* getCallback(wcCallback cbk) { return callbacks.at((int)cbk); }
	~wcInternalClient() {
	    delete client;
	}
};

#ifdef __cplusplus
extern "C" {
#endif

static std::vector<wcInternalClient*> wcClients;

wcHandle DLLEXPORT wcClientCreate() {
    wcHandle res = -1;
    wcInternalClient* e = new wcInternalClient(res);
    for (size_t i = 0; i < wcClients.size(); i++)
    {
        if (!ASSIGNED(wcClients[i])) res = (wcHandle)i;
    }
    if (res < 0)
    {
        wcClients.push_back(e);
        res = (wcHandle)wcClients.size() - 1;
    }
    else
    {
        wcClients[res] = e;
    }
    e->handle = res;
    return res;
}
inline int CheckClient(wcHandle & handle) {
	if (handle < (int)wcClients.size() && handle >= 0) {
		if ASSIGNED(wcClients[handle]) {
                return WC_OK;
		};
	};
	return WC_BAD_TARGET;
}

#define WC_CLIENT(arg) wcClients[arg]->client

inline int CheckTask(wcTask task, uint32_t allowed) {
    if ASSIGNED(task) {
        try {
            wcHTTP2AbsBackgroundTask* locTask0 = static_cast<wcHTTP2AbsBackgroundTask*>(task);
            wcHTTP2BackgroundTask* locTask = dynamic_cast<wcHTTP2BackgroundTask*>(locTask0);
            if (locTask != NULL) {
                if ((allowed & locTask->getTaskClass()) != 0) {
                    return WC_OK;
                } else return WC_TASK_MISMATCH;
            } else return WC_NOT_A_TASK;
        } catch (...) {
            return WC_NOT_A_TASK;
        }
    } else return WC_BAD_TARGET;
}

wcRCode DLLEXPORT wcClientStart(wcHandle client) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    WC_CLIENT(client)->start();

    return v;
}

wcRCode DLLEXPORT wcClientProceed(wcHandle client) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    WC_CLIENT(client)->proceed();

    return v;
}

wcRCode DLLEXPORT wcClientTasksProceed(wcHandle client) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    WC_CLIENT(client)->tasksProceed();

    return v;
}

wcRCode DLLEXPORT wcClientAuth(wcHandle client, const char * aLogin, const char * aPwrd) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    std::string strLogin (aLogin);
    std::string strPwrd (aPwrd);
    WC_CLIENT(client)->authorize(strLogin, strPwrd);

    return v;
}

wcRCode DLLEXPORT wcClientInvalidateState(wcHandle client, wcStateId aStateId) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    switch (aStateId) {
    case wcstLog :
        WC_CLIENT(client)->clearLog();
        break;
    case wcstError :
        WC_CLIENT(client)->clearError();
        break;
    case wcstStreams :
        WC_CLIENT(client)->setNeedToUpdateStreams(true);
        break;
    case wcstDevices :
        WC_CLIENT(client)->setNeedToUpdateDevices(true);
        break;
    case wcstRecords :
        WC_CLIENT(client)->setNeedToUpdateRecords(true);
        break;
    case wcstMsgs :
        WC_CLIENT(client)->setNeedToUpdateMsgs(true);
        break;
    case wcstMsgsStamp:
        WC_CLIENT(client)->clearLastMsgsStamp();
        break;
    case wcstRecordsStamp:
        WC_CLIENT(client)->clearLastRecsStamp();
        break;
    case wcstMetaData :
        WC_CLIENT(client)->setMetaData("");
        break;
    case wcstDeviceName:
        WC_CLIENT(client)->setDevice("");
        break;
    case wcstHostName:
        WC_CLIENT(client)->setHost("");
        break;
    case wcstProxy:
        WC_CLIENT(client)->setProxy("");
        break;
    default :
        v = WC_BAD_PARAM;
        break;
    }

    return v;
}

wcRCode DLLEXPORT wcClientGetIntState(wcHandle client, wcStateId aStateId, int * aStateVal) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    if (!ASSIGNED(aStateVal)) return WC_BAD_VALUE;

    switch (aStateId) {
    case wcstError :
        *aStateVal = WC_CLIENT(client)->getLastError();
        break;
    case wcstLog :
        *aStateVal = WC_CLIENT(client)->getLogLength();
        break;
    default :
        v = WC_BAD_PARAM;
        break;
    }

    return v;
}

wcRCode DLLEXPORT wcClientGetBoolState(wcHandle client, wcStateId aStateId) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    switch (aStateId) {
    case wcstConnection :
        if (WC_CLIENT(client)->getConnected()) v = WC_TRUE;
        break;
    case wcstVerifyTLS :
        if (WC_CLIENT(client)->getVerifyTLS()) v = WC_TRUE;
        break;
    case wcstError :
        if (WC_CLIENT(client)->hasError()) v = WC_TRUE;
        break;
    case wcstStreaming :
        if (WC_CLIENT(client)->getStreaming()) v = WC_TRUE;
        break;
    case wcstLog :
        if (WC_CLIENT(client)->getLogLength() > 0) v = WC_TRUE;
        break;
    case wcstStreams :
        if (WC_CLIENT(client)->getNeedToUpdateStreams()) v = WC_TRUE;
        break;
    case wcstDevices :
        if (WC_CLIENT(client)->getNeedToUpdateDevices()) v = WC_TRUE;
        break;
    case wcstRecords :
        if (WC_CLIENT(client)->getNeedToUpdateRecords()) v = WC_TRUE;
        break;
    case wcstMsgs :
        if (WC_CLIENT(client)->getNeedToUpdateMsgs()) v = WC_TRUE;
        break;
    default :
        v = WC_BAD_PARAM;
        break;
    }

    return v;
}

wcRCode DLLEXPORT wcClientSetBoolState(wcHandle client, wcStateId aStateId, wcStateVal aStateVal) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    switch (aStateId) {
    case wcstConnection :
        if (aStateVal == WC_FALSE)
            WC_CLIENT(client)->disconnect();
        else
            v = WC_BAD_VALUE;
        break;
    case wcstVerifyTLS :
        WC_CLIENT(client)->setVerifyTLS(aStateVal == WC_TRUE);
        break;
    case wcstStreaming :
        if (aStateVal == WC_FALSE)
            WC_CLIENT(client)->stopStreaming();
        else
            v = WC_BAD_VALUE;
        break;
    case wcstStreams :
        if (aStateVal == WC_TRUE)
            WC_CLIENT(client)->setNeedToUpdateStreams(true);
        else
            v = WC_BAD_VALUE;
        break;
    case wcstDevices :
        if (aStateVal == WC_TRUE)
            WC_CLIENT(client)->setNeedToUpdateDevices(true);
        else
            v = WC_BAD_VALUE;
        break;
    case wcstRecords :
        if (aStateVal == WC_TRUE)
            WC_CLIENT(client)->setNeedToUpdateRecords(true);
        else
            v = WC_BAD_VALUE;
        break;
    case wcstMsgs :
        if (aStateVal == WC_TRUE)
            WC_CLIENT(client)->setNeedToUpdateMsgs(true);
        else
            v = WC_BAD_VALUE;
        break;
    default :
        v = WC_BAD_PARAM;
        break;
    }

    return v;
}

wcRCode DLLEXPORT wcClientSetStrValue(wcHandle client, wcStateId aStateId, const char * aStateVal) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    if (!ASSIGNED(aStateVal)) return WC_BAD_VALUE;

    switch (aStateId) {
    case wcstMetaData :
        WC_CLIENT(client)->setMetaData(aStateVal);
        break;
    case wcstDeviceName :
        WC_CLIENT(client)->setDevice(aStateVal);
        break;
    case wcstHostName :
        WC_CLIENT(client)->setHost(aStateVal);
        break;
    case wcstMsgsStamp :
        WC_CLIENT(client)->setLastMsgsStamp(aStateVal);
        break;
    case wcstRecordsStamp :
        WC_CLIENT(client)->setLastRecsStamp(aStateVal);
        break;
    case wcstProxy :
        WC_CLIENT(client)->setProxy(aStateVal);
        break;
    case wcstProxyProtocol :
        WC_CLIENT(client)->setProxyProtocol(aStateVal);
        break;
    case wcstProxyHost :
        WC_CLIENT(client)->setProxyHost(aStateVal);
        break;
    case wcstProxyPort :
        WC_CLIENT(client)->setProxyPort(aStateVal);
        break;
    case wcstProxyUser :
        WC_CLIENT(client)->setProxyUser(aStateVal);
        break;
    case wcstProxyPwrd :
        WC_CLIENT(client)->setProxyPwrd(aStateVal);
        break;
    default :
        v = WC_BAD_PARAM;
        break;
    }

    return v;
}

wcRCode internalGetStrValue(wcHandle client, wcStateId aStateId, std::string & value) {
    switch (aStateId) {
    case wcstMsgsStamp:
        value = WC_CLIENT(client)->getLastMsgsStamp();
        break;
    case wcstRecordsStamp:
        value = WC_CLIENT(client)->getLastRecsStamp();
        break;
    case wcstSID :
        value = WC_CLIENT(client)->getSID();
        break;
    case wcstLog :
        value = WC_CLIENT(client)->popLog();
        break;
    case wcstMetaData :
        value = WC_CLIENT(client)->getMetaData();
        break;
    case wcstDeviceName :
        value = WC_CLIENT(client)->getDevice();
        break;
    case wcstHostName :
        value = WC_CLIENT(client)->getHost();
        break;
    case wcstProxy :
        value = WC_CLIENT(client)->getProxy();
        break;
    case wcstProxyAuth :
        value = WC_CLIENT(client)->getProxyAuth();
        break;
    default :
        return WC_BAD_PARAM;
    }
    return WC_OK;
}

wcRCode DLLEXPORT wcClientGetStrValue(wcHandle client, wcStateId aStateId, char ** aStateVal) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    if (!ASSIGNED(aStateVal)) return WC_BAD_VALUE;
    if (ASSIGNED(*aStateVal)) return WC_BAD_VALUE;

    std::string value;
    v = internalGetStrValue(client, aStateId, value);
    if (v != WC_OK) return v;

    *aStateVal = (char*)malloc(value.size()+1);
    memcpy(*aStateVal, value.c_str(), value.size());
    (*aStateVal)[value.size()] = 0;

    return v;
}

wcRCode DLLEXPORT wcClientGetStrNValue(wcHandle client, wcStateId aStateId, uint32_t sz, char * aStateVal)
{
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    if (!ASSIGNED(aStateVal)) return WC_BAD_VALUE;

    std::string value;
    v = internalGetStrValue(client, aStateId, value);
    if (v != WC_OK) return v;

    if (sz <= value.size()) return WC_BAD_VALUE;
    memcpy(aStateVal, value.c_str(), value.size());
    aStateVal[value.size()] = 0;

    return v;
}

wcRCode DLLEXPORT wcClientDisconnect(wcHandle client) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    WC_CLIENT(client)->disconnect();

    return v;
}

wcRCode DLLEXPORT wcClientDestroy(wcHandle client) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    delete wcClients[client];
    wcClients[client] =  NULL;

    return v;
}

inline wcInternalClient * findClient(wcCallbackClient client) {
    for (size_t i = 0; i < wcClients.size(); i++)
    {
        if (ASSIGNED(wcClients[i])) {
            if (wcClients[i]->client == client)
                return wcClients[i];
        }
    }
    return NULL;
}

inline void setIntClientCallback(wcHandle client, wcCallback cb, void * func) {
    wcClients[client]->setCallback(cb, func);
}


#ifndef COMMON_FUNC_ONLY

void lib_callcJSONNotifyEvent(wcCallback cbk, wcCallbackClient self, wcCallbackTask tsk, const cJSON * v) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((cJSONNotifyEventLibFunc)(int_client->getCallback(cbk)))(int_client->handle, tsk, v);
}

void lib_callJSONNotifyEvent(wcCallback cbk, wcCallbackClient self, wcCallbackTask tsk, wcJSON & v) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((JSONNotifyEventLibFunc)(int_client->getCallback(cbk)))(int_client->handle, tsk, v);
}

void lib_callStringNotify(wcCallback cbk, wcCallbackClient self, const std::string& v) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((StringNotifyLibFunc)(int_client->getCallback(cbk)))(int_client->handle, v);
}

#endif

void lib_callJSONStrNotifyEvent(wcCallback cbk, wcCallbackClient self, wcCallbackTask tsk, const char * v) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((JSONStrNotifyEventLibFunc)(int_client->getCallback(cbk)))(int_client->handle, tsk, v);
}

void lib_callDataAltNotifyEvent(wcCallback cbk, wcCallbackClient self, wcCallbackTask tsk, void * v, size_t sz) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((DataAltNotifyEventLibFunc)(int_client->getCallback(cbk)))(int_client->handle, tsk, v, sz);
}

void lib_callDataNotifyEvent(wcCallback cbk, wcCallbackClient self, wcCallbackTask tsk, void * v) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((DataNotifyEventLibFunc)(int_client->getCallback(cbk)))(int_client->handle, tsk, v);
}

void lib_callNotifyEvent(wcCallback cbk, wcCallbackClient self, void * v) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((NotifyEventLibFunc)(int_client->getCallback(cbk)))(int_client->handle, v);
}

void lib_callTaskNotifyEvent(wcCallback cbk, wcCallbackClient self, wcCallbackTask tsk) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((TaskNotifyLibFunc)(int_client->getCallback(cbk)))(int_client->handle, tsk);
}

void lib_callConnNotifyEvent(wcCallback cbk, wcCallbackClient self, bool v) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((ConnNotifyEventLibFunc)(int_client->getCallback(cbk)))(int_client->handle, v);
}

void lib_callCStringNotify(wcCallback cbk, wcCallbackClient self, const char * v) {
    wcInternalClient * int_client = findClient(self);
    if ASSIGNED(int_client)
        ((CStringNotifyLibFunc)(int_client->getCallback(cbk)))(int_client->handle, v);
}

void lib_wccbkInitCURL(wcCallbackClient self, void* v) {
    lib_callNotifyEvent(wccbkInitCURL, self, v);
}

void lib_wccbkDisconnect(wcCallbackClient self, void* v) {
    lib_callNotifyEvent(wccbkDisconnect, self, v);
}

void lib_wccbkSuccessAuth(wcCallbackClient self, wcCallbackTask v) {
    lib_callTaskNotifyEvent(wccbkSuccessAuth, self, v);
}

void lib_wccbkSynchroUpdateTask(wcCallbackClient self, wcCallbackTask tsk) {
    lib_callTaskNotifyEvent(wccbkSynchroUpdateTask, self, tsk);
}

void lib_wccbkAfterLaunchInStream(wcCallbackClient self, wcCallbackTask tsk) {
    lib_callTaskNotifyEvent(wccbkAfterLaunchInStream, self, tsk);
}

void lib_wccbkAfterLaunchOutStream(wcCallbackClient self, wcCallbackTask tsk) {
    lib_callTaskNotifyEvent(wccbkAfterLaunchOutStream, self, tsk);
}

void lib_wccbkSuccessIOStream(wcCallbackClient self, wcCallbackTask tsk) {
    lib_callTaskNotifyEvent(wccbkSuccessIOStream, self, tsk);
}

void lib_wccbkSuccessSaveRecord(wcCallbackClient self, wcCallbackTask tsk) {
    lib_callTaskNotifyEvent(wccbkSuccessSaveRecord, self, tsk);
}

void lib_wccbkConnected(wcCallbackClient self, bool v) {
    lib_callConnNotifyEvent(wccbkConnected, self, v);
}

void lib_cStr_wccbkSuccessUpdateRecords(wcCallbackClient self, wcCallbackTask tsk, const char * v) {
    lib_callJSONStrNotifyEvent(wccbkSuccessUpdateRecords, self, tsk, v);
}

void lib_cStr_wccbkSuccessUpdateMsgs(wcCallbackClient self, wcCallbackTask tsk, const char * v) {
    lib_callJSONStrNotifyEvent(wccbkSuccessUpdateMsgs, self, tsk, v);
}

void lib_cStr_wccbkSuccessUpdateDevices(wcCallbackClient self, wcCallbackTask tsk, const char * v) {
    lib_callJSONStrNotifyEvent(wccbkSuccessUpdateDevices, self, tsk, v);
}

void lib_cStr_wccbkSuccessUpdateStreams(wcCallbackClient self, wcCallbackTask tsk, const char * v) {
    lib_callJSONStrNotifyEvent(wccbkSuccessUpdateStreams, self, tsk, v);
}

void lib_cStr_wccbkSuccessGetConfig(wcCallbackClient self, wcCallbackTask tsk, const char * v) {
    lib_callJSONStrNotifyEvent(wccbkSuccessGetConfig, self, tsk, v);
}

void lib_cStr_wccbkSuccessRequestRecordMeta(wcCallbackClient self, wcCallbackTask tsk, const char * v) {
    lib_callJSONStrNotifyEvent(wccbkSuccessRequestRecordMeta, self, tsk, v);
}

void lib_cStr_wccbkSuccessDeleteRecords(wcCallbackClient self, wcCallbackTask tsk, const char * v) {
    lib_callJSONStrNotifyEvent(wccbkSuccessDeleteRecords, self, tsk, v);
}

void lib_cStr_wccbkAddLog(wcCallbackClient self, const char * v) {
    lib_callCStringNotify(wccbkAddLog, self, v);
}

void lib_cStr_wccbkSIDSetted(wcCallbackClient self, const char * v) {
    lib_callCStringNotify(wccbkSIDSetted, self, v);
}

void lib_wccbkSuccessRequestRecord(wcCallbackClient self, wcCallbackTask tsk, void * strm) {
    lib_callDataNotifyEvent(wccbkSuccessRequestRecord, self, tsk, strm);
}

void lib_Alt_wccbkSuccessRequestRecord(wcCallbackClient self, wcCallbackTask tsk, void * data, size_t sz) {
    lib_callDataAltNotifyEvent(wccbkSuccessRequestRecord, self, tsk, data, sz);
}

#define SET_INT_CALLBACK(handle, cb, meth, func) setIntClientCallback(handle, cb, (void*)func);WC_CLIENT(handle)->meth(lib_ ## cb);
#define SET_INT_CSTR_CALLBACK(handle, cb, meth, func) setIntClientCallback(handle, cb, (void*)func);WC_CLIENT(handle)->meth(lib_cStr_ ## cb);
#define SET_INT_ALT_CALLBACK(handle, cb, meth, func) setIntClientCallback(handle, cb, (void*)func);WC_CLIENT(handle)->meth(lib_Alt_ ## cb);

#ifndef COMMON_FUNC_ONLY
void lib_cJson_wccbkSuccessUpdateRecords(wcCallbackClient self, wcCallbackTask tsk, const cJSON* v) {
    lib_callcJSONNotifyEvent(wccbkSuccessUpdateRecords, self, tsk, v);
}

void lib_cJson_wccbkSuccessUpdateMsgs(wcCallbackClient self, wcCallbackTask tsk, const cJSON* v) {
    lib_callcJSONNotifyEvent(wccbkSuccessUpdateMsgs, self, tsk, v);
}

void lib_cJson_wccbkSuccessUpdateDevices(wcCallbackClient self, wcCallbackTask tsk, const cJSON* v) {
    lib_callcJSONNotifyEvent(wccbkSuccessUpdateDevices, self, tsk, v);
}

void lib_cJson_wccbkSuccessUpdateStreams(wcCallbackClient self, wcCallbackTask tsk, const cJSON* v) {
    lib_callcJSONNotifyEvent(wccbkSuccessUpdateStreams, self, tsk, v);
}

void lib_cJson_wccbkSuccessGetConfig(wcCallbackClient self, wcCallbackTask tsk, const cJSON* v) {
    lib_callcJSONNotifyEvent(wccbkSuccessGetConfig, self, tsk, v);
}

void lib_cJson_wccbkSuccessRequestRecordMeta(wcCallbackClient self, wcCallbackTask tsk, const cJSON* v) {
    lib_callcJSONNotifyEvent(wccbkSuccessRequestRecordMeta, self, tsk, v);
}

void lib_cJson_wccbkSuccessDeleteRecords(wcCallbackClient self, wcCallbackTask tsk, const cJSON* v) {
    lib_callcJSONNotifyEvent(wccbkSuccessDeleteRecords, self, tsk, v);
}

void lib_Json_wccbkSuccessUpdateRecords(wcCallbackClient self, wcCallbackTask tsk,  wcJSON& v) {
    lib_callJSONNotifyEvent(wccbkSuccessUpdateRecords, self, tsk, v);
}

void lib_Json_wccbkSuccessUpdateMsgs(wcCallbackClient self, wcCallbackTask tsk,  wcJSON& v) {
    lib_callJSONNotifyEvent(wccbkSuccessUpdateMsgs, self, tsk, v);
}

void lib_Json_wccbkSuccessUpdateDevices(wcCallbackClient self, wcCallbackTask tsk,  wcJSON& v) {
    lib_callJSONNotifyEvent(wccbkSuccessUpdateDevices, self, tsk, v);
}

void lib_Json_wccbkSuccessUpdateStreams(wcCallbackClient self, wcCallbackTask tsk,  wcJSON& v) {
    lib_callJSONNotifyEvent(wccbkSuccessUpdateStreams, self, tsk, v);
}

void lib_Json_wccbkSuccessGetConfig(wcCallbackClient self, wcCallbackTask tsk, wcJSON& v) {
    lib_callJSONNotifyEvent(wccbkSuccessGetConfig, self, tsk, v);
}

void lib_Json_wccbkSuccessRequestRecordMeta(wcCallbackClient self, wcCallbackTask tsk, wcJSON& v) {
    lib_callJSONNotifyEvent(wccbkSuccessRequestRecordMeta, self, tsk, v);
}

void lib_Json_wccbkSuccessDeleteRecords(wcCallbackClient self, wcCallbackTask tsk,  wcJSON& v) {
    lib_callJSONNotifyEvent(wccbkSuccessDeleteRecords, self, tsk, v);
}

void lib_Str_wccbkAddLog(wcCallbackClient self, const std::string& v) {
    lib_callStringNotify(wccbkAddLog, self, v);
}

void lib_Str_wccbkSIDSetted(wcCallbackClient self, const std::string& v) {
    lib_callStringNotify(wccbkSIDSetted, self, v);
}

#define SET_INT_STR_CALLBACK(handle, cb, meth, func) setIntClientCallback(handle, cb, (void*)func);WC_CLIENT(handle)->meth(lib_Str_ ## cb);
#define SET_INT_CJSON_CALLBACK(handle, cb, meth, func) setIntClientCallback(handle, cb, (void*)func);WC_CLIENT(handle)->meth(lib_cJson_ ## cb);
#define SET_INT_JSON_CALLBACK(handle, cb, meth, func) setIntClientCallback(handle, cb, (void*)func);WC_CLIENT(handle)->meth(lib_Json_ ## cb);
#endif


wcRCode DLLEXPORT wcSetNotifyCallback(wcHandle client, wcCallback callbackId, NotifyEventLibFunc func) {
    /*
        NotifyEvent mOnInitCURL {NULL_NotifyEvent};
        NotifyEvent mOnDisconnect {NULL_NotifyEvent};
    */
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkInitCURL:
        SET_INT_CALLBACK(client, wccbkInitCURL, setOnInitCURL, func);
        break;
    case wccbkDisconnect:
        SET_INT_CALLBACK(client, wccbkDisconnect, setOnDisconnect, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;
}

wcRCode DLLEXPORT wcSetTaskCallback(wcHandle client, wcCallback callbackId, TaskNotifyLibFunc func) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkSuccessAuth:
        SET_INT_CALLBACK(client, wccbkSuccessAuth, setOnSuccessAuth, func);
        break;
    case wccbkSynchroUpdateTask:
        SET_INT_CALLBACK(client, wccbkSynchroUpdateTask, setOnSynchroUpdateTask, func);
        break;
    case wccbkAfterLaunchInStream:
        SET_INT_CALLBACK(client, wccbkAfterLaunchInStream, setOnAfterLaunchInStream, func);
        break;
    case wccbkAfterLaunchOutStream:
        SET_INT_CALLBACK(client, wccbkAfterLaunchOutStream, setOnAfterLaunchOutStream, func);
        break;
    case wccbkSuccessIOStream:
        SET_INT_CALLBACK(client, wccbkSuccessIOStream, setOnSuccessIOStream, func);
        break;
    case wccbkSuccessSaveRecord:
        SET_INT_CALLBACK(client, wccbkSuccessSaveRecord, setOnSuccessSaveRecord, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;
}

wcRCode DLLEXPORT wcSetConnCallback(wcHandle client, ConnNotifyEventLibFunc func) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    SET_INT_CALLBACK(client, wccbkConnected, setOnConnectedChanged, func);
    return v;
}

#ifndef COMMON_FUNC_ONLY
wcRCode DLLEXPORT wcSetcJSONCallback(wcHandle client,   wcCallback callbackId, cJSONNotifyEventLibFunc func) {
    /*
        cJSONNotifyEvent mOnSuccessUpdateRecords {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessUpdateDevices {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessUpdateMsgs {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessSendMsg {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessGetConfig {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessDeleteRecords {NULL_cJSONNotifyEvent};
    */
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkSuccessUpdateRecords:
        SET_INT_CJSON_CALLBACK(client, wccbkSuccessUpdateRecords, setOnSuccessUpdateRecords, func);
        break;
    case wccbkSuccessUpdateDevices:
        SET_INT_CJSON_CALLBACK(client, wccbkSuccessUpdateDevices, setOnSuccessUpdateDevices, func);
        break;
    case wccbkSuccessUpdateStreams:
        SET_INT_CJSON_CALLBACK(client, wccbkSuccessUpdateStreams, setOnSuccessUpdateStreams, func);
        break;
    case wccbkSuccessUpdateMsgs:
        SET_INT_CJSON_CALLBACK(client, wccbkSuccessUpdateMsgs, setOnSuccessUpdateMsgs, func);
        break;
    case wccbkSuccessRequestRecordMeta:
        SET_INT_CJSON_CALLBACK(client, wccbkSuccessRequestRecordMeta, setOnSuccessRequestRecordMeta, func);
        break;
    case wccbkSuccessGetConfig:
        SET_INT_CJSON_CALLBACK(client, wccbkSuccessGetConfig, setOnSuccessGetConfig, func);
        break;
    case wccbkSuccessDeleteRecords:
        SET_INT_CJSON_CALLBACK(client, wccbkSuccessDeleteRecords, setOnSuccessDeleteRecords, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;
}

wcRCode DLLEXPORT wcSetJSONCallback(wcHandle client, wcCallback callbackId, JSONNotifyEventLibFunc func) {
    /*
        cJSONNotifyEvent mOnSuccessUpdateRecords {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessUpdateDevices {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessUpdateMsgs {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessSendMsg {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessGetConfig {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessDeleteRecords {NULL_cJSONNotifyEvent};
    */
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkSuccessUpdateRecords:
        SET_INT_JSON_CALLBACK(client, wccbkSuccessUpdateRecords, setOnAltSuccessUpdateRecords, func);
        break;
    case wccbkSuccessUpdateDevices:
        SET_INT_JSON_CALLBACK(client, wccbkSuccessUpdateDevices, setOnAltSuccessUpdateDevices, func);
        break;
    case wccbkSuccessUpdateStreams:
        SET_INT_JSON_CALLBACK(client, wccbkSuccessUpdateStreams, setOnAltSuccessUpdateStreams, func);
        break;
    case wccbkSuccessUpdateMsgs:
        SET_INT_JSON_CALLBACK(client, wccbkSuccessUpdateMsgs, setOnAltSuccessUpdateMsgs, func);
        break;
    case wccbkSuccessRequestRecordMeta:
        SET_INT_JSON_CALLBACK(client, wccbkSuccessRequestRecordMeta, setOnAltSuccessRequestRecordMeta, func);
        break;
    case wccbkSuccessGetConfig:
        SET_INT_JSON_CALLBACK(client, wccbkSuccessGetConfig, setOnAltSuccessGetConfig, func);
        break;
    case wccbkSuccessDeleteRecords:
        SET_INT_JSON_CALLBACK(client, wccbkSuccessDeleteRecords, setOnAltSuccessDeleteRecords, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;
}
#endif

wcRCode DLLEXPORT wcSetJSONStrCallback(wcHandle client, wcCallback callbackId, JSONStrNotifyEventLibFunc func) {
    /*
        cJSONNotifyEvent mOnSuccessUpdateRecords {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessUpdateDevices {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessUpdateMsgs {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessSendMsg {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessGetConfig {NULL_cJSONNotifyEvent};
        cJSONNotifyEvent mOnSuccessDeleteRecords {NULL_cJSONNotifyEvent};
    */
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkSuccessUpdateRecords:
        SET_INT_CSTR_CALLBACK(client, wccbkSuccessUpdateRecords, setOnAltStrSuccessUpdateRecords, func);
        break;
    case wccbkSuccessUpdateDevices:
        SET_INT_CSTR_CALLBACK(client, wccbkSuccessUpdateDevices, setOnAltStrSuccessUpdateDevices, func);
        break;
    case wccbkSuccessUpdateStreams:
        SET_INT_CSTR_CALLBACK(client, wccbkSuccessUpdateStreams, setOnAltStrSuccessUpdateStreams, func);
        break;
    case wccbkSuccessUpdateMsgs:
        SET_INT_CSTR_CALLBACK(client, wccbkSuccessUpdateMsgs, setOnAltStrSuccessUpdateMsgs, func);
        break;
    case wccbkSuccessRequestRecordMeta:
        SET_INT_CSTR_CALLBACK(client, wccbkSuccessRequestRecordMeta, setOnAltStrSuccessRequestRecordMeta, func);
        break;
    case wccbkSuccessGetConfig:
        SET_INT_CSTR_CALLBACK(client, wccbkSuccessGetConfig, setOnAltStrSuccessGetConfig, func);
        break;
    case wccbkSuccessDeleteRecords:
        SET_INT_CSTR_CALLBACK(client, wccbkSuccessDeleteRecords, setOnAltStrSuccessDeleteRecords, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;
}

#ifndef COMMON_FUNC_ONLY
wcRCode DLLEXPORT wcSetStringCallback(wcHandle client, wcCallback callbackId, StringNotifyLibFunc func) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkAddLog:
        SET_INT_STR_CALLBACK(client, wccbkAddLog, setOnAddLog, func);
        break;
    case wccbkSIDSetted:
        SET_INT_STR_CALLBACK(client, wccbkSIDSetted, setOnSIDSetted, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;

}
#endif

wcRCode DLLEXPORT wcSetCStringCallback(wcHandle client, wcCallback callbackId, CStringNotifyLibFunc func) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkAddLog:
        SET_INT_CSTR_CALLBACK(client, wccbkAddLog, setOnAddLogC, func);
        break;
    case wccbkSIDSetted:
        SET_INT_CSTR_CALLBACK(client, wccbkSIDSetted, setOnSIDSettedC, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;

}

#ifndef COMMON_FUNC_ONLY
wcRCode DLLEXPORT wcSetDataCallback(wcHandle client, wcCallback callbackId, DataNotifyEventLibFunc func) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkSuccessRequestRecord:
        SET_INT_CALLBACK(client, wccbkSuccessRequestRecord, setOnSuccessRequestRecord, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;
}
#endif

wcRCode DLLEXPORT wcSetAltDataCallback(wcHandle client, wcCallback callbackId, DataAltNotifyEventLibFunc func) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    switch (callbackId) {
    case wccbkSuccessRequestRecord:
        SET_INT_ALT_CALLBACK(client, wccbkSuccessRequestRecord, setOnAltSuccessRequestRecord, func);
        break;
    default:
        v = WC_BAD_PARAM;
        break;
    }
    return v;
}

wcRCode DLLEXPORT wcGetConfig(wcHandle client, void * data) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    WC_CLIENT(client)->getConfig(data);
    return v;
}

wcRCode DLLEXPORT wcSetConfig(wcHandle client, const char * cfg) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;

    wcJSON obj(cfg);
    if (obj.isValid()) {
        std::string v(cfg);
        WC_CLIENT(client)->setConfig(v);
    } else {
        v = WC_BAD_VALUE;
    }
    return v;
}

wcRCode DLLEXPORT wcDeleteRecords(wcHandle client, const char * indices, void * data) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    wcJSON obj(indices);
    if (obj.isValid()) {
        WC_CLIENT(client)->deleteRecords(obj.getcJSON(), data);
    } else {
        v = WC_BAD_VALUE;
    }
    return v;
}

wcRCode DLLEXPORT wcSendMsg(wcHandle client, const char * msg, void * data) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    wcJSON obj(msg);
    if (obj.isValid()) {
        WC_CLIENT(client)->sendMsg(obj.getcJSON(), data);
    } else {
        v = WC_BAD_VALUE;
    }
    return v;
}
#ifndef COMMON_FUNC_ONLY
wcRCode DLLEXPORT wcSendMsg_cJson(wcHandle client, cJSON * msg, void * data) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    if (ASSIGNED(msg)) {
        WC_CLIENT(client)->sendMsg(msg, data);
    } else {
        v = WC_BAD_VALUE;
    }
    return v;
}
#endif
wcRCode DLLEXPORT wcRequestRecord(wcHandle client, int rid, void * data) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    WC_CLIENT(client)->requestRecord(rid, data);
    return v;
}

wcRCode DLLEXPORT wcSaveRecord(wcHandle client, const void * aBuf, size_t  Sz, const char * meta, void * data) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    WC_CLIENT(client)->saveRecord(aBuf, Sz, meta, data);
    return v;
}

wcRCode DLLEXPORT wcLaunchOutStream(wcHandle client, const char * subProtocol, int delta, void * data) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    if (!ASSIGNED(subProtocol)) return WC_BAD_VALUE;
    WC_CLIENT(client)->launchOutStream(std::string(subProtocol), delta, data);
    return v;
}

wcRCode DLLEXPORT wcLaunchInStream(wcHandle client, const char * deviceName, void * data) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    WC_CLIENT(client)->launchInStream(std::string(deviceName), data);
    return v;
}

//frame
wcRCode DLLEXPORT wcClientGetFrameID(wcHandle client, long * id) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    if (!(WC_CLIENT(client)->getStreaming())) return WC_CLIENT_NOT_STREAMING;
    *id = WC_CLIENT(client)->getFrame()->getFrameID();
    return v;
}

wcRCode DLLEXPORT wcClientFrameLock(wcHandle client) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    WC_CLIENT(client)->getFrame()->lock();
    return v;
}

wcRCode DLLEXPORT wcClientFrameUnLock(wcHandle client) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    WC_CLIENT(client)->getFrame()->unlock();
    return v;
}

wcRCode DLLEXPORT wcClientFramePushData(wcHandle client, const void * data, size_t len) {
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    if (!(WC_CLIENT(client)->getStreaming())) return WC_CLIENT_NOT_STREAMING;

    wcMemoryStream * str = new wcMemoryStream(len);
    str->write(data, len);
    str->resetPos();
    WC_CLIENT(client)->getFrame()->nextFrame(str);
    return v;
}

wcRCode DLLEXPORT wcClientFrameGetData(wcHandle client, void ** data, size_t * len){
    wcRCode v = CheckClient(client);
    if (v != WC_OK) return v;
    if (!ASSIGNED(data)) return WC_BAD_PARAM;
    if (!ASSIGNED(len)) return WC_BAD_PARAM;
    if (!(WC_CLIENT(client)->getStreaming())) return WC_CLIENT_NOT_STREAMING;

    WC_CLIENT(client)->getFrame()->lock();
    wcCustomMemoryStream * str = WC_CLIENT(client)->getFrame()->getLstFrame();
    if (ASSIGNED(str)) {
        if ASSIGNED(*data) {
            if (*len >= str->getSize()) {
                *len = str->getSize();
                memcpy(*data, str->getMemory(), *len);
            } else {
                v = WC_NOT_ENOUGH_MEM;
            }
        } else {
            *data = malloc(str->getSize());
            if ASSIGNED(*data) {
                *len = str->getSize();
                memcpy(*data, str->getMemory(), *len);
            } else {
                v = WC_NOT_ENOUGH_MEM;
                *len = 0;
            }
        }
    } else {
        *len = 0;
    }
    WC_CLIENT(client)->getFrame()->unlock();

    return v;
}

//tasks
wcRCode DLLEXPORT wcTaskGetClass(wcTask task, uint32_t * id) {
    wcRCode v = CheckTask(task, WC_ALL_TASKS);
    if (v != WC_OK) return v;
    if (!ASSIGNED(id)) return WC_BAD_PARAM;

    *id = static_cast<wcHTTP2BackgroundTask *>(task)->getTaskClass();

    return v;
}

wcRCode DLLEXPORT wcTaskGetUserData(wcTask task, void ** data){
    wcRCode v = CheckTask(task, WC_ALL_TASKS);
    if (v != WC_OK) return v;
    if (!ASSIGNED(data)) return WC_BAD_PARAM;

    *data = static_cast<wcHTTP2BackgroundTask *>(task)->getData();

    return v;
}

wcRCode DLLEXPORT wcTaskSetUserData(wcTask task, void * data){
    wcRCode v = CheckTask(task, WC_ALL_TASKS);
    if (v != WC_OK) return v;

    static_cast<wcHTTP2BackgroundTask *>(task)->setData(data);

    return v;
}

wcRCode DLLEXPORT wcTaskLock(wcTask task){
    wcRCode v = CheckTask(task, WC_ALL_TASKS);
    if (v != WC_OK) return v;

    static_cast<wcHTTP2BackgroundTask *>(task)->lock();

    return v;
}

wcRCode DLLEXPORT wcTaskUnLock(wcTask task){
    wcRCode v = CheckTask(task, WC_ALL_TASKS);
    if (v != WC_OK) return v;

    static_cast<wcHTTP2BackgroundTask *>(task)->unlock();

    return v;
}

//streaming
wcRCode DLLEXPORT wcInTaskPopFrame(wcTask task, void ** data, size_t * len) {
    wcRCode v = CheckTask(task, WC_IN_STREAM_TASK);
    if (v != WC_OK) return v;
    if (!ASSIGNED(data)) return WC_BAD_PARAM;
    if (!ASSIGNED(len)) return WC_BAD_PARAM;

    wcCustomMemoryStream * strm = static_cast<wcHTTP2BackgroundInStreamTask *>(task)->popFrame();
    if ASSIGNED(strm) {
        if (ASSIGNED(*data)) {
            if (*len >= strm->getSize()) {
                *len = strm->getSize();
                memcpy(*data, strm->getMemory(), *len);
            } else
                v = WC_NOT_ENOUGH_MEM;
        } else {
            *len = strm->getSize();
            *data = malloc(*len);
            memcpy(*data, strm->getMemory(), *len);
        }
        delete strm;
    } else {
        *len = 0;
    }

    return v;
}

#ifdef __cplusplus
}
#endif
