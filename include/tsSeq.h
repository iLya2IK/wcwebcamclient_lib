#pragma once
#ifndef TSSEQ_H
#define TSSEQ_H

#include <tsObject.h>
#include <wcCommonCallbacks.h>
#include <deque>

template <class T>
class tsSeq : public tsObject
{
    public:
        virtual ~tsSeq();

        void extractAll();

        void doForAllEx(DoExecuteEx what, void * data);
        void doForAll(DoExecute what);
        void extractObjectsByCriteria(DoExecuteCheck criteria, DoExecute afterextract, void * data);
        T* pop_front();
        void push_back(T* v);
        void clear();
        size_t count();
    private:
        std::deque<T*> mList;
};


template <class T>
tsSeq<T>::~tsSeq<T>() {
    clear();
}

template <class T>
void tsSeq<T>::extractAll() {
    lock();
    mList.clear();
    unlock();
}

template <class T>
void tsSeq<T>::doForAllEx(DoExecuteEx what, void * data) {
    lock();
    for (auto i = mList.begin(); i != mList.end() ; ) {
        EXEC_METHOD(what, *i, data);
        ++i;
    }
    unlock();
}

template <class T>
void tsSeq<T>::doForAll(DoExecute what) {
    lock();
    for (auto i = mList.begin(); i != mList.end() ; ) {
        EXEC_METHOD(what, *i);
        ++i;
    }
    unlock();
}

template <class T>
void tsSeq<T>::extractObjectsByCriteria(DoExecuteCheck criteria, DoExecute afterextract, void * data) {
    lock();
    for (auto i = mList.begin(); i != mList.end() ; ) {
      if (EXEC_METHOD(criteria, *i, data)) {
            EXEC_METHOD(afterextract, *i);
            i = mList.erase(i);
      }
      else
        ++i;
    }
    unlock();
}

template <class T>
T* tsSeq<T>::pop_front() {
    lock();
    T* res;
    if (!mList.empty()) {
        res = mList.front();
        mList.pop_front();
    }
    else res = NULL;
    unlock();
    return res;
}

template <class T>
void tsSeq<T>::push_back(T* v) {
    lock();
    mList.push_back(v);
    unlock();
}

template <class T>
void tsSeq<T>::clear() {
    lock();
    while (!mList.empty())
    {
        T * res = mList.front();
        delete res;
        mList.pop_front();
    }
    unlock();
}

template <class T>
size_t tsSeq<T>::count() {
    lock();
    size_t res = mList.size();
    unlock();
    return res;
}

#endif // TSSEQ_H
