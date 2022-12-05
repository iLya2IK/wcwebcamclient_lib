/*===============================================================*/
/* Classes to work with IO data streams                          */
/* wcStreaming.cpp                                               */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/


#include <wcStreaming.h>

void wcHTTP2BackgroundOutStreamTask::popNextFrame()
{
    wcCustomMemoryStream * Fr = mFrames->pop_front();
    if ASSIGNED(Fr)
    {
        if ASSIGNED(mRequest) delete mRequest;
        mRequest = Fr;
    }
}

wcHTTP2BackgroundOutStreamTask::wcHTTP2BackgroundOutStreamTask(wcHTTP2BackgroundTasks* aPool,
                                                               wcHTTP2SettingsIntf* Settings,
                                                               const std::string & aSubProto,
                                                               int aDelta,
                                                               bool aIsSilent, void* aData)
: wcHTTP2BackgroundTask(aPool, Settings, aIsSilent, aData)
{
    mFrames = new wcMemSeq();
    mInc = 0;
    mDelta = aDelta;
    mSubProto = aSubProto;
}

wcHTTP2BackgroundOutStreamTask::~wcHTTP2BackgroundOutStreamTask()
{
    delete mFrames;
}

size_t wcHTTP2BackgroundOutStreamTask::read(void* ptr, size_t len, size_t nmemb)
{
    if (finished()) return CURL_READFUNC_ABORT;

    lock();
    size_t res;
    if (ASSIGNED(mRequest) && (mRequest->getSize() > 0))
        res = mRequest->read(ptr, nmemb * len);
    else
        res = 0;

    if (res == 0)
    {
        popNextFrame();
        if (ASSIGNED(mRequest) && (mRequest->getSize() > 0))
            res = mRequest->read(ptr, nmemb * len);
    }
    unlock();

    if (res == 0)
        res = CURL_READFUNC_PAUSE;

    return res;
}

void wcHTTP2BackgroundOutStreamTask::launchStream(OnGetNextFrame aOnGetNextFrame)
{
    mOnGetNextFrame = aOnGetNextFrame;
    if (getPool()->ready())
    {
        mCURL = curl_easy_init();
        if ASSIGNED(mCURL)
        {
            mResponse->resetPos();
            mPath = "/input.raw?" + std::string(JSON_RPC_SHASH) + "=" + mSettings->getEncodedSID();
            if (!mSubProto.empty()) mPath += "&" + std::string(JSON_RPC_SUBPROTO) + "=" + encodeHTTP(mSubProto);
            if (mDelta > 0) mPath += "&" + std::string(JSON_RPC_DELTA) + "=" + std::to_string(mDelta);

            CURLConfig((int64_t)0x500000000);
            CURLSetUpload();
            CURLSetSeekFunction();
            CURLSetReadFunction(-1);

            mRequest = new wcMemoryStream();

            attachToPool();
        } else
            doError(TASK_ERROR_CANT_EASY_CURL);
    }
}

void wcHTTP2BackgroundOutStreamTask::pushFrame(wcCustomMemoryStream* aFrame)
{
    if ASSIGNED(aFrame) mFrames->push_back(aFrame);
}

void wcHTTP2BackgroundOutStreamTask::doIdle()
{
    lock();
    int fr = EXEC_METHOD(mOnGetNextFrame, this);
    unlock();
    if (fr > 0) {
        fr = (int)curl_easy_pause(mCURL, CURLPAUSE_CONT);
        if (fr != CURLE_OK)
            doError(TASK_ERROR_CURL, fr);
    }
}

void wcHTTP2BackgroundInStreamTask::pushFrame(size_t aStartAt)
{
    lock();
    mFrameID++;
    wcMemoryStream * aFrame = new wcMemoryStream();
    mFrameBuffer->seek(aStartAt, 0);
    aFrame->copyFrom(mFrameBuffer, mFrameSize + WEBCAM_FRAME_HEADER_SIZE);
    aFrame->resetPos();
    mFrames->push_back(aFrame);
    unlock();
}

int wcHTTP2BackgroundInStreamTask::tryConsumeFrame(const void* Chunk, int ChunkSz)
{
    int64_t BP;
    auto truncateFrameBuffer = [&]()
    {
        if (BP > 0) {
            if ((mFrameBufferSize - BP) > 0)
            {
                mFrameBufferSize = mFrameBufferSize - BP;
                memmove((char*)mFrameBuffer->getMemory(), (char*)mFrameBuffer->getMemory() + BP, mFrameBufferSize);
            }
            else
              mFrameBufferSize = 0;
            BP = 0;
        }
    };
    auto bufferFreeSize = [&]()
    {
        return WEBCAM_FRAME_BUFFER_SIZE - mFrameBufferSize;
    };
    BP = 0;
    int64_t P;
    uint32_t C;
    uint16_t W;
    lock();
    int ChunkPos = 0;
    bool proceed = true;
    while (proceed)
    {
        if (bufferFreeSize() == 0)
        {
            doError(ERR_WEBCAM_STREAM_BUFFER_OVERFLOW);
            proceed = false;
            break;
        }

        if (ChunkPos < ChunkSz)
        {
            mFrameBuffer->seek(mFrameBufferSize, 0);
            P = ChunkSz - ChunkPos;
            if (P > bufferFreeSize()) P = bufferFreeSize();
            mFrameBuffer->write(((char*)Chunk + ChunkPos), P);
            ChunkPos += P;
            mFrameBufferSize = mFrameBuffer->getPosition();
        }

        mFrameBuffer->seek(BP, 0);
        switch (mFrameState) {
            case fstWaitingStartOfFrame:
            {
                mFrameSize = 0;
                if (((int64_t)mFrameBufferSize - BP) >= (int64_t)WEBCAM_FRAME_HEADER_SIZE)
                {
                    mFrameBuffer->read(&W, sizeof(uint16_t));
                    if (W == WEBCAM_FRAME_START_SEQ)
                    {
                        mFrameBuffer->read(&C, sizeof(uint32_t));
                        if (C > (WEBCAM_FRAME_BUFFER_SIZE - WEBCAM_FRAME_HEADER_SIZE))
                        {
                            doError(ERR_WEBCAM_STREAM_FRAME_TO_BIG);
                            proceed = false;
                        } else {
                            mFrameSize = C;
                            mFrameState = fstWaitingData;
                        }
                    } else {
                        doError(ERR_WEBCAM_STREAM_WRONG_HEADER);
                        proceed = false;
                    }
                } else
                {
                    truncateFrameBuffer();
                    if (ChunkPos == ChunkSz) proceed = false;
                }
                break;
            }
            case fstWaitingData:
            {
                if (((int64_t)mFrameBufferSize - BP) >= (int64_t)(mFrameSize + WEBCAM_FRAME_HEADER_SIZE))
                {
                    pushFrame(BP);
                    BP += mFrameSize + WEBCAM_FRAME_HEADER_SIZE;
                    mFrameState = fstWaitingStartOfFrame;
                } else
                {
                    truncateFrameBuffer();
                    if (ChunkPos == ChunkSz) proceed = false;
                }
                break;
            }
        }
    }
    unlock();
    return ChunkPos;
}

wcHTTP2BackgroundInStreamTask::wcHTTP2BackgroundInStreamTask(wcHTTP2BackgroundTasks* aPool,
                                                             wcHTTP2SettingsIntf* Settings,
                                                             const std::string & aDevice,
                                                             bool aIsSilent, void* aData)
: wcHTTP2BackgroundTask(aPool, Settings, aIsSilent, aData)
{
    delete mResponse;
    mResponse = NULL;

    mDevice = aDevice;

    mFrames = new wcMemSeq();

    mFrameBuffer = new wcMemoryStream(WEBCAM_FRAME_BUFFER_SIZE);
    mFrameState = fstWaitingStartOfFrame;
    mFrameBufferSize = 0;
    mFrameSize = 0;
    mFrameID = 0;
}

wcHTTP2BackgroundInStreamTask::~wcHTTP2BackgroundInStreamTask()
{
    delete mFrameBuffer;
    mFrames->clear();
    delete mFrames;
}

size_t wcHTTP2BackgroundInStreamTask::write(const void* ptr, size_t len, size_t nmemb)
{
    if (finished()) return 0;

    size_t res = tryConsumeFrame(ptr, len * nmemb);

    if ((mFrames->count() > 0) && _ASSIGNED(mOnHasNextFrame))
        EXEC_METHOD(mOnHasNextFrame, this);

    return res;
}

void wcHTTP2BackgroundInStreamTask::launchStream(OnHasNextFrame aOnHasNextFrame)
{
    mOnHasNextFrame = aOnHasNextFrame;
    if (getPool()->ready())
    {
        mCURL = curl_easy_init();
        if ASSIGNED(mCURL)
        {
            mPath = "/output.raw?" + std::string(JSON_RPC_SHASH) + "=" + mSettings->getEncodedSID() +
                                              "&" + JSON_RPC_DEVICE + "=" + encodeHTTP(mDevice);

            CURLConfig((int64_t)0);

            curl_easy_setopt(mCURL, CURLOPT_TIMEOUT, (long)(-1));

            attachToPool();
        } else
            doError(TASK_ERROR_CANT_EASY_CURL);
    }
}

wcCustomMemoryStream* wcHTTP2BackgroundInStreamTask::popFrame()
{
    return mFrames->pop_front();
}

void wcHTTP2BackgroundInStreamTask::close()
{
    setData(NULL);
    wcHTTP2BackgroundTask::close();
}

void wcHTTP2BackgroundInStreamTask::doIdle()
{
    if ((mFrames->count() > 0) && _ASSIGNED(mOnHasNextFrame))
        EXEC_METHOD(mOnHasNextFrame, this);
}

wcHTTP2StreamFrame::wcHTTP2StreamFrame()
{
    mFrame = NULL;
    mLstFrame = NULL;
    mFrameID = 0;
}

wcHTTP2StreamFrame::~wcHTTP2StreamFrame()
{
    if ASSIGNED(mFrame) delete mFrame;
    if ASSIGNED(mLstFrame) delete mLstFrame;
}

wcCustomMemoryStream* wcHTTP2StreamFrame::extractFrame()
{
    lock();
    wcCustomMemoryStream * res = mFrame;
    mFrame = NULL;
    unlock();
    return res;
}

void wcHTTP2StreamFrame::nextFrame(wcCustomMemoryStream* aBmp)
{
    lock();
    mFrameID++;
    if ASSIGNED(mLstFrame) delete mLstFrame;
    mLstFrame = new wcMemoryStream();
    mLstFrame->copyFrom(aBmp, aBmp->getSize());
    aBmp->resetPos();
    mFrame = aBmp;
    unlock();
}

wcCustomMemoryStream* wcHTTP2StreamFrame::getFrame()
{
    lock();
    wcCustomMemoryStream* res = mLstFrame;
    unlock();
    return res;
}

int wcHTTP2StreamFrame::getFrameID()
{
    lock();
    int res = mFrameID;
    unlock();
    return res;
}

wcCustomMemoryStream* wcHTTP2StreamFrame::getLstFrame()
{
    lock();
    wcCustomMemoryStream* res = mLstFrame;
    unlock();
    return res;
}
