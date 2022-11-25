#ifndef WCCALLBACKS_H_INCLUDED
#define WCCALLBACKS_H_INCLUDED

#include <wcDefines.h>
#include <wcCommonCallbacks.h>
#include <wcMemoryStream.h>
#include <NotifyObj.h>
#include <wcJSON.h>
#include <cJSON.h>

typedef void (*cJSONNotifyEventFunc) (wcCallbackClient, wcCallbackTask, const cJSON *);
struct cJSONNotifyEvent { cJSONNotifyEventFunc exec; wcCallbackClient self; };
typedef void (*StringNotifyFunc) (wcCallbackClient self, const std::string&);
struct StringNotify { StringNotifyFunc exec; wcCallbackClient self; };
typedef void (*JSONNotifyEventFunc) (wcCallbackClient, wcCallbackTask, wcJSON&);
struct JSONNotifyEvent { JSONNotifyEventFunc exec; wcCallbackClient self; };

static const cJSONNotifyEvent NULL_cJSONNotifyEvent {NULL, NULL};
static const StringNotify NULL_StringNotify {NULL, NULL};
static const JSONNotifyEvent NULL_JSONNotifyEvent {NULL, NULL};

#define JSON_EXEC_METHOD(meth, tsk, obj) (*(meth.exec))(meth.self, tsk, obj)
#define JSON_EXEC_STR_METHOD(meth, tsk, obj) jsonToStrExec(meth, tsk, obj)

EmptyNotify EmptyNotifySet(const EmptyNotifyFunc exec, NotifyObj* self);
NotifyEvent NotifyEventSet(const NotifyEventFunc exec, NotifyObj* self);
TaskNotify TaskNotifySet(const TaskNotifyFunc exec, NotifyObj* self);
ConnNotifyEvent ConnNotifyEventSet(const ConnNotifyEventFunc exec, NotifyObj* self);
CStringNotify CStringNotifySet(const CStringNotifyFunc exec, NotifyObj* self);
StringNotify StringNotifySet(const StringNotifyFunc exec, NotifyObj* self);
DataNotifyEvent DataNotifyEventSet(const DataNotifyEventFunc exec, NotifyObj* self);
DataAltNotifyEvent DataAltNotifyEventSet(const DataAltNotifyEventFunc exec, NotifyObj* self);
DoExecute DoExecuteSet(const DoExecuteFunc exec, NotifyObj* self);
DoExecuteEx DoExecuteExSet(const DoExecuteExFunc exec, NotifyObj* self);
DoExecuteCheck DoExecuteCheckSet(const DoExecuteCheckFunc exec, NotifyObj* self);

cJSONNotifyEvent cJSONNotifyEventSet(const cJSONNotifyEventFunc exec, NotifyObj* self);
JSONNotifyEvent JSONNotifyEventSet(const JSONNotifyEventFunc exec, NotifyObj* self);
JSONStrNotifyEvent JSONStrNotifyEventSet(const JSONStrNotifyEventFunc exec, NotifyObj* self);

void jsonToStrExec(JSONStrNotifyEvent meth, void * tsk, wcJSON & jObj);

#endif // WCCALLBACKS_H_INCLUDED
