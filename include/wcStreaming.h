#ifndef WCSTREAMING_H_INCLUDED
#define WCSTREAMING_H_INCLUDED

#include <wcHTTP2BackgroundTask.h>

typedef tsSeq<wcCustomMemoryStream> wcMemSeq;

typedef int (*OnGetNextFrameFunc) (void*, void*);
struct OnGetNextFrame { OnGetNextFrameFunc exec; void* self; };
typedef void (*OnHasNextFrameFunc) (void*, void*);
struct OnHasNextFrame { OnHasNextFrameFunc exec; void* self; };

class wcHTTP2BackgroundOutStreamTask : public wcHTTP2BackgroundTask
{
  private:
    int mInc;
    int mDelta;
    std::string mSubProto;
    wcMemSeq * mFrames;
    OnGetNextFrame mOnGetNextFrame {NULL, NULL};
  protected:
    void popNextFrame();
  public:
    virtual uint32_t getTaskClass() override {return WC_OUT_STREAM_TASK;};
    wcHTTP2BackgroundOutStreamTask(wcHTTP2BackgroundTasks * aPool, wcHTTP2SettingsIntf * Settings, bool aIsSilent, void * aData);
    virtual ~wcHTTP2BackgroundOutStreamTask();

    void setSubProto(const std::string & aSubProto);
    void setDelta(int delta);

    virtual size_t read(void * ptr, size_t len, size_t nmemb) override;
    void launchStream(OnGetNextFrame aOnGetNextFrame);
    void pushFrame(wcCustomMemoryStream* aFrame);

    virtual void doIdle() override;
};

enum wcRESTWebCamFrameState
{
    fstWaitingStartOfFrame=0,
    fstWaitingData
};

class wcHTTP2BackgroundInStreamTask : public wcHTTP2BackgroundTask
{
    private:
        wcMemoryStream * mFrameBuffer;
        wcRESTWebCamFrameState mFrameState;
        uint32_t mFrameSize;
        uint32_t mFrameBufferSize;
        int mFrameID;

        wcMemSeq * mFrames;
        OnHasNextFrame mOnHasNextFrame {NULL, NULL};
    protected:
        void pushFrame(size_t aStartAt);
        int tryConsumeFrame(const void * Chunk, int ChunkSz);
    public:
        virtual uint32_t getTaskClass() override {return WC_IN_STREAM_TASK;};
        wcHTTP2BackgroundInStreamTask(wcHTTP2BackgroundTasks * aPool, wcHTTP2SettingsIntf * Settings, bool aIsSilent, void * aData);
        virtual ~wcHTTP2BackgroundInStreamTask();

        virtual size_t write(const void * ptr, size_t len, size_t nmemb) override;
        void launchStream(const std::string & aDevice, OnHasNextFrame aOnHasNextFrame);

        wcCustomMemoryStream * popFrame();

        virtual void close() override;

        virtual void doIdle() override;
};

class wcHTTP2StreamFrame : public tsObject
{
  private:
    wcCustomMemoryStream * mLstFrame;
    wcCustomMemoryStream * mFrame;
    int mFrameID;
  public:
    wcHTTP2StreamFrame();
    virtual ~wcHTTP2StreamFrame();

    wcCustomMemoryStream * extractFrame();
    void nextFrame(wcCustomMemoryStream * aBmp);

    wcCustomMemoryStream * getFrame();
    int getFrameID();
    wcCustomMemoryStream * getLstFrame();
};

#endif // WCSTREAMING_H_INCLUDED
