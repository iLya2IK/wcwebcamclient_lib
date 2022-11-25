#ifndef WCCOMMONCALLBACKS_H_INCLUDED
#define WCCOMMONCALLBACKS_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

typedef void * wcCallbackClient;
typedef void * wcCallbackTask;

typedef void (*EmptyNotifyFunc) (wcCallbackClient self);
struct EmptyNotify {  EmptyNotifyFunc exec; wcCallbackClient self; };
typedef void (*NotifyEventFunc) (wcCallbackClient self, void *);
struct NotifyEvent {  NotifyEventFunc exec; wcCallbackClient self; };
typedef void (*TaskNotifyFunc) (wcCallbackClient, wcCallbackTask);
struct TaskNotify {  TaskNotifyFunc exec; wcCallbackClient self; };
typedef void (*ConnNotifyEventFunc) (wcCallbackClient, bool);
struct ConnNotifyEvent { ConnNotifyEventFunc exec; wcCallbackClient self; };
typedef void (*CStringNotifyFunc) (wcCallbackClient self, const char *);
struct CStringNotify { CStringNotifyFunc exec; wcCallbackClient self; };
typedef void (*DataNotifyEventFunc) (wcCallbackClient, wcCallbackTask, void*);
struct DataNotifyEvent { DataNotifyEventFunc exec; wcCallbackClient self; };
typedef void (*DataAltNotifyEventFunc) (wcCallbackClient, wcCallbackTask, void*, size_t);
struct DataAltNotifyEvent { DataAltNotifyEventFunc exec; wcCallbackClient self; };
typedef void (*DoExecuteFunc) (void*, void*);
struct DoExecute { DoExecuteFunc exec; void* self; };
typedef void (*DoExecuteExFunc) (void*, void*, const void*);
struct DoExecuteEx { DoExecuteExFunc exec; void* self; };
typedef bool (*DoExecuteCheckFunc) (void*, void*, const void*);
struct DoExecuteCheck { DoExecuteCheckFunc exec; void* self; };
typedef void (*JSONStrNotifyEventFunc) (wcCallbackClient, wcCallbackTask, const char *);
struct JSONStrNotifyEvent { JSONStrNotifyEventFunc exec; wcCallbackClient self; };

#define EXEC_METHOD(meth, ...)  (*(meth.exec))(meth.self, ##__VA_ARGS__)
#define _ASSIGNED(meth) (meth.self != NULL)

static const EmptyNotify NULL_EmptyNotify {NULL, NULL};
static const NotifyEvent NULL_NotifyEvent {NULL, NULL};
static const TaskNotify NULL_TaskNotify {NULL, NULL};
static const ConnNotifyEvent NULL_ConnNotifyEvent {NULL, NULL};
static const CStringNotify NULL_CStringNotify {NULL, NULL};
static const DataNotifyEvent NULL_DataNotifyEvent {NULL, NULL};
static const DataAltNotifyEvent NULL_DataAltNotifyEvent {NULL, NULL};
static const DoExecute NULL_DoExecute {NULL, NULL};
static const DoExecuteEx NULL_DoExecuteEx {NULL, NULL};
static const DoExecuteCheck NULL_DoExecuteCheck {NULL, NULL};
static const JSONStrNotifyEvent NULL_JSONStrNotifyEvent {NULL, NULL};

#endif // WCCOMMONCALLBACKS_H_INCLUDED
