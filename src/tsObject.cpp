/*===============================================================*/
/* Threadsafe wrapper (abstract level)                           */
/* tsObject.cpp                                                  */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "tsObject.h"

tsObject::tsObject()
{
    pthread_mutexattr_t MAttr;
    int res = pthread_mutexattr_init(&MAttr);
    if (res==0)
    {
       res=pthread_mutexattr_settype(&MAttr, PTHREAD_MUTEX_RECURSIVE);
       if (res==0)
         pthread_mutex_init(&m_mutex, &MAttr);
    }
    else
       pthread_mutex_init(&m_mutex, NULL);
    pthread_mutexattr_destroy(&MAttr);
}

tsObject::~tsObject()
{
    while (pthread_mutex_unlock(&m_mutex) == 0) {};
    pthread_mutex_destroy(&m_mutex);
}

void tsObject::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

void tsObject::lock()
{
    pthread_mutex_lock(&m_mutex);
}
