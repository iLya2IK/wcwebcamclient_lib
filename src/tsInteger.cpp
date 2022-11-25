/*===============================================================*/
/* Threadsafe wrapper for integer value                          */
/* tsInteger.cpp                                                 */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "tsInteger.h"

tsInteger::tsInteger(long value)
{
    mValue = value;
}

long tsInteger::getValue()
{
    lock();
    long v = mValue;
    unlock();
    return v;
}

void tsInteger::setValue(long value)
{
    lock();
    mValue = value;
    unlock();
}
