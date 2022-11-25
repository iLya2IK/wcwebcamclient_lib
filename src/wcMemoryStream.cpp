/*===============================================================*/
/* Free-pascal like class to work with memory streams            */
/* wcMemoryStream.cpp                                            */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "wcMemoryStream.h"

using namespace std;

wcCustomMemoryStream::wcCustomMemoryStream()
{
    mBuf = NULL;
    mPos = 0;
    mSize = 0;
}

wcCustomMemoryStream::wcCustomMemoryStream(const void * aBuf, size_t aSize)
{
    setPointer(aBuf, aSize);
}

void wcCustomMemoryStream::setPointer(const void * aBuf, size_t aSize)
{
    mPos = 0;
    mBuf = (void *)aBuf;
    mSize = aSize;
}

void wcCustomMemoryStream::clearStr() {
    mSize = 0;
    mPos = 0;
}

size_t wcCustomMemoryStream::read(void * dst, size_t len)
{
    size_t res = 0;
    if ((len > 0) && (mPos < mSize)) {
        res = len;
        if (res > (mSize - mPos))
          res = mSize - mPos;
        memcpy(dst, (void*)((char*)mBuf + mPos), res);
        mPos += res;
    }
    return res;
}

void * wcMemoryStream::reallocStr(size_t * cap)
{
    size_t GC = mCapacity + (mCapacity / 4);
    if ((*cap > mCapacity) && (*cap < GC))
        *cap = GC;

    *cap = (*cap + (size_t)0xFFF) & ~((size_t)0xFFF);
    void* res;
    if (*cap == mCapacity)
      res = getMemory();
    else
      res = realloc(getMemory(), *cap);

    return res;
}

void wcMemoryStream::clearStr()
{
    wcCustomMemoryStream::clearStr();
    setCapacity(0);
}

void wcMemoryStream::setCapacity(size_t cap)
{
    wcCustomMemoryStream::setPointer(reallocStr(&cap), mSize);
    mCapacity = cap;
}

size_t wcMemoryStream::write(const void * src, size_t len)
{
    if (len == 0) { return 0; }
    size_t NewPos = mPos + len;
    if (NewPos > mSize) {
       if (NewPos > mCapacity)
         setCapacity(NewPos);
       mSize = NewPos;
    }
    memcpy((void*)((char*)getMemory() + mPos), src, len);
    mPos = NewPos;
    return len;
}

void wcMemoryStream::setPointer(const void * aBuf, size_t aSize)
{
    if (getMemory() != NULL) free(getMemory());
    wcCustomMemoryStream::setPointer(aBuf, aSize);
    mCapacity = aSize;
}

void wcMemoryStream::copyFrom(wcCustomMemoryStream* src, size_t cnt)
{
    size_t wc = write(src->getOffsMemory(), cnt);
    src->seek(wc, 1);
}

wcMemoryStream::~wcMemoryStream()
{
    clearStr();
}
