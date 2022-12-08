#ifndef WCMEMORYSTREAM_H
#define WCMEMORYSTREAM_H

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cinttypes>

class wcCustomMemoryStream
{
    public:
        wcCustomMemoryStream();
        wcCustomMemoryStream(const void * aBuf, size_t aSize);
        virtual ~wcCustomMemoryStream() { if (mBuf) free(mBuf); };
        size_t read(void * dst, size_t len);
        virtual void copyFrom(wcCustomMemoryStream* src, size_t cnt) {};
        void setPosition(size_t p) { mPos = p;}
        void resetPos() { mPos = 0; }
        void seek(long offset, long origin) {
            switch (origin) {
            case 0 :
                mPos = offset;
                break;
            case 1 :
                mPos += offset;
                break;
            case 2 :
                mPos = mSize + offset;
                break;
            default:
                break;
            }
        };
        size_t getSize() { return mSize;}
        size_t getPosition() { return mPos;}
        void* getMemory() { return mBuf; }
        void* getOffsMemory() { return (char*)mBuf + mPos; }

        virtual void setPointer(const void * aBuf, size_t aSize);
        virtual void clearStr();
    protected:
        size_t mSize{0};
        size_t mPos{0};
    private:
        void * mBuf{NULL};
};

class wcMemoryStream : public wcCustomMemoryStream
{
    public:
        wcMemoryStream() : wcCustomMemoryStream() { mCapacity = 0; };
        wcMemoryStream(void * aBuf, size_t aSize) : wcCustomMemoryStream(aBuf, aSize) { mCapacity = aSize; };
        wcMemoryStream(size_t aSize) : wcCustomMemoryStream() {
            wcCustomMemoryStream::setPointer(malloc(aSize), aSize);
            mCapacity = aSize;
        };
        virtual void setPointer(const void * aBuf, size_t aSize) override;
        size_t write(const void * src, size_t len);
        void copyFrom(wcCustomMemoryStream* src, size_t cnt) override;
        virtual void clearStr() override;
        char * getNullTerminated() {
            if (!mNullTerminated) {
                if (mCapacity > getSize())
                    ((char *)getMemory())[getSize()] = 0;
                else {
                    uint8_t terminator = 0;
                    write(&terminator, sizeof(terminator));
                }
                mNullTerminated = true;
            }
            return (char *)getMemory();
        }
    private:
        size_t mCapacity{0};
        bool mNullTerminated{false};
        void setCapacity(size_t cap);
        void * reallocStr(size_t * cap);
};

#endif // WCMEMORYSTREAM_H
