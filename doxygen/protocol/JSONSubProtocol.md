JSON sub-protocol description {#JSONProtocolHome}
===============

Data exchange between devices is carried out according to the HTTP/2 protocol using the POST method. The contents of requests and responses are JSON objects. Both methods of passing parameters from the client to the server are available - as a URL string after the "?" (where it is possible or necessary) or/and in the content of the request as a string describing the JSON object. The server always responds with a JSON object in the content of the response (excluding the [getRecordData.json](\ref getRecordData) request).
 
The following requests are allowed

* [authorize.json](\ref authorize)
* [addRecord.json](\ref addRecord)
* [addMsgs.json](\ref addMsgs)
* [getRecordMeta.json](\ref getRecordMeta)
* [getRecordData.json](\ref getRecordData)
* [getRecordCount.json](\ref getRecordCount)
* [deleteRecords.json](\ref deleteRecords)
* [getMsgs.json](\ref getMsgs) 
* [getMsgsAndSync.json](\ref getMsgsAndSync) 
* [getDevicesOnline.json](\ref getDevicesOnline) 
* [getStreams.json](\ref getStreams) 
* [getConfig.json](\ref getConfig)
* [setConfig.json](\ref setConfig) 
* [heartBit.json](\ref heartBit)

Streaming
* [output.raw](\ref output) 
* [input.raw](\ref input) 

[Example of protocol application and possible message format for a client-server application](https://github.com/iLya2IK/webcamdevice)

[TOC]

-----------

#include "getConfig.json.md"


- - - - - -

#include "setConfig.json.md"

- - - - - -

#include "server-config.md"

- - - - - -

#include "Data-stream-format.md"

- - - - - -

#include "output.raw.md"

- - - - - -

#include "input.raw.md"

- - - - - -

#include "authorize.json.md"

- - - - - -

#include "addRecord.json.md"

- - - - - -

#include "addMsgs.json.md"

- - - - - -

#include "getRecordMeta.json.md"

- - - - - -

#include "getRecordData.json.md"

- - - - - -

#include "getRecordCount.json.md"

- - - - - -

#include "getMsgsAndSync.json.md"

- - - - - -

#include "getMsgs.json.md"

- - - - - -

#include "getDevicesOnline.json.md"

- - - - - -

#include "getStreams.json.md"

- - - - - -

#include "deleteRecords.json.md"

- - - - - -

#include "heartBit.json.md"

- - - - - -

#include "JSON_BAD.md" 
