#ifndef WCCOMMONCALLBACKS_H_INCLUDED
#define WCCOMMONCALLBACKS_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

typedef void * wcCallbackClient;
typedef void * wcCallbackTask;

typedef void (*EmptyNotifyFunc) (wcCallbackClient self);
typedef struct EmptyNotify {  EmptyNotifyFunc exec; wcCallbackClient self; } EmptyNotify;
typedef void (*NotifyEventFunc) (wcCallbackClient self, void *);
typedef struct NotifyEvent {  NotifyEventFunc exec; wcCallbackClient self; } NotifyEvent;
typedef void (*TaskNotifyFunc) (wcCallbackClient, wcCallbackTask);
typedef struct TaskNotify {  TaskNotifyFunc exec; wcCallbackClient self; } TaskNotify;
typedef void (*ConnNotifyEventFunc) (wcCallbackClient, int);
typedef struct ConnNotifyEvent { ConnNotifyEventFunc exec; wcCallbackClient self; } ConnNotifyEvent;
typedef void (*CStringNotifyFunc) (wcCallbackClient self, const char *);
typedef struct CStringNotify { CStringNotifyFunc exec; wcCallbackClient self; } CStringNotify;
typedef void (*DataNotifyEventFunc) (wcCallbackClient, wcCallbackTask, void*);
typedef struct DataNotifyEvent { DataNotifyEventFunc exec; wcCallbackClient self; } DataNotifyEvent;
typedef void (*DataAltNotifyEventFunc) (wcCallbackClient, wcCallbackTask, void*, size_t);
typedef struct DataAltNotifyEvent { DataAltNotifyEventFunc exec; wcCallbackClient self; } DataAltNotifyEvent;
typedef void (*DoExecuteFunc) (void*, void*);
typedef struct DoExecute { DoExecuteFunc exec; void* self; } DoExecute;
typedef void (*DoExecuteExFunc) (void*, void*, const void*);
typedef struct DoExecuteEx { DoExecuteExFunc exec; void* self; } DoExecuteEx;
typedef int (*DoExecuteCheckFunc) (void*, void*, const void*);
typedef struct DoExecuteCheck { DoExecuteCheckFunc exec; void* self; } DoExecuteCheck;
typedef void (*JSONStrNotifyEventFunc) (wcCallbackClient, wcCallbackTask, const char *);
typedef struct JSONStrNotifyEvent { JSONStrNotifyEventFunc exec; wcCallbackClient self; } JSONStrNotifyEvent;

#define EXEC_METHOD(meth, ...)  (*(meth.exec))(meth.self, ##__VA_ARGS__)
#define _ASSIGNED(meth) (meth.self != NULL)

static const EmptyNotify NULL_EmptyNotify = {NULL, NULL};
static const NotifyEvent NULL_NotifyEvent = {NULL, NULL};
static const TaskNotify NULL_TaskNotify = {NULL, NULL};
static const ConnNotifyEvent NULL_ConnNotifyEvent = {NULL, NULL};
static const CStringNotify NULL_CStringNotify = {NULL, NULL};
static const DataNotifyEvent NULL_DataNotifyEvent = {NULL, NULL};
static const DataAltNotifyEvent NULL_DataAltNotifyEvent = {NULL, NULL};
static const DoExecute NULL_DoExecute = {NULL, NULL};
static const DoExecuteEx NULL_DoExecuteEx = {NULL, NULL};
static const DoExecuteCheck NULL_DoExecuteCheck = {NULL, NULL};
static const JSONStrNotifyEvent NULL_JSONStrNotifyEvent = {NULL, NULL};

#endif // WCCOMMONCALLBACKS_H_INCLUDED
