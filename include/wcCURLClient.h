#ifndef WCCURLCLIENT_H
#define WCCURLCLIENT_H

#include <wcDefines.h>
#include <wcCommonCallbacks.h>
#include <tsObject.h>
#include <tsInteger.h>
#include <wcHTTP2AsyncBackground.h>
#include <wcHTTP2BackgroundTask.h>
#include <wcStreaming.h>
#include <wcHTTP2SettingsIntf.h>
#include <tsStringList.h>
#include <wcJSONUtils.h>
#include <wcJSON.h>


#define DECL_JSON_RESPONSE_CALLBACK(cb) \
        void setOn ## cb(cJSONNotifyEventFunc aValue) {mOn ## cb = cJSONNotifyEventSet(aValue, this);}; \
        void setOnAlt ## cb(JSONNotifyEventFunc aValue) {mOnAlt ## cb = JSONNotifyEventSet(aValue, this);}; \
        void setOnAltStr ## cb(JSONStrNotifyEventFunc aValue) {mOnAltStr ## cb = JSONStrNotifyEventSet(aValue, this);}; \
        cJSONNotifyEventFunc getOn ## cb() {return mOn ## cb.exec;}; \
        JSONNotifyEventFunc getOnAlt ## cb() {return mOnAlt ## cb.exec;}; \
        JSONStrNotifyEventFunc getOnAltStr ## cb() {return mOnAltStr ## cb.exec;};

#define DECL_JSON_RESPONSE_CALLBACK_PRIVATE(cb) \
        cJSONNotifyEvent mOn ## cb {NULL_cJSONNotifyEvent}; \
        JSONNotifyEvent mOnAlt ## cb {NULL_JSONNotifyEvent}; \
        JSONStrNotifyEvent mOnAltStr ## cb {NULL_JSONStrNotifyEvent};

//setIntClientCallback(handle, cb, (void*)func);WC_CLIENT(handle)->meth(lib_ ## cb);

class wcCURLClient : public tsObject
{
    public:
        wcCURLClient();
        virtual ~wcCURLClient();

        void setHost(const std::string & aHost) { mSetts->setHost(aHost); };
        void setMetaData(const std::string & aMeta) { mSetts->setMetaData(aMeta); };
        void setDevice(const std::string & aDevice) { mSetts->setDevice(aDevice); };
        void setProxy(const std::string & aValue) { mSetts->setProxy(aValue); };
        void setProxyProtocol(const std::string & aValue) { mSetts->setProxyProt(aValue); };
        void setProxyHost(const std::string & aValue) { mSetts->setProxyHost(aValue); };
        void setProxyPort(const std::string & aValue) { mSetts->setProxyPort(aValue); };
        void setProxyUser(const std::string & aValue) { mSetts->setProxyAuthName(aValue); };
        void setProxyPwrd(const std::string & aValue) { mSetts->setProxyAuthPass(aValue); };
        void setVerifyTSL(bool aValue) { mSetts->setVerifyTSL(aValue); };
        void setLastMsgsStamp(const std::string & aValue) { lock(); mLastMsgsStamp = aValue; unlock(); };
        void setLastRecsStamp(const std::string & aValue) { lock(); mLastRecsStamp = aValue; unlock(); };
        void start();
        void tasksProceed();
        void proceed();

        bool getVerifyTSL();
        bool hasMetaData() { return mSetts->hasMetaData(); };
        std::string getMetaData();
        std::string getDevice();
        std::string getHost();
        std::string getSID();
        std::string getEncodedSID();
        std::string getProxy();
        std::string getProxyAuth();
        std::string getLastMsgsStamp();
        std::string getLastRecsStamp();

        bool getConnected();
        bool getNeedToUpdateDevices();
        bool getNeedToUpdateMsgs();
        bool getNeedToUpdateStreams();
        bool getNeedToUpdateRecords();
        bool getStreaming();
        bool hasError();
        int  getLastError();
        int  getLastSubError();
        void clearError();
        void clearLastMsgsStamp();
        void clearLastRecsStamp();

        void setNeedToUpdateMsgs(bool AValue);
        void setNeedToUpdateDevices(bool AValue);
        void setNeedToUpdateStreams(bool AValue);
        void setNeedToUpdateRecords(bool AValue);
        void stopStreaming();

        void authorize(const std::string & aName, const std::string & aPass);
        void doPost(const std::string & aPath, std::string & aContent,
          TaskNotify aOnSuccess);
        void doPost(const std::string & aPath, std::string & aContent,
          TaskNotify aOnSuccess, void * data);
        void doPost(const std::string & aPath, std::string & aContent,
          TaskNotify aOnSuccess, bool silent, void * data);
        void doPost(const std::string & aPath, const void * aContent,
          size_t aContentSize, TaskNotify aOnSuccess, bool inStack,
          bool silent, void * data);
        void getConfig(void * data = NULL);
        void setConfig(std::string & aStr);
        void deleteRecords(cJSON * aIndices, void * data = NULL);
        void sendMsg(cJSON * aMsg, void * data = NULL);
        void requestRecord(int rid, void * data = NULL);
        void requestRecordMeta(int rid, void * data);
        void saveRecord(const void * aBuf, size_t  Sz, const char * meta, void * data = NULL);
        bool launchOutStream(void * data);
        bool launchInStream(const std::string & aDevice, void * data);
        void disconnect();

        void addLog(const std::string & aStr);
        void addLog(const char * aStr);
        void addLogF(const char * aStr, ...);
        int getLogLength();
        std::string getLogAt(int atp);
        std::string popLog();
        void clearLog();

        wcHTTP2StreamFrame * getFrame() { return mFrame; }

        DECL_JSON_RESPONSE_CALLBACK(SuccessUpdateRecords);
        DECL_JSON_RESPONSE_CALLBACK(SuccessUpdateDevices);
        DECL_JSON_RESPONSE_CALLBACK(SuccessUpdateStreams);
        DECL_JSON_RESPONSE_CALLBACK(SuccessUpdateMsgs);
        DECL_JSON_RESPONSE_CALLBACK(SuccessSendMsg);
        DECL_JSON_RESPONSE_CALLBACK(SuccessGetConfig);
        DECL_JSON_RESPONSE_CALLBACK(SuccessDeleteRecords);
        DECL_JSON_RESPONSE_CALLBACK(SuccessRequestRecordMeta);

        void setOnInitCURL(NotifyEventFunc aValue) {mOnInitCURL = NotifyEventSet(aValue, this);};
        void setOnConnectedChanged(ConnNotifyEventFunc aValue) {mOnConnected = ConnNotifyEventSet(aValue, this);};
        void setOnDisconnect(NotifyEventFunc aValue) {mOnDisconnect = NotifyEventSet(aValue, this);};
        void setOnAddLog(StringNotifyFunc aValue) {mOnAddLog = StringNotifySet(aValue, this);};
        void setOnAddLogC(CStringNotifyFunc aValue) {mOnAddLogC = CStringNotifySet(aValue, this);};
        void setOnSuccessAuth(TaskNotifyFunc aValue) {mOnSuccessAuth = TaskNotifySet(aValue, this);};
        void setOnSuccessSaveRecord(TaskNotifyFunc aValue) {mOnSuccessSaveRecord = TaskNotifySet(aValue, this);};
        void setOnAfterLaunchInStream(TaskNotifyFunc aValue) {mOnAfterLaunchInStream = TaskNotifySet(aValue, this);};
        void setOnAfterLaunchOutStream(TaskNotifyFunc aValue) {mOnAfterLaunchOutStream = TaskNotifySet(aValue, this);};
        void setOnSIDSetted(StringNotifyFunc aValue) {mOnSIDSetted = StringNotifySet(aValue, this);};
        void setOnSIDSettedC(CStringNotifyFunc aValue) {mOnSIDSettedC = CStringNotifySet(aValue, this);};
        void setOnSynchroUpdateTask(TaskNotifyFunc aValue) {mOnSynchroUpdateTask = TaskNotifySet(aValue, this);};
        void setOnSuccessIOStream(TaskNotifyFunc aValue) {mOnSuccessIOStream = TaskNotifySet(aValue, this);};
        void setOnSuccessRequestRecord(DataNotifyEventFunc aValue) {mOnSuccessRequestRecord = DataNotifyEventSet(aValue, this);};
        void setOnAltSuccessRequestRecord(DataAltNotifyEventFunc aValue) {mOnAltSuccessRequestRecord = DataAltNotifyEventSet(aValue, this);};

        NotifyEventFunc getOnInitCURL() {return mOnInitCURL.exec;};
        ConnNotifyEventFunc getOnConnectedChanged() {return mOnConnected.exec;};
        NotifyEventFunc getOnDisconnect() {return mOnDisconnect.exec;};
        StringNotifyFunc getOnAddLog() {return mOnAddLog.exec;};
        CStringNotifyFunc getOnAddLogC() {return mOnAddLogC.exec;};
        TaskNotifyFunc getOnSuccessAuth() {return mOnSuccessAuth.exec;};
        DataNotifyEventFunc getOnSuccessRequestRecord() {return mOnSuccessRequestRecord.exec;};
        DataAltNotifyEventFunc getOnAltSuccessRequestRecord() {return mOnAltSuccessRequestRecord.exec;};
        TaskNotifyFunc getOnSuccessSaveRecord() {return mOnSuccessSaveRecord.exec;};
        TaskNotifyFunc getOnAfterLaunchInStream() {return mOnAfterLaunchInStream.exec;};
        TaskNotifyFunc getOnAfterLaunchOutStream() {return mOnAfterLaunchOutStream.exec;};
        StringNotifyFunc getOnSIDSetted() {return mOnSIDSetted.exec;};
        TaskNotifyFunc getOnSynchroUpdateTask() {return mOnSynchroUpdateTask.exec;};
        TaskNotifyFunc getOnSuccessIOStream() {return mOnSuccessIOStream.exec;};

        virtual void successAuth(void* ATask);
        virtual void successDeleteRecords(void* ATask);
        virtual void successGetConfig(void* ATask);

        virtual void successRequestRecord(void* ATask);
        virtual void successRequestRecordMeta(void* ATask);
        virtual void successSaveRecord(void* ATask);
        virtual void successSendMsg(void* ATask);
        virtual void successUpdateDevices(void* ATask);
        virtual void successUpdateStreams(void* ATask);
        virtual void successUpdateMsgs(void* ATask);
        virtual void successUpdateRecords(void* ATask);
        virtual void taskFinished(void* ATask);

        virtual void onHasNextFrame(void* ATask);
        virtual void successIOStream(void* ATask);
        virtual int  onGetNextFrame(void* ATask);
    protected:
        std::string mLastMsgsStamp, mLastRecsStamp;

        void updateDevices();
        void updateStreams();
        void updateRecords();
        void updateMsgs();
        virtual void synchroFinishTasks();
        virtual void synchroUpdateTasks();

        virtual void setConnected(bool AValue);
        void setStreaming(bool AValue);
        void setSID(std::string& AValue);

        virtual wcJSON consumeResponseToObj(wcHTTP2BackgroundTask * ATask);

        wcHTTP2BackgroundTasksProto * getUpdates();
        wcHTTP2BackgroundTasksProto * getSynchroFinishedTasks();
        wcHTTP2AsyncBackground * getTaskPool();
        wcHTTP2SettingsIntf * getSetts();

        void pushError(int aError, int aSubError);
        bool doInitMultiPipeling();
    private:
        bool mConnected{false},
            mNeedToUpdateDevices{false},
            mNeedToUpdateRecords{false},
            mNeedToUpdateMsgs{false},
            mNeedToUpdateStreams{false},
            mStreaming{false},
            mInitialized{false};

        TaskNotify mOnSuccessIOStream {NULL_TaskNotify};
        TaskNotify mOnSynchroUpdateTask {NULL_TaskNotify};
        NotifyEvent mOnDisconnect {NULL_NotifyEvent};
        StringNotify mOnAddLog {NULL_StringNotify};
        CStringNotify mOnAddLogC {NULL_CStringNotify};
        TaskNotify mOnAfterLaunchInStream {NULL_TaskNotify};
        TaskNotify mOnAfterLaunchOutStream {NULL_TaskNotify};

        DECL_JSON_RESPONSE_CALLBACK_PRIVATE(SuccessUpdateRecords);
        DECL_JSON_RESPONSE_CALLBACK_PRIVATE(SuccessUpdateDevices);
        DECL_JSON_RESPONSE_CALLBACK_PRIVATE(SuccessUpdateStreams);
        DECL_JSON_RESPONSE_CALLBACK_PRIVATE(SuccessUpdateMsgs);
        DECL_JSON_RESPONSE_CALLBACK_PRIVATE(SuccessSendMsg);
        DECL_JSON_RESPONSE_CALLBACK_PRIVATE(SuccessGetConfig);
        DECL_JSON_RESPONSE_CALLBACK_PRIVATE(SuccessDeleteRecords);
        DECL_JSON_RESPONSE_CALLBACK_PRIVATE(SuccessRequestRecordMeta);

        NotifyEvent mOnInitCURL {NULL_NotifyEvent};
        TaskNotify mOnSuccessSaveRecord {NULL_TaskNotify};
        DataNotifyEvent mOnSuccessRequestRecord {NULL_DataNotifyEvent};
        DataAltNotifyEvent mOnAltSuccessRequestRecord {NULL_DataAltNotifyEvent};
        ConnNotifyEvent mOnConnected {NULL_ConnNotifyEvent};
        TaskNotify mOnSuccessAuth {NULL_TaskNotify};
        StringNotify mOnSIDSetted {NULL_StringNotify};
        CStringNotify mOnSIDSettedC {NULL_CStringNotify};

        wcHTTP2BackgroundTasksProto * mUpdates {NULL};
        wcHTTP2StreamFrame * mFrame {NULL};
        wcHTTP2BackgroundTasksProto * mSynchroFinishedTasks {NULL};
        wcHTTP2AsyncBackground * mTaskPool {NULL};
        wcHTTP2SettingsIntf * mSetts {NULL};

        tsStringList * mLog {NULL};

        tsInteger * mLastError;
        tsInteger * mLastSubError;
};

#endif // WCCURLCLIENT_H
