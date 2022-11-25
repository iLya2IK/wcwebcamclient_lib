/*===============================================================*/
/* Threadsafe wrapper for CURLM object                           */
/* tsCURLM.cpp                                                   */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "tsCURLM.h"

tsCURLM::~tsCURLM() { doneCURLM(); }

void tsCURLM::initCURLM() {
    lock();
    mValue = curl_multi_init();
    if (mValue != NULL)
      curl_multi_setopt(mValue, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
    unlock();
}

void tsCURLM::doneCURLM() {
    lock();
    if (mValue != NULL)
      curl_multi_cleanup(mValue);
    mValue = NULL;
    unlock();
}

CURLM * tsCURLM::getValue() {
    CURLM * res;
    lock();
    res = mValue;
    unlock();
    return res;
}

CURLM * tsCURLM::getValueUnsafe() {
    return mValue;
}
