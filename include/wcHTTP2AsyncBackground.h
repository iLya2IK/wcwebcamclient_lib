#ifndef WCHTTP2ASYNCBACKGROUND_H
#define WCHTTP2ASYNCBACKGROUND_H

#include <wcThread.h>
#include <wcHTTP2BackgroundTask.h>

class wcHTTP2AsyncBackground : public wcThread
{
    public:
        wcHTTP2AsyncBackground();
        virtual ~wcHTTP2AsyncBackground();
        virtual void execute() override;
        void addTask(wcHTTP2BackgroundTask * aTask);
        bool ready();
        wcHTTP2BackgroundTasks * getTasks();
    protected:

    private:
        wcHTTP2BackgroundTasks * mTasks;
};

#endif // WCHTTP2ASYNCBACKGROUND_H
