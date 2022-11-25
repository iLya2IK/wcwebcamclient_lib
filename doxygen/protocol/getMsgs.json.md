getMsgs.json {#getMsgs}
==========

Receiving messages from the server.

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:", "stamp":":string:"}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/getMsgs.json?shash=string&stamp=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**stamp**_ - the timestamp of the last received message. The server will send all messages that arrived later (not include) than the given timestamp, otherwise all messages will be resent from the last [sync message](\ref addMsgs) (optional)

Response
------------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"msgs":[{"msg":":string:","device":":string:","stamp":":string:","params":{...}},...],"result":"OK"}
~~~~~~~~~~~~~

* _**msgs**_ - the array of messages received
* _**msg**_ - the name of the message inside the internal logic of your client-server application
* _**device**_  - message emitter device name.
* _**params**_  - message parameters inside the internal logic of your client-server application
* _**stamp**_  - the timestamp of the message. The timestamp is in a format determined by the server and may not match the client's local settings.

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE="}' -X POST https://localhost:8080/getMsgs.json
{"msgs":[{"msg":"1","device":"dev1","params":{},"stamp":"2022-02-07 12:28:35"},{"msg":"2","device":"dev2","params":{},"stamp":"2022-02-07 14:00:00"}],"result":"OK"}

> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","stamp":"2022-02-07 12:28:35"}' -X POST https://localhost:8080/getMsgs.json
{"msgs":[{"msg":"2","device":"dev2","params":{},"stamp":"2022-02-07 14:00:00"}],"result":"OK"}

> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","msg":"sync"}' -X POST https://localhost:8080/addMsgs.json
{"result":"OK"}

> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE="}' -X POST https://localhost:8080/getMsgs.json
{"msgs":[],"result":"OK"}
```
