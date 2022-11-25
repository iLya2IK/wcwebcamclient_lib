#ifndef WCTHREAD_H
#define WCTHREAD_H

#include <iostream>
#include <pthread.h>
#include <cassert>

class wcThread {
public:
	wcThread(bool isDetached = false);
	virtual ~wcThread();
	void start();
	virtual void execute() {};
	void* join();
	void setFreeOnTerminate(bool doFree);
	bool terminated() {return mTerminated;}
	void terminate() {mTerminated = true;}
private:
	// thread ID
	pthread_t PthreadThreadID;
	// true if thread created in detached state
	bool detached;
	bool mFreeOnTerminate;
	bool mTerminated;
	pthread_attr_t threadAttribute;
	wcThread(const wcThread&);
	const wcThread& operator=(const wcThread&);
	// called when run() completes
	void setCompleted();
	// stores return value from run()
	void* result;
	static void* startThread(void* pVoid);
	void printError(char * msg, int status, char* fileName, int lineNumber);
};

#endif // WCTHREAD_H
