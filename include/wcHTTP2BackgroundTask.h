#ifndef WCHTTP2BACKGROUNDTASK_H
#define WCHTTP2BACKGROUNDTASK_H

#include <wcConstsTypes.h>
#include <wcCallbacks.h>
#include <tsSeq.h>
#include <wcHTTP2SettingsIntf.h>

class wcHTTP2AbsBackgroundTask : public tsObject {public: virtual uint32_t getTaskClass() {return WC_BASE_TASK;}};

typedef tsSeq<wcHTTP2AbsBackgroundTask> wcHTTP2BackgroundTasksProto;

class wcHTTP2BackgroundTasks : public wcHTTP2BackgroundTasksProto
{
    private:
        tsCURLM * mCURLM;
        EmptyNotify mOnMultiError;

        void doMultiError(long code);
    public:
        wcHTTP2BackgroundTasks();
        virtual ~wcHTTP2BackgroundTasks();
        tsCURLM * getCURLM() { return mCURLM; };
        void lockCURLM() { mCURLM->lock(); };
        void unlockCURLM()  { mCURLM->unlock(); };
        bool doInitMultiPipeling();
        void doIdle();
        void terminate();
        void closeStreaming();

        bool ready();

        void setOnMultiError(EmptyNotify aOnMultiError) { mOnMultiError = aOnMultiError; };
};

class wcHTTP2BackgroundTask : public wcHTTP2AbsBackgroundTask
{
    public:
        virtual uint32_t getTaskClass() override {return WC_TASK;};

        wcHTTP2BackgroundTask(wcHTTP2BackgroundTasks * aPool, wcHTTP2SettingsIntf * Settings, bool aIsSilent, void * aData);
        virtual ~wcHTTP2BackgroundTask();

        bool doPost(const std::string & aPath, const void * aContent, size_t aContentSize, bool inStack);
        virtual long seek(curl_off_t offset, long origin);
        virtual size_t write(const void * ptr, size_t len, size_t nmemb);
        virtual size_t read(void * ptr, size_t len, size_t nmemb);

        virtual void doIdle() {};
        void terminate();
        virtual void close();

        bool finished() { lock(); bool res = mState >= STATE_FINISHED; unlock(); return res;};
        bool hasOnFinish() { return (mOnFinish.self != NULL); };
        void doFinish() { EXEC_METHOD(mOnFinish, this); };

        wcHTTP2BackgroundTasks * getPool() { return mPool; };

        uint8_t getState() { lock(); uint8_t res = mState; unlock(); return res; };
        long getErrorCode() { lock(); long res = mErrorCode; unlock(); return res; };
        long getErrorSubCode() { lock(); long res = mErrorSubCode; unlock(); return res; };
        long getResponseCode() { lock(); long res = mResponseCode; unlock(); return res; };
        std::string getErrorString();
        std::string getPath() { lock(); std::string res = mPath; unlock(); return res; };
        CURL* getEasyCURL()  { lock(); CURL* res = mCURL; unlock(); return res; };

        void setData(void * aData) { lock(); mData = aData; unlock(); }
        void * getData() { lock(); void * aData = mData; unlock(); return aData; }
        void * getDataUnsafe() { return mData; }

        bool getIsSilent() { lock(); bool res = mIsSilent; unlock(); return res; };

        void setResponseCode(long avalue) { lock(); mResponseCode = avalue; unlock(); };
        void setState(uint8_t avalue) { lock(); mState = avalue; unlock(); };
        void doError(long err) { doError(err, 0); };
        void doError(long err, long subcode) { lock(); mErrorCode = err; mErrorSubCode = subcode; unlock(); };

        void setOnSuccess(TaskNotify aOnSuccess) { mOnSuccess = aOnSuccess; };
        void setOnFinish(TaskNotify aOnFinish) { mOnFinish = aOnFinish; };

        TaskNotify getOnSuccess() { return mOnSuccess; };
        TaskNotify getOnFinish() { return mOnFinish; };

        wcMemoryStream * getResponse() { return mResponse; };
    protected:
        void finalize();
        void attachToPool();

        void CURLConfig(int64_t mContentSize);
        void CURLSetPost();
        void CURLSetUpload();
        void CURLSetSeekFunction();
        void CURLSetReadFunction(int64_t sz);

        wcHTTP2SettingsIntf * mSettings {NULL};
        wcHTTP2BackgroundTasks * mPool {NULL};

        std::string mPath;
        CURL * mCURL {NULL};
        curl_slist * mHeaders {NULL};

        wcMemoryStream * mResponse {NULL};
        wcCustomMemoryStream * mRequest {NULL};

        TErrorBuffer mErrorBuffer;
    private:
        TaskNotify mOnSuccess{NULL_TaskNotify};
        TaskNotify mOnFinish{NULL_TaskNotify};

        uint8_t mState {STATE_INIT};
        long mResponseCode {0};
        long mErrorCode {0}, mErrorSubCode {0};
        bool mIsSilent {false};
        void * mData {NULL};
};

extern size_t WriteFunctionCallback(void * ptr, size_t len, size_t nmemb, void * data);
extern size_t SeekFunctionCallback(void * ptr, curl_off_t offset, long origin);
extern size_t ReadFunctionCallback(void * ptr, size_t len, size_t nmemb, void * data);

#endif // WCHTTP2BACKGROUNDTASK_H
