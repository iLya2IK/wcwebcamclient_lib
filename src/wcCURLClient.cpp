/*===============================================================*/
/* Main class                                                    */
/* wcCURLClient.cpp                                              */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "wcCURLClient.h"

using namespace std;

wcCURLClient::wcCURLClient()
{
    mLog = new tsStringList();
    mSynchroFinishedTasks = new wcHTTP2BackgroundTasksProto();
    mTaskPool =  new wcHTTP2AsyncBackground();
    mUpdates = new wcHTTP2BackgroundTasksProto();

    mLastError = new tsInteger(0);
    mLastSubError = new tsInteger(0);

    mFrame = new wcHTTP2StreamFrame();

    mSetts = new wcHTTP2SettingsIntf();

    curl_global_init(CURL_GLOBAL_ALL);
    mConnected = true;
    disconnect();
}

wcCURLClient::~wcCURLClient()
{
    mTaskPool->terminate();
    mTaskPool->join();
    delete mTaskPool;

    curl_global_cleanup();

    delete mSetts;
    delete mSynchroFinishedTasks;
    delete mFrame;
    mUpdates->extractAll();
    delete mUpdates;

    delete mLog;
    delete mLastError;
    delete mLastSubError;
}

void wcCURLClient::start() {
    mTaskPool->start();
}

void wcCURLClient::tasksProceed()
{
    synchroUpdateTasks();
    synchroFinishTasks();
}

void wcCURLClient::proceed()
{
    if (getConnected())
    {
      if (getNeedToUpdateDevices())
        updateDevices();
      if (getNeedToUpdateRecords())
        updateRecords();
      if (getNeedToUpdateMsgs())
        updateMsgs();
      if (getNeedToUpdateStreams())
        updateStreams();
    }
}

void intern_SuccessAuth(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successAuth(ATask);
}

void intern_SuccessDeleteRecords(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successDeleteRecords(ATask);
}

void intern_SuccessGetConfig(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successGetConfig(ATask);
}

void intern_SuccessRequestRecord(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successRequestRecord(ATask);
}

void intern_SuccessRequestRecordMeta(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successRequestRecordMeta(ATask);
}

void intern_SuccessSaveRecord(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successSaveRecord(ATask);
}

void intern_SuccessSendMsg(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successSendMsg(ATask);
}

void intern_SuccessUpdateDevices(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successUpdateDevices(ATask);
}

void intern_SuccessUpdateStreams(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successUpdateStreams(ATask);
}

void intern_SuccessUpdateMsgs(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successUpdateMsgs(ATask);
}

void intern_SuccessUpdateRecords(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successUpdateRecords(ATask);
}

void intern_TaskFinished(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->taskFinished(ATask);
}

void intern_HasNextFrame(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->onHasNextFrame(ATask);
}

int intern_GetNextFrame(void* obj, void* ATask)
{
    return static_cast<wcCURLClient*>(obj)->onGetNextFrame(ATask);
}

void intern_SuccessIOStream(void* obj, void* ATask)
{
    static_cast<wcCURLClient*>(obj)->successIOStream(ATask);
}

bool wcCURLClient::getConnected()
{
    lock();
    bool res = mConnected;
    unlock();
    return res;
}

bool wcCURLClient::getVerifyTSL()
{
    return mSetts->getVerifyTSL();
}

string wcCURLClient::getDevice()
{
    return mSetts->getDevice();
}

string wcCURLClient::getMetaData()
{
    return mSetts->getMetaData();
}

string wcCURLClient::getHost()
{
    return mSetts->getHost();
}

string wcCURLClient::getSID()
{
    return mSetts->getSID();
}

string wcCURLClient::getEncodedSID()
{
    return mSetts->getEncodedSID();
}

string wcCURLClient::getProxy()
{
    return mSetts->getProxyStr();
}

string wcCURLClient::getProxyAuth()
{
    return mSetts->getProxyAuth();
}

string wcCURLClient::getLastMsgsStamp()
{
    lock();
    std::string res = mLastMsgsStamp;
    unlock();
    return res;
}

string wcCURLClient::getLastRecsStamp()
{
    lock();
    std::string res = mLastRecsStamp;
    unlock();
    return res;
}

bool wcCURLClient::getNeedToUpdateStreams()
{
    lock();
    bool res = mNeedToUpdateStreams;
    unlock();
    return res;
}

void wcCURLClient::clearLastMsgsStamp()
{
    lock();
    mLastMsgsStamp = std::string(REST_SYNC_MSG);
    unlock();
}

void wcCURLClient::clearLastRecsStamp()
{
    lock();
    mLastRecsStamp = "";
    unlock();
}

void wcCURLClient::setNeedToUpdateStreams(bool AValue)
{
    lock();
    mNeedToUpdateStreams = AValue;
    unlock();
}

bool wcCURLClient::getNeedToUpdateDevices()
{
    lock();
    bool res = mNeedToUpdateDevices;
    unlock();
    return res;
}

bool wcCURLClient::getNeedToUpdateMsgs()
{
    lock();
    bool res = mNeedToUpdateMsgs;
    unlock();
    return res;
}

bool wcCURLClient::getNeedToUpdateRecords()
{
    lock();
    bool res = mNeedToUpdateRecords;
    unlock();
    return res;
}

bool wcCURLClient::getStreaming()
{
    lock();
    bool res = mStreaming;
    unlock();
    return res;
}

void wcCURLClient::stopStreaming()
{
    if (getStreaming()) {
        //find streaming tasks and stop them
    }
}

void wcCURLClient::setNeedToUpdateMsgs(bool AValue)
{
    lock();
    mNeedToUpdateMsgs = AValue;
    unlock();
}

void wcCURLClient::setNeedToUpdateDevices(bool AValue)
{
    lock();
    mNeedToUpdateDevices = AValue;
    unlock();
}

void wcCURLClient::setNeedToUpdateRecords(bool AValue)
{
    lock();
    mNeedToUpdateRecords = AValue;
    unlock();
}

void wcCURLClient::setStreaming(bool AValue)
{
    lock();
    mStreaming = AValue;
    unlock();
}

void wcCURLClient::setConnected(bool AValue)
{
    lock();
    if (mConnected == AValue)
    {
        unlock();
        return;
    }
    mConnected = AValue;
    unlock();
    if _ASSIGNED(mOnConnected)
        EXEC_METHOD(mOnConnected, AValue);
}

void wcCURLClient::setSID(std::string& AValue)
{
    mSetts->setSID(AValue);
    if (_ASSIGNED(mOnSIDSetted))
        EXEC_METHOD(mOnSIDSetted, AValue);
    else
    if (_ASSIGNED(mOnSIDSettedC))
        EXEC_METHOD(mOnSIDSettedC, AValue.c_str());
}

wcHTTP2SettingsIntf* wcCURLClient::getSetts()
{
    return mSetts;
}

wcHTTP2BackgroundTasksProto * wcCURLClient::getUpdates()
{
    return mUpdates;
}

wcHTTP2BackgroundTasksProto * wcCURLClient::getSynchroFinishedTasks()
{
    return mSynchroFinishedTasks;
}

wcHTTP2AsyncBackground * wcCURLClient::getTaskPool()
{
    return mTaskPool;
}

void wcCURLClient::successAuth(void* ATask)
{
    wcHTTP2BackgroundTask* aTask = static_cast<wcHTTP2BackgroundTask*>(ATask);

    if (aTask->getErrorCode() == TASK_NO_ERROR)
    {
        wcJSON jObj = consumeResponseToObj(aTask);
        if (jObj.isValid())
        {
            wcJSON jData = jObj.getObjItem(JSON_RPC_SHASH);
            if (jData.isValid())
            {
                std::string res = std::string(jData.valueString());
                setSID(res);

                setConnected(true);
                setNeedToUpdateDevices(true);
                setNeedToUpdateRecords(true);
                setNeedToUpdateMsgs(true);
                if _ASSIGNED(mOnSuccessAuth)
                    EXEC_METHOD(mOnSuccessAuth, ATask);
            }
        }
    }
    else
        disconnect();
}

void wcCURLClient::disconnect()
{
    mTaskPool->getTasks()->terminate();
    lock();
    mConnected = false;
    mNeedToUpdateDevices = false;
    mNeedToUpdateRecords = false;
    mNeedToUpdateMsgs    = false;
    unlock();

    if _ASSIGNED(mOnDisconnect)
        EXEC_METHOD(mOnDisconnect, this);

    mLastMsgsStamp = std::string(REST_SYNC_MSG);
    mLastRecsStamp = "";
    setSID(mLastRecsStamp);
}

void wcCURLClient::authorize(const std::string & aName, const std::string & aPass)
{
    wcJSON jObj = jsonNewObject();
    jObj.add_cString(JSON_RPC_NAME, aName.c_str());
    jObj.add_cString(JSON_RPC_PASS, aPass.c_str());
    jObj.add_cString(JSON_RPC_DEVICE, getDevice().c_str());
    if (hasMetaData()) {
        jObj.add_cString(JSON_RPC_META, getMetaData().c_str());
    }
    std::string aStr = jObj.toString();
    disconnect();
    std::string aMsg("/authorize.json");
    doPost(aMsg, aStr, {intern_SuccessAuth, this}, false, NULL);
}

void wcCURLClient::doPost(const std::string & aPath, std::string & aContent,
          TaskNotify aOnSuccess)
{
    doPost(aPath, aContent, aOnSuccess, true, NULL);
}


void wcCURLClient::doPost(const std::string & aPath, std::string & aContent,
          TaskNotify aOnSuccess, void * data)
{
    doPost(aPath, aContent, aOnSuccess, true, data);
}

void wcCURLClient::doPost(const std::string & aPath, std::string & aContent,
                          TaskNotify aOnSuccess, bool silent, void * data)
{
    const char * ptr;
    if (aContent.size() > 0)
        ptr = aContent.c_str();
    else
        ptr = NULL;
    doPost(aPath, ptr, aContent.size(), aOnSuccess,
           true, silent, data);
}

void wcCURLClient::doPost(const std::string & aPath, const void * aContent,
                          size_t aContentSize, TaskNotify aOnSuccess, bool inStack,
                          bool silent, void* data)
{
    if (doInitMultiPipeling())
    {
        wcHTTP2BackgroundTask * Tsk = new wcHTTP2BackgroundTask(mTaskPool->getTasks(), mSetts, silent, data);
        Tsk->setOnFinish({intern_TaskFinished, this});
        Tsk->setOnSuccess(aOnSuccess);
        Tsk->setData(data);
        Tsk->doPost(aPath, aContent, aContentSize, inStack);
        mTaskPool->addTask(Tsk);
    }
}

void wcCURLClient::updateDevices()
{
    setNeedToUpdateDevices(false);
    wcJSON jObj = jsonNewObject();
    jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
    std::string aStr = jObj.toString();
    std::string aMsg("/getDevicesOnline.json");
    doPost(aMsg, aStr, {intern_SuccessUpdateDevices, this}, NULL);
}

void wcCURLClient::updateStreams()
{
    setNeedToUpdateStreams(false);
    wcJSON jObj = jsonNewObject();
    jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
    std::string aStr = jObj.toString();
    std::string aMsg("/getStreams.json");
    doPost(aMsg, aStr, {intern_SuccessUpdateStreams, this}, NULL);
}

void wcCURLClient::updateRecords()
{
    setNeedToUpdateRecords(false);
    wcJSON jObj = jsonNewObject();
    jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
    jObj.add_cString(JSON_RPC_STAMP, getLastRecsStamp().c_str());
    std::string aStr = jObj.toString();
    std::string aMsg("/getRecordCount.json");
    doPost(aMsg, aStr, {intern_SuccessUpdateRecords, this}, NULL);
}

void wcCURLClient::updateMsgs()
{
    setNeedToUpdateMsgs(false);
    wcJSON jObj = jsonNewObject();
    jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
    jObj.add_cString(JSON_RPC_STAMP, getLastMsgsStamp().c_str());
    std::string aStr = jObj.toString();
    std::string aMsg("/getMsgs.json");
    doPost(aMsg, aStr, {intern_SuccessUpdateMsgs, this});
}


void wcCURLClient::getConfig(void * data)
{
    wcJSON jObj = jsonNewObject();
    jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
    std::string aStr = jObj.toString();
    std::string aMsg("/getConfig.json");
    doPost(aMsg, aStr, {intern_SuccessGetConfig, this});
}

void wcCURLClient::setConfig(std::string& aStr)
{
    std::string aMsg("/setConfig.json");
    doPost(aMsg, aStr, NULL_TaskNotify, NULL);
}

void wcCURLClient::deleteRecords(cJSON* aIndices, void * data)
{
    if ASSIGNED(aIndices) {
        wcJSON jObj = jsonNewObject();
        jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
        jObj.add_cItem(JSON_RPC_RECORDS, aIndices);
        std::string aStr = jObj.toString();
        std::string aMsg("/deleteRecords.json");
        doPost(aMsg, aStr, {intern_SuccessDeleteRecords, this}, data);
    }
}

void wcCURLClient::sendMsg(cJSON* aMsg, void * data)
{
    if ASSIGNED(aMsg) {
        wcJSON jObj(aMsg, false);
        jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
        std::string aStr = jObj.toString();
        std::string aMsgStr("/addMsgs.json");
        doPost(aMsgStr, aStr, {intern_SuccessSendMsg, this}, data);
    }
}

void wcCURLClient::requestRecord(int rid, void * data)
{
    wcJSON jObj = jsonNewObject();
    jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
    jObj.add_cInt(JSON_RPC_RID, rid);
    std::string aStr = jObj.toString();
    std::string aMsg("/getRecordData.json");
    doPost(aMsg, aStr, {intern_SuccessRequestRecord, this}, data);
}

void wcCURLClient::requestRecordMeta(int rid, void * data)
{
    wcJSON jObj = jsonNewObject();
    jObj.add_cString(JSON_RPC_SHASH, getSID().c_str());
    jObj.add_cInt(JSON_RPC_RID, rid);
    std::string aStr = jObj.toString();
    std::string aMsg("/getRecordMeta.json");
    doPost(aMsg, aStr, {intern_SuccessRequestRecordMeta, this}, data);
}

void wcCURLClient::saveRecord(const void* aBuf, size_t Sz, const char * meta, void * data)
{
    std::string aMsg("/addRecord.json?");
    aMsg += JSON_RPC_SHASH;
    aMsg += "=";
    aMsg += getEncodedSID();
    if (strlen(meta) > 0) {
        aMsg += "&meta=";
        std::string aMeta(meta);
        aMsg += encodeHTTP(aMeta);
    }
    doPost(aMsg, aBuf, Sz, {intern_SuccessSaveRecord, this}, false, true, data);
}

void wcCURLClient::addLog(const std::string & aStr) {
    mLog->add(aStr);
    if _ASSIGNED(mOnAddLog)
        EXEC_METHOD(mOnAddLog, aStr);
    else
    if _ASSIGNED(mOnAddLogC)
        EXEC_METHOD(mOnAddLogC, aStr.c_str());
}

void wcCURLClient::addLog(const char * aStr) {
    std::string nStr(aStr);
    addLog(nStr);
}

void wcCURLClient::addLogF(const char * aStr, ...) {
    va_list args;
    va_start(args, aStr);
    std::string nStr = vstr_format(aStr, args);
    va_end(args);
    addLog(nStr);
}

int wcCURLClient::getLogLength() {
    return mLog->size();
}

std::string wcCURLClient::getLogAt(int atp) {
    return mLog->getAt(atp);
}

std::string wcCURLClient::popLog() {
    return mLog->pop();
}

void wcCURLClient::clearLog() {
    mLog->clear();
}

bool wcCURLClient::hasError()
{
    return mLastError->getValue() != 0;
}

int wcCURLClient::getLastError()
{
    return mLastError->getValue();
}

int wcCURLClient::getLastSubError()
{
    return mLastSubError->getValue();
}

void wcCURLClient::clearError()
{
    mLastError->setValue(0);
    mLastSubError->setValue(0);
}

void wcCURLClient::pushError(int aError, int aSubError)
{
    mLastError->setValue(aError);
    mLastSubError->setValue(aSubError);
}

void wcCURLClient::synchroFinishTasks()
{
    while (true) {
      wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(mSynchroFinishedTasks->pop_front());
      if ASSIGNED(Tsk)
      {
          if (Tsk->getErrorString().size() > 0)
            addLog(Tsk->getErrorString());

          switch (Tsk->getErrorCode()) {
              case CURLE_OK :
                if (!(Tsk->getIsSilent()) || (Tsk->getResponseCode() != 200)) {
                    addLogF("HTTP2 \"%s\". Code - %ld", Tsk->getPath().c_str(), Tsk->getResponseCode());
                }
                break;
              case TASK_ERROR_ATTACH_REQ :
                addLogF("Cant attach easy req to multi. Code - %ld", Tsk->getErrorSubCode());
                pushError(TASK_ERROR_ATTACH_REQ, Tsk->getErrorSubCode());
                break;
              case TASK_ERROR_CANT_EASY_CURL :
                addLogF("Cant create easy req. Code - %ld", Tsk->getErrorSubCode());
                pushError(TASK_ERROR_CANT_EASY_CURL, Tsk->getErrorSubCode());
                break;
              default :
                addLogF("HTTP2 \"%s\" FAIL. Code - %ld. Subcode - %ld",
                        Tsk->getPath().c_str(), Tsk->getErrorCode(), Tsk->getErrorSubCode());
                pushError(Tsk->getErrorCode(), Tsk->getErrorSubCode());
                break;
          }

          if ( Tsk->getErrorString().size() > 0 )
            addLog(Tsk->getErrorString());

          if _ASSIGNED(Tsk->getOnSuccess())
            EXEC_METHOD(Tsk->getOnSuccess(), Tsk);

          delete Tsk;
      } else
          break;
    }
}

void wcCURLClient::synchroUpdateTasks()
{
  while (true)
  {
    wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(mUpdates->pop_front());
    if ASSIGNED(Tsk)
    {
        Tsk->lock();
        if _ASSIGNED(mOnSynchroUpdateTask)
            EXEC_METHOD(mOnSynchroUpdateTask, Tsk);
        Tsk->unlock();
    } else
      break;
  }
}

#define JSON_CLIENT_CALLBACK(cb, tsk, obj) \
            if _ASSIGNED(mOn ## cb) \
                EXEC_METHOD(mOn ## cb, tsk, obj.getcJSON()); \
            else \
            if _ASSIGNED(mOnAlt ## cb) \
                JSON_EXEC_METHOD(mOnAlt ## cb, tsk, obj); \
            else \
            if _ASSIGNED(mOnAltStr ## cb) \
                JSON_EXEC_STR_METHOD(mOnAltStr ## cb, tsk, obj);

#define JSON_RESULT_CONSUME(cb) \
        wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(ATask); \
        if (Tsk->getErrorCode() == TASK_NO_ERROR) \
        { \
            wcJSON jObj = consumeResponseToObj(Tsk); \
            JSON_CLIENT_CALLBACK(cb, Tsk, jObj) \
        } else \
            disconnect();

void wcCURLClient::successDeleteRecords(void* ATask)
{
    JSON_RESULT_CONSUME(SuccessDeleteRecords);
}

void wcCURLClient::successGetConfig(void* ATask)
{
    JSON_RESULT_CONSUME(SuccessGetConfig);
}

void wcCURLClient::successRequestRecordMeta(void* ATask)
{
    JSON_RESULT_CONSUME(SuccessRequestRecordMeta);
}

void wcCURLClient::successRequestRecord(void* ATask)
{
    wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(ATask);
    if (Tsk->getErrorCode() == TASK_NO_ERROR)
    {
        Tsk->getResponse()->resetPos();

        if _ASSIGNED(mOnSuccessRequestRecord)
            EXEC_METHOD(mOnSuccessRequestRecord, Tsk, Tsk->getResponse());
        else
        if _ASSIGNED(mOnAltSuccessRequestRecord)
            EXEC_METHOD(mOnAltSuccessRequestRecord, Tsk, Tsk->getResponse()->getMemory(), Tsk->getResponse()->getSize());
    } else
        disconnect();
}

void wcCURLClient::successSaveRecord(void* ATask)
{
    wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(ATask);
    if (Tsk->getErrorCode() == TASK_NO_ERROR)
    {
        if _ASSIGNED(mOnSuccessSaveRecord)
            EXEC_METHOD(mOnSuccessSaveRecord, ATask);
    } else
        disconnect();
}

void wcCURLClient::successSendMsg(void* ATask)
{
    JSON_RESULT_CONSUME(SuccessSendMsg);
}

void wcCURLClient::successUpdateDevices(void* ATask)
{
    wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(ATask);
    if (Tsk->getErrorCode() == TASK_NO_ERROR)
    {
        wcJSON jObj = consumeResponseToObj(Tsk);

        if (jObj.isValid()) {
            wcJSON jArr = jObj.getObjItem(JSON_RPC_DEVICES);
            if (jArr.isValid())
            {
                JSON_CLIENT_CALLBACK(SuccessUpdateDevices, ATask, jArr);
            }
        } else
            disconnect();
    } else
        disconnect();
}

void wcCURLClient::successUpdateStreams(void* ATask)
{
    wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(ATask);
    if (Tsk->getErrorCode() == TASK_NO_ERROR)
    {
        wcJSON jObj = consumeResponseToObj(Tsk);

        if (jObj.isValid()) {
            wcJSON jArr = jObj.getObjItem(JSON_RPC_DEVICES);
            if (jArr.isValid())
            {
                JSON_CLIENT_CALLBACK(SuccessUpdateStreams, ATask, jArr);
            }
        } else
            disconnect();
    } else
        disconnect();
}

void wcCURLClient::successUpdateMsgs(void* ATask)
{
    wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(ATask);
    if (Tsk->getErrorCode() == TASK_NO_ERROR)
    {
        wcJSON jObj = consumeResponseToObj(Tsk);

        if (jObj.isValid()) {
            wcJSON jArr = jObj.getObjItem(JSON_RPC_MSGS);
            if (jArr.isValid())
            {
                JSON_CLIENT_CALLBACK(SuccessUpdateMsgs, ATask, jArr);

                int size = jArr.length();
                for (int i = 0; i < size; i++) {
                    wcJSON jEl = jArr.getArrayItem(i);
                    wcJSON stamp = jEl.getObjItem(JSON_RPC_STAMP);
                    if (stamp.isValid()) {
                        setLastMsgsStamp(std::string(stamp.valueString()));
                        break;
                    }
                }
            }
        }
    } else
        disconnect();
}

void wcCURLClient::successUpdateRecords(void* ATask)
{
    wcHTTP2BackgroundTask * Tsk = static_cast< wcHTTP2BackgroundTask * >(ATask);
    if (Tsk->getErrorCode() == TASK_NO_ERROR)
    {
        wcJSON jObj = consumeResponseToObj(Tsk);

        if (jObj.isValid()) {
            wcJSON jArr = jObj.getObjItem(JSON_RPC_RECORDS);
            if (jArr.isValid())
            {
                JSON_CLIENT_CALLBACK(SuccessUpdateRecords, ATask, jArr);

                int size = jArr.length();
                for (int i = 0; i < size; i++) {
                    wcJSON jEl = jArr.getArrayItem(i);
                    wcJSON stamp = jEl.getObjItem(JSON_RPC_STAMP);
                    if (stamp.isValid()) {
                        setLastRecsStamp(std::string(stamp.valueString()));
                        break;
                    }
                }
            }
        }
    } else
        disconnect();
}

void wcCURLClient::taskFinished(void* ATask)
{
    mSynchroFinishedTasks->push_back(static_cast<wcHTTP2BackgroundTask*>(ATask));
}

wcJSON wcCURLClient::consumeResponseToObj(wcHTTP2BackgroundTask* ATask)
{
    wcJSON Result(NULL, false);
    int aCode = -1;
    std::string aResult(JSON_RPC_BAD);
    ATask->getResponse()->resetPos();
    if (ATask->getResponse()->getSize() > 0)
    {
      try {
          wcJSON jData = json_parse(ATask->getResponse()->getNullTerminated());
          if (jData.isObject()) {
              Result = jData;
              jData = Result.getObjItem(JSON_RPC_RESULT);
              if (jData.isString())
              {
                if (strcmp(jData.valueString(), JSON_RPC_BAD) == 0)
                {
                    jData = Result.getObjItem(JSON_RPC_CODE);
                    if (jData.isNumber())
                        aCode = jData.valueInt();
                    Result.clean();
                } else
                    aCode = 0;
              } else {
                Result.clean();
              }
          }
      }
      catch (const std::exception & e) {
          Result.clean();

          addLog(e.what());
      }
    }
    if ((aCode > 0) || (!ATask->getIsSilent()))
    {
        int aRCode;
        if (aCode < 0)
            aRCode = 1;
        else
        if (aCode > MAX_RESPONSE_ERRORS)
            aRCode = 1;
        else
            aRCode = aCode;
        addLogF("HTTP2 JSON Req result code [%d] - %s", aCode, RESPONSE_ERRORS[aRCode]);
    }
    return Result;
}

bool wcCURLClient::doInitMultiPipeling()
{
    bool res = mTaskPool->getTasks()->doInitMultiPipeling();

    if (res) {
        if (_ASSIGNED(mOnInitCURL) && !mInitialized)
          EXEC_METHOD(mOnInitCURL,  mTaskPool->getTasks());
        mInitialized = true;
    } else
        mInitialized = false;
    return res;
}

bool wcCURLClient::launchOutStream(void * data)
{
    if (doInitMultiPipeling()) {
        wcHTTP2BackgroundOutStreamTask * Tsk = new wcHTTP2BackgroundOutStreamTask(getTaskPool()->getTasks(), mSetts, true, NULL);
        Tsk->setOnFinish({intern_TaskFinished, this});
        Tsk->setOnSuccess({intern_SuccessIOStream, this});
        Tsk->setData(data);

        if _ASSIGNED(mOnAfterLaunchOutStream)
          EXEC_METHOD(mOnAfterLaunchOutStream, Tsk);

        Tsk->launchStream({intern_GetNextFrame, this});
        getTaskPool()->addTask(Tsk);
        setStreaming(true);

        return true;
    }
    return false;
}

bool wcCURLClient::launchInStream(const std::string& aDevice, void * data)
{
    if (doInitMultiPipeling()) {
        wcHTTP2BackgroundInStreamTask * Tsk = new wcHTTP2BackgroundInStreamTask(getTaskPool()->getTasks(), mSetts, true, NULL);
        Tsk->setOnFinish({intern_TaskFinished, this});
        Tsk->setOnSuccess({intern_SuccessIOStream, this});
        Tsk->setData(data);

        if _ASSIGNED(mOnAfterLaunchInStream)
          EXEC_METHOD(mOnAfterLaunchInStream, Tsk);

        Tsk->launchStream(aDevice, {intern_HasNextFrame, this});
        getTaskPool()->addTask(Tsk);

        return true;
    }
    return false;
}

void wcCURLClient::onHasNextFrame(void* ATask)
{
    wcHTTP2BackgroundTask* ATaskObj = static_cast<wcHTTP2BackgroundTask*>(ATask);
    mUpdates->push_back(ATaskObj);
}

void wcCURLClient::successIOStream(void* ATask)
{
    wcHTTP2BackgroundTask* ATaskObj = static_cast<wcHTTP2BackgroundTask*>(ATask);
    ATaskObj->lock();
    //if (dynamic_cast<wcHTTP2BackgroundOutStreamTask*>(ATaskObj) != NULL)
    if (ATaskObj->getTaskClass() == WC_OUT_STREAM_TASK)
        setStreaming(false);
    ATaskObj->unlock();
    if _ASSIGNED(mOnSuccessIOStream)
        EXEC_METHOD(mOnSuccessIOStream, ATask);
}

int wcCURLClient::onGetNextFrame(void* ATask)
{
    if ASSIGNED(mFrame->getFrame())
    {
        wcHTTP2BackgroundOutStreamTask* ATaskObj = static_cast<wcHTTP2BackgroundOutStreamTask*>(ATask);
        mFrame->lock();
        ATaskObj->pushFrame(mFrame->extractFrame());
        int res = mFrame->getFrameID();
        mFrame->unlock();
        mUpdates->push_back(ATaskObj);
        return res;
    } else
        return 0;
}

