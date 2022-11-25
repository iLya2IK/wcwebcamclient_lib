/*===============================================================*/
/* C++ class to work with threads                                */
/* wcThread.cpp                                                  */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "wcThread.h"

wcThread::wcThread(bool isDetached) : detached(isDetached) {
    mFreeOnTerminate = false;
    mTerminated = false;
}

void wcThread::setFreeOnTerminate(bool doFree) {
    mFreeOnTerminate = doFree;
}

void* wcThread::startThread(void* pVoid) {
	// thread start function when a Runnable is involved
	wcThread* runnableThread = static_cast<wcThread*>(pVoid);
	assert(runnableThread);
	runnableThread->execute();
	void* res = runnableThread->result;
	runnableThread->setCompleted();
	return res;
}

wcThread::~wcThread() {}

void wcThread::start() {
	// initialize attribute object
	int status = pthread_attr_init(&threadAttribute);
	if(status) {
		exit(status);
	}

	// set the scheduling scope attribute
	status = pthread_attr_setscope(&threadAttribute,
					PTHREAD_SCOPE_SYSTEM);
	if(status) {
		exit(status);
	}

	if(detached) {
		// set the detachstate attribute to detached
		status = pthread_attr_setdetachstate(&threadAttribute,
						PTHREAD_CREATE_DETACHED);
		if(status) {
			exit(status);
		}
	}
	status = pthread_create(&PthreadThreadID, &threadAttribute,
		wcThread::startThread, (void*)this);
	if(status) {
		exit(status);
	}
	status = pthread_attr_destroy(&threadAttribute);
	if(status) {
		exit(status);
	}
}


void* wcThread::join() {
	// A thread calling T.join() waits until thread T completes.
	int status = pthread_join(PthreadThreadID, NULL);
	// result was already saved by thread start function
	if(status) {
		exit(status);
	}
	return result;
}

void wcThread::setCompleted() {
    terminate();
    if (mFreeOnTerminate) {
        delete this;
    }
}

void wcThread::printError(char * msg, int status, char* fileName, int lineNumber) {
	/*cout << msg << " " << fileName << ":" << lineNumber <<
		"-" << strerror(status) << endl;*/
}
