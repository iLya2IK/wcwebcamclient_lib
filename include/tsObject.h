#ifndef TSOBJECT_H
#define TSOBJECT_H

#include "pthread.h"
#include "NotifyObj.h"

class tsObject : public NotifyObj
{
    public:
        tsObject();
        virtual ~tsObject();

        void lock();
        void unlock();

    private:
        pthread_mutex_t m_mutex;
};

#endif // TSOBJECT_H
