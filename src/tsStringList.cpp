/*===============================================================*/
/* Threadsafe wrapper for list of strings                        */
/* tsStringList.cpp                                              */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "tsStringList.h"

void tsStringList::add(std::string avalue)
{
    lock();
    vtr.push_back(avalue);
    unlock();
}

std::string tsStringList::pop() {
    std::string s;
    lock();
    if (vtr.size() > 0) {
        s =  vtr.at(vtr.size()-1);
        vtr.pop_back();
    } else s = std::string("");
    unlock();
    return s;
}

int tsStringList::size()
{
    lock();
    int sz = vtr.size();
    unlock();
    return sz;
}

void tsStringList::clear()
{
    lock();
    vtr.clear();
    unlock();
}

std::string tsStringList::getAt(int i) {
    std::string s;
    lock();
    if ((vtr.size() > (size_t)i ) && (i >= 0)) {
        s = vtr.at(i);
    } else s = std::string("");
    unlock();
    return s;
}
