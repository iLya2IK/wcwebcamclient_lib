/*===============================================================*/
/* Class to work with background tasks                           */
/* wcHTTP2BackgroundTask.cpp                                     */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "wcHTTP2BackgroundTask.h"

size_t WriteFunctionCallback(void * ptr, size_t len, size_t nmemb, void * data)
{
    return static_cast<wcHTTP2BackgroundTask*>(data)->write(ptr, len, nmemb);
}

size_t SeekFunctionCallback(void * ptr, curl_off_t offset, long origin)
{
    return static_cast<wcHTTP2BackgroundTask*>(ptr)->seek(offset, origin);
}

size_t ReadFunctionCallback(void * ptr, size_t len, size_t nmemb, void * data)
{
    return static_cast<wcHTTP2BackgroundTask*>(data)->read(ptr, len, nmemb);
}

wcHTTP2BackgroundTask::wcHTTP2BackgroundTask(wcHTTP2BackgroundTasks * aPool,
  wcHTTP2SettingsIntf * Settings, bool aIsSilent, void * aData)
{
  mCURL = NULL;
  mPool = aPool;
  mSettings = Settings;
  mErrorCode = TASK_NO_ERROR;
  mErrorSubCode = 0;
  mState = STATE_INIT;
  mIsSilent = aIsSilent;

  mResponse = new wcMemoryStream();
  mRequest = NULL;

  mData = aData;
}

wcHTTP2BackgroundTask::~wcHTTP2BackgroundTask()
{
  finalize();
  if (ASSIGNED(mResponse)) delete mResponse;
  if (ASSIGNED(mRequest)) delete mRequest;
}

void wcHTTP2BackgroundTask::CURLConfig(int64_t mContentSize) {
    //
    curl_easy_setopt(mCURL, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
    curl_easy_setopt(mCURL, CURLOPT_URL, std::string(mSettings->getHost() + mPath).c_str());

    if (mSettings->hasProxy()) {
        curl_easy_setopt(mCURL, CURLOPT_PROXY, mSettings->getProxyStr().c_str());
        if (mSettings->hasProxyAuth()) {
            curl_easy_setopt(mCURL, CURLOPT_PROXYAUTH, CURLAUTH_ANYSAFE);
            curl_easy_setopt(mCURL, CURLOPT_PROXYUSERPWD, mSettings->getProxyAuth().c_str());
        }
    }

    if (!mSettings->getVerifyTSL()) {
        curl_easy_setopt(mCURL, CURLOPT_SSL_VERIFYPEER, (long)0);
        curl_easy_setopt(mCURL, CURLOPT_SSL_VERIFYHOST, (long)0);
    }

    curl_easy_setopt(mCURL, CURLOPT_WRITEDATA, (void*)this);
    curl_easy_setopt(mCURL, CURLOPT_WRITEFUNCTION, WriteFunctionCallback);
    std::string v = std::to_string(mContentSize);
    mHeaders = curl_slist_append(mHeaders, std::string(std::string("content-length: ") + v).c_str());
    curl_easy_setopt(mCURL, CURLOPT_HTTPHEADER, mHeaders);
    curl_easy_setopt(mCURL, CURLOPT_PIPEWAIT, (long)1);
    curl_easy_setopt(mCURL, CURLOPT_NOSIGNAL, (long)1);

    curl_easy_setopt(mCURL, CURLOPT_ERRORBUFFER, mErrorBuffer);
    memset(mErrorBuffer, 0, CURL_ERROR_SIZE);
}

void wcHTTP2BackgroundTask::CURLSetPost() {
    curl_easy_setopt(mCURL, CURLOPT_POST, (long)1);
}

void wcHTTP2BackgroundTask::CURLSetUpload() {
    curl_easy_setopt(mCURL, CURLOPT_UPLOAD, (long)1);
}

void wcHTTP2BackgroundTask::CURLSetSeekFunction() {
    curl_easy_setopt(mCURL, CURLOPT_SEEKFUNCTION, SeekFunctionCallback);
    curl_easy_setopt(mCURL, CURLOPT_SEEKDATA, (void*)this);
}

void wcHTTP2BackgroundTask::CURLSetReadFunction(int64_t sz) {
    curl_easy_setopt(mCURL, CURLOPT_READDATA, (void*)this);
    curl_easy_setopt(mCURL, CURLOPT_READFUNCTION, ReadFunctionCallback);
    curl_easy_setopt(mCURL, CURLOPT_INFILESIZE, (long)sz);
    curl_easy_setopt(mCURL, CURLOPT_INFILESIZE_LARGE, (curl_off_t)sz);
}

bool wcHTTP2BackgroundTask::doPost(const std::string& aPath, const void* aContent, size_t aContentSize, bool inStack) {
  bool res= false;

  if (mPool->ready())
  {
    mCURL = curl_easy_init();
    if (ASSIGNED(mCURL))
    {
        mResponse->clearStr();
        mPath = aPath;

        CURLConfig((int64_t)aContentSize);
        CURLSetPost();

        if (aContentSize > 0) {
            if (inStack) {
                mRequest = new wcMemoryStream();
                static_cast<wcMemoryStream*>(mRequest)->write(aContent,  aContentSize);
                mRequest->resetPos();
            } else
                mRequest = new wcCustomMemoryStream(aContent,  aContentSize);

            CURLSetReadFunction((int64_t)aContentSize);
        }

        attachToPool();

        res = true;
    } else
        doError(TASK_ERROR_CANT_EASY_CURL);
  }
  return res;
}

long wcHTTP2BackgroundTask::seek(curl_off_t offset, long origin)
{
  lock();
    mRequest->seek(offset, origin);
  unlock();
  return CURL_SEEKFUNC_OK;
}

size_t wcHTTP2BackgroundTask::write(const void* ptr, size_t len, size_t nmemb)
{
  if (finished()) return 0;

  return mResponse->write(ptr, len * nmemb);
}

size_t wcHTTP2BackgroundTask::read(void* ptr, size_t len, size_t nmemb)
{
  if (finished()) return CURL_READFUNC_ABORT;

  if (ASSIGNED(mRequest))
    return mRequest->read(ptr, nmemb * len);
  else
    return 0;
}

void wcHTTP2BackgroundTask::terminate()
{
    setState(STATE_TERMINATED);
}

void wcHTTP2BackgroundTask::close()
{
    terminate();
}

void wcHTTP2BackgroundTask::finalize()
{
  if ASSIGNED(mCURL)
  {
    mPool->lockCURLM();
        curl_multi_remove_handle(mPool->getCURLM()->getValueUnsafe(), mCURL);
    mPool->unlockCURLM();
    if ASSIGNED(mHeaders)
      curl_slist_free_all(mHeaders);
    curl_easy_cleanup(mCURL);
    mCURL = NULL;
  }
}

void wcHTTP2BackgroundTask::attachToPool() {
  mPool->lockCURLM();
    mErrorSubCode = (long)curl_multi_add_handle(mPool->getCURLM()->getValueUnsafe(), mCURL);
    if (mErrorSubCode != CURLE_OK)
      doError(TASK_ERROR_ATTACH_REQ, mErrorSubCode);
  mPool->unlockCURLM();
}

std::string wcHTTP2BackgroundTask::getErrorString() {
    lock();
    std::string res(mErrorBuffer);
    unlock();
    return res;
}

void wcHTTP2BackgroundTasks_idleTask(void* obj, void* aTsk)
{
    static_cast<wcHTTP2BackgroundTask*>(aTsk)->doIdle();
}

void wcHTTP2BackgroundTasks_terminateTask(void* obj, void* aTsk)
{
    static_cast<wcHTTP2BackgroundTask*>(aTsk)->terminate();
}

void wcHTTP2BackgroundTasks_afterTaskExtract(void* obj, void* aTsk)
{
    wcHTTP2BackgroundTask* aTask = static_cast<wcHTTP2BackgroundTask*>(aTsk);
    if (aTask->hasOnFinish()) {
        aTask->doFinish();
    }
    else
        delete aTask;
}

bool wcHTTP2BackgroundTasks_isTaskFinished(void* obj, void* aTsk, const void* data)
{
    return (static_cast<wcHTTP2BackgroundTask*>(aTsk))->finished();
}

void wcHTTP2BackgroundTasks_setTaskFinished(void* obj, void* aTsk, const void* data)
{
    wcHTTP2BackgroundTask* aTask = static_cast<wcHTTP2BackgroundTask*>(aTsk);
    if (aTask->getEasyCURL() == ((CURLMsg*)data)->easy_handle) {
        aTask->setState(STATE_FINISHED);
        if (((CURLMsg*)data)->data.result != CURLE_OK)
        {
            aTask->doError(TASK_ERROR_CURL, ((CURLMsg*)data)->data.result);
        }
        else
        {
            long rc, sb = curl_easy_getinfo(((CURLMsg*)data)->easy_handle,
                                                     CURLINFO_RESPONSE_CODE,
                                                     &rc);
            if (sb == CURLE_OK)
                aTask->setResponseCode(rc);
            else
                aTask->doError(TASK_ERROR_GET_INFO, sb);
        }
    }
}

void wcHTTP2BackgroundTasks_setMultiPollError(void* obj, void* aTsk, const void* data)
{
  static_cast<wcHTTP2BackgroundTask*>(aTsk)->doError(TASK_ERROR_CURL, *((long *)data));
}

void wcHTTP2BackgroundTasks::doMultiError(long code)
{
  doForAllEx(DoExecuteExSet(wcHTTP2BackgroundTasks_setMultiPollError, this), &code);
  lock();
    if (ASSIGNED(mCURLM)) {
        delete mCURLM;
        mCURLM = NULL;
    }
  unlock();
  if (_ASSIGNED(mOnMultiError))
    EXEC_METHOD(mOnMultiError);
}

bool wcHTTP2BackgroundTasks::doInitMultiPipeling()
{
    if (!ASSIGNED(mCURLM->getValue())) {
        mCURLM->lock();
            mCURLM->initCURLM();
            bool res = ASSIGNED(mCURLM->getValue());
        mCURLM->unlock();

        return res;
    } else
        return true;
}

wcHTTP2BackgroundTasks::wcHTTP2BackgroundTasks()
{
    mCURLM = new tsCURLM();
    mOnMultiError.self = NULL;
}

wcHTTP2BackgroundTasks::~wcHTTP2BackgroundTasks()
{
    clear();
    if ASSIGNED(mCURLM) {
        delete mCURLM;
        mCURLM = NULL;
    }
}

void wcHTTP2BackgroundTasks::doIdle()
{
    int response_code, still_running, msgq;

    mCURLM->lock();
        if (ready()) {
            response_code = curl_multi_perform(mCURLM->getValue(), &still_running);

            if (response_code == CURLE_OK) {
                CURLMsg * m;
                do {
                    m = curl_multi_info_read(mCURLM->getValue(), &msgq);
                    if ((ASSIGNED(m)) && (m->msg == CURLMSG_DONE))
                         doForAllEx(DoExecuteExSet(wcHTTP2BackgroundTasks_setTaskFinished, this), m);
                } while ( ASSIGNED(m) );
                if (still_running > 0)
                    response_code = curl_multi_poll(mCURLM->getValue(), NULL, 0, 200, NULL);
            }
        } else
          response_code = 0;
    mCURLM->unlock();
    if (response_code != CURLE_OK)
        doMultiError(response_code);

    doForAll(DoExecuteSet(wcHTTP2BackgroundTasks_idleTask, this));
    extractObjectsByCriteria(DoExecuteCheckSet(wcHTTP2BackgroundTasks_isTaskFinished, this),
                             DoExecuteSet(wcHTTP2BackgroundTasks_afterTaskExtract, this), NULL);
}

void wcHTTP2BackgroundTasks::terminate()
{
    doForAll(DoExecuteSet(wcHTTP2BackgroundTasks_terminateTask, this));
}

bool wcHTTP2BackgroundTasks::ready()
{
    return ASSIGNED(mCURLM->getValue());
}
