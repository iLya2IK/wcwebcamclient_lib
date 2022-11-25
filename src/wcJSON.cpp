/*===============================================================*/
/* C++ like wrapper for cJSON objects                            */
/* wcJSON.cpp                                                    */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include <wcJSON.h>

wcJSON json_parse(const char * aStr) { return wcJSON(aStr); }
wcJSON jsonNewObject() { return wcJSON(jsonObject, NULL); }
wcJSON jsonNewArray() { return wcJSON(jsonArray, NULL); }

cJSON * wcJSON::intern_parse(const char * value) {
    cJSON * Result = NULL;

    wcJSON::locker.lock();

    Result = cJSON_Parse(value);
    if (!ASSIGNED(Result)) {
        const char * err = cJSON_GetErrorPtr();
        if ASSIGNED(err)
            mErrorStr = str_format("JSON exception. Error parsing expression at \'%.128s\'", err);
        else
            mErrorStr = std::string(C_UNKNOWN_ERROR);
    }

    wcJSON::locker.unlock();

    return Result;
}

char * wcJSON::valueString() {
    if ASSIGNED(mJSON) {
        return mJSON->valuestring;
    } else
        return NULL;
}

int wcJSON::valueInt() {
    if ASSIGNED(mJSON) {
        return mJSON->valueint;
    } else
        return 0;
}

double wcJSON::valueDouble() {
    if ASSIGNED(mJSON) {
        return mJSON->valuedouble;
    } else
        return 0.0;
}


wcJSON::wcJSON(wcJSON_type atype, void * avalue)
{
    switch (atype) {
    case jsonArray:
        mJSON = cJSON_CreateArray();
        break;
    case jsonObject:
        mJSON = cJSON_CreateObject();
        break;
    case jsonString:
        mJSON = cJSON_CreateString((char *) avalue);
        break;
    case jsonBoolean:
        mJSON = cJSON_CreateBool(*((bool *) avalue));
        break;
    case jsonNumber:
        mJSON = cJSON_CreateNumber(*((double *) avalue));
        break;
    case jsonNull:
        mJSON = cJSON_CreateNull();
        break;
    case jsonRaw:
        mJSON = cJSON_CreateRaw((char *) avalue);
        break;
    default:
        mJSON = NULL;
    }
}

wcJSON wcJSON::addBoolean(const char* aname, bool avalue)
{
    return wcJSON(add_cBoolean(aname, avalue), false);
}

wcJSON wcJSON::addInt(const char* aname, int avalue)
{
    return wcJSON(add_cInt(aname, avalue), false);
}

wcJSON wcJSON::addDouble(const char* aname, double avalue)
{
    return wcJSON(add_cDouble(aname, avalue), false);
}

wcJSON wcJSON::addString(const char* aname, const char* avalue)
{
    return wcJSON(add_cString(aname, avalue), false);
}

wcJSON wcJSON::addObject(const char* aname)
{
    return wcJSON(add_cObject(aname), false);
}

wcJSON wcJSON::addArray(const char* aname)
{
    return wcJSON(add_cArray(aname), false);
}

cJSON * wcJSON::add_cBoolean(const char * aname, bool avalue)
{
    return cJSON_AddBoolToObject(mJSON, aname, avalue);
}

cJSON* wcJSON::add_cInt(const char* aname, int avalue)
{
    return cJSON_AddNumberToObject(mJSON, aname, (double)avalue);
}

cJSON* wcJSON::add_cDouble(const char* aname, double avalue)
{
    return cJSON_AddNumberToObject(mJSON, aname, avalue);
}

cJSON* wcJSON::add_cString(const char* aname, const char* avalue)
{
    return cJSON_AddStringToObject(mJSON, aname, avalue);
}

cJSON* wcJSON::add_cObject(const char* aname)
{
    return cJSON_AddObjectToObject(mJSON, aname);
}

cJSON* wcJSON::add_cArray(const char* aname)
{
    return cJSON_AddArrayToObject(mJSON, aname);
}

void wcJSON::add_cItem(const char* aname, cJSON * item)
{
    cJSON_AddItemToObject(mJSON, aname, item);
}

std::string  wcJSON::toString() {
    char * val = cJSON_PrintUnformatted(mJSON);
    std::string res(val);
    cJSON_free(val);
    return res;
}

tsObject wcJSON::locker;
