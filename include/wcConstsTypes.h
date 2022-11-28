#ifndef WCCONSTSTYPES_H
#define WCCONSTSTYPES_H

#include <wcDefines.h>
#include <tsCURLM.h>

#define WC_BASE_TASK        0
#define WC_TASK             1
#define WC_IN_STREAM_TASK   2
#define WC_OUT_STREAM_TASK  4
#define WC_ALL_TASKS        0xffffffff

#define STATE_INIT       0
#define STATE_FINISHED   1
#define STATE_TERMINATED 2

#define TASK_NO_ERROR 0x0
#define TASK_ERROR_ATTACH_REQ 0x10
#define TASK_ERROR_CANT_EASY_CURL 0x11
#define TASK_ERROR_GET_INFO 0x12
#define TASK_ERROR_CURL 0x13

#define ERR_WEBCAM_STREAM_BUFFER_OVERFLOW 0x21
#define ERR_WEBCAM_STREAM_FRAME_TO_BIG 0x22
#define ERR_WEBCAM_STREAM_WRONG_HEADER 0x23

extern const uint16_t WEBCAM_FRAME_START_SEQ;
#define WEBCAM_FRAME_HEADER_SIZE  (sizeof(uint16_t) + sizeof(uint32_t))
extern const uint32_t WEBCAM_FRAME_BUFFER_SIZE;

typedef char TErrorBuffer[CURL_ERROR_SIZE + 1];

extern const char * C_UNKNOWN_ERROR;

extern const char * JSON_RPC_OK;
extern const char * JSON_RPC_BAD;

extern const char * REST_SYNC_MSG;
extern const char * JSON_RPC_SYNC;
extern const char * JSON_RPC_CONFIG;
extern const char * JSON_RPC_MSG;
extern const char * JSON_RPC_MSGS;
extern const char * JSON_RPC_RECORDS;
extern const char * JSON_RPC_DEVICES;
extern const char * JSON_RPC_RESULT;
extern const char * JSON_RPC_CODE;
extern const char * JSON_RPC_NAME;
extern const char * JSON_RPC_PASS;
extern const char * JSON_RPC_SHASH;
extern const char * JSON_RPC_META;
extern const char * JSON_RPC_STAMP;
extern const char * JSON_RPC_MID;
extern const char * JSON_RPC_RID;
extern const char * JSON_RPC_DEVICE;
extern const char * JSON_RPC_TARGET;
extern const char * JSON_RPC_PARAMS;
extern const char * JSON_RPC_SUBPROTO;
extern const char * JSON_RPC_DELTA;

#define MAX_RESPONSE_ERRORS 15

extern const char * RESPONSE_ERRORS[];

std::string str_format(const char * format, ...);
std::string vstr_format(const char * format, va_list args);
std::string encodeHTTP(const std::string & aStr);
void decodeProxy(const std::string & aStr, std::string & aProtocol, std::string & aHost, std::string & aPort, std::string & aUName, std::string & aPwrd);

#endif // WCCONSTSTYPES_H
