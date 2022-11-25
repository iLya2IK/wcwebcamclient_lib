/*===============================================================*/
/* Class to work with background tasks (abstract level)          */
/* wcHTTP2AsyncBackground.cpp                                    */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "wcHTTP2AsyncBackground.h"

wcHTTP2AsyncBackground::wcHTTP2AsyncBackground()
{
    //setFreeOnTerminate(true);
    mTasks = new wcHTTP2BackgroundTasks();
}

wcHTTP2AsyncBackground::~wcHTTP2AsyncBackground()
{
    delete mTasks;
}

void wcHTTP2AsyncBackground::addTask(wcHTTP2BackgroundTask* aTask)
{
    mTasks->push_back(aTask);
}

bool wcHTTP2AsyncBackground::ready()
{
    return mTasks->ready();
}

wcHTTP2BackgroundTasks* wcHTTP2AsyncBackground::getTasks()
{
    return mTasks;
}

void wcHTTP2AsyncBackground::execute()
{
  while (!terminated()) {
      mTasks->doIdle();
      struct timespec tw = {0,100000000};
      struct timespec tr;
      nanosleep (&tw, &tr);
  }
  mTasks->terminate();
}
