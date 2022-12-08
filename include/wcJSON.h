#ifndef WCJSON_H_INCLUDED
#define WCJSON_H_INCLUDED

#include <wcConstsTypes.h>
#include <cJSON.h>

enum wcJSON_type { jsonBoolean, jsonNull, jsonNumber, jsonString, jsonArray, jsonObject, jsonRaw };

class wcJSON
{
    public:
        wcJSON(wcJSON_type atype, void * avalue);
        wcJSON(const char * value) {
            mJSON = intern_parse(value);
        };
        wcJSON(const wcJSON & value) :
            mJSON (value.mJSON),
            jobjown (value.jobjown),
            mErrorStr (value.mErrorStr)
        {};
        wcJSON(const std::string & value) {
            mJSON = intern_parse(value.c_str());
        }
        wcJSON(cJSON * aJson, bool is_objown) {
            mJSON = aJson;
            jobjown = is_objown;
            if (!ASSIGNED(mJSON)) {
                mErrorStr = std::string(C_UNKNOWN_ERROR);
            }
        };
        ~wcJSON() {
            clean();
        };


        void clean() {
            if (ASSIGNED(mJSON) && jobjown)
                cJSON_Delete(mJSON);
            mJSON = NULL;
        };
        void detach() { jobjown = false; };

        int length() {return cJSON_GetArraySize(mJSON); };

        bool isValid() { return ASSIGNED(mJSON); };
        bool isNonValid() { return !ASSIGNED(mJSON); };

        bool isBool() { return cJSON_IsBool(mJSON); };
        bool isNumber() { return cJSON_IsNumber(mJSON); };
        bool isString() { return cJSON_IsString(mJSON); };
        bool isArray() { return cJSON_IsArray(mJSON); };
        bool isObject() { return cJSON_IsObject(mJSON); };
        bool isRaw() { return cJSON_IsRaw(mJSON); };
        bool isNull() { return cJSON_IsNull(mJSON); };

        cJSON * getcJSON() {return mJSON;};

        char * valueString();
        int valueInt();
        double valueDouble();

        wcJSON getObjItem(const char * avalue) {
            return wcJSON(cJSON_GetObjectItem(mJSON, avalue), false);
        }

        wcJSON getArrayItem(int avalue) {
            return wcJSON(cJSON_GetArrayItem(mJSON, avalue), false);
        }

        wcJSON& operator=(const wcJSON& value)
        {
            mJSON = value.mJSON;
            mErrorStr = value.mErrorStr;
            jobjown = value.jobjown;
            return *this;
        }

        wcJSON  addBoolean(const char * aname, bool avalue);
        wcJSON  addInt(const char * aname, int avalue);
        wcJSON  addDouble(const char * aname, double avalue);
        wcJSON  addString(const char * aname, const char * avalue);
        wcJSON  addObject(const char * aname);
        wcJSON  addArray(const char * aname);

        cJSON * add_cBoolean(const char * aname, bool avalue);
        cJSON * add_cInt(const char * aname, int avalue);
        cJSON * add_cDouble(const char * aname, double avalue);
        cJSON * add_cString(const char * aname, const char * avalue);
        cJSON * add_cObject(const char * aname);
        cJSON * add_cArray(const char * aname);
        void    add_cItem(const char* aname, cJSON * item);

        std::string toString();

        std::string getErrorStr() {return mErrorStr;};
    private:
        cJSON * intern_parse(const char * value);
        cJSON * mJSON;
        bool jobjown {true};
        std::string mErrorStr;
        static tsObject locker;
};

#endif // WCJSON_H_INCLUDED
