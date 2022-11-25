/*===============================================================*/
/* Additional callbacks for C++ applications                     */
/* wcCallbacks.cpp                                               */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include <wcCallbacks.h>

void jsonToStrExec(JSONStrNotifyEvent meth, void * tsk, wcJSON & jObj) {
    if (jObj.isValid()) {
        (*(meth.exec))(meth.self, tsk, jObj.toString().c_str());
    } else {
        (*(meth.exec))(meth.self, tsk, NULL);
    }
}

cJSONNotifyEvent cJSONNotifyEventSet(const cJSONNotifyEventFunc exec, NotifyObj* self)
{
    return {exec, self};
}

JSONNotifyEvent JSONNotifyEventSet(const JSONNotifyEventFunc exec, NotifyObj* self)
{
    return {exec, self};
}

JSONStrNotifyEvent JSONStrNotifyEventSet(const JSONStrNotifyEventFunc exec, NotifyObj* self)
{
    return {exec, self};
}

EmptyNotify EmptyNotifySet(const EmptyNotifyFunc exec, NotifyObj* self) {
    return {exec, self};
}

NotifyEvent NotifyEventSet(const NotifyEventFunc exec, NotifyObj* self) {
    return {exec, self};
}

TaskNotify TaskNotifySet(const TaskNotifyFunc exec, NotifyObj* self) {
    return {exec, self};
}

ConnNotifyEvent ConnNotifyEventSet(const ConnNotifyEventFunc exec, NotifyObj* self) {
    return {exec, self};
}

StringNotify StringNotifySet(const StringNotifyFunc exec, NotifyObj* self) {
    return {exec, self};
}

CStringNotify CStringNotifySet(const CStringNotifyFunc exec, NotifyObj* self) {
    return {exec, self};
}

DataNotifyEvent DataNotifyEventSet(const DataNotifyEventFunc exec, NotifyObj* self) {
    return {exec, self};
}

DataAltNotifyEvent DataAltNotifyEventSet(const DataAltNotifyEventFunc exec, NotifyObj* self) {
    return {exec, self};
}

DoExecute DoExecuteSet(const DoExecuteFunc exec, NotifyObj* self) {
    return {exec, self};
}

DoExecuteEx DoExecuteExSet(const DoExecuteExFunc exec, NotifyObj* self) {
    return {exec, self};
}

DoExecuteCheck DoExecuteCheckSet(const DoExecuteCheckFunc exec, NotifyObj* self) {
    return {exec, self};
}
