/*===============================================================*/
/* Common constants                                              */
/* wcConsts.cpp                                                  */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include <wcConstsTypes.h>

const uint16_t WEBCAM_FRAME_START_SEQ   = 0xaaaa;
const uint32_t WEBCAM_FRAME_BUFFER_SIZE = 0x200000;

const char * C_UNKNOWN_ERROR  =  "Unknown internal error";

const char * JSON_RPC_OK      =  "OK";
const char * JSON_RPC_BAD     =  "BAD";

const char * REST_SYNC_MSG    =  "{\"msg\":\"sync\"}";
const char * JSON_RPC_SYNC    =  "sync";
const char * JSON_RPC_CONFIG  =  "config";
const char * JSON_RPC_MSG     =  "msg";
const char * JSON_RPC_MSGS    =  "msgs";
const char * JSON_RPC_RECORDS =  "records";
const char * JSON_RPC_DEVICES =  "devices";
const char * JSON_RPC_RESULT  =  "result";
const char * JSON_RPC_CODE    =  "code";
const char * JSON_RPC_NAME    =  "name";
const char * JSON_RPC_PASS    =  "pass";
const char * JSON_RPC_SHASH   =  "shash";
const char * JSON_RPC_META    =  "meta";
const char * JSON_RPC_STAMP   =  "stamp";
const char * JSON_RPC_MID     =  "mid";
const char * JSON_RPC_RID     =  "rid";
const char * JSON_RPC_DEVICE  =  "device";
const char * JSON_RPC_TARGET  =  "target";
const char * JSON_RPC_PARAMS  =  "params";

const char * RESPONSE_ERRORS[] = {
                          "NO_ERROR",
                          "UNSPECIFIED",
                          "INTERNAL_UNKNOWN_ERROR",
                          "DATABASE_FAIL",
                          "JSON_PARSER_FAIL",
                          "JSON_FAIL",
                          "NO_SUCH_SESSION",
                          "NO_SUCH_USER",
                          "NO_DEVICES_ONLINE",
                          "NO_SUCH_RECORD",
                          "NO_DATA_RETURNED",
                          "EMPTY_REQUEST",
                          "MALFORMED_REQUEST",
                          "NO_CHANNEL",
                          "ERRORED_STREAM",
                          "NO_SUCH_DEVICE"};
