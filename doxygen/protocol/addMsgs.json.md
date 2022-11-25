addMsgs.json {#addMsgs}
===============

Send messages to the server.

Request
--------

The JSON object

variant 1

~~~~~~~~~~~~~{.java}
{"shash":":string:", "msg":":string:", "target":":string:", "params":{...}}
~~~~~~~~~~~~~

variant 2

~~~~~~~~~~~~~{.java}
{"shash":":string:", "msgs":[{"msg":":string:", "target":":string:", "params":{...}},...]}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/addMsgs.json?shash=string&msg=string&device=string&target=string&params=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**msg**_ - the name of the message inside the internal logic of your client-server application (required)
* _**target**_ - message recipient device name (optional). If this parameter is empty, the message is broadcast and will be sent to all devices in the account.
* _**params**_ - message parameters inside the internal logic of your client-server application (optional)
* _**msgs**_ - the array of messages to send (optional)

Response
--------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"result":"OK"}
~~~~~~~~~~~~~

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Sync message
--------

After receiving messages from the server, the client application must send a synchronization message, otherwise the messages will be resent by the server.
The format of sync message is
~~~~~~~~~~~~~{.java}
{"shash":":string:", "msg":"sync"}
~~~~~~~~~~~~~

Example
--------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","msg":"obey","params":{"please":1}}' -X POST https://localhost:8080/addMsgs.json
{"result":"OK"}

> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","msgs":[{"msg":"status","target":"device1"},{"msg":"shutdown","target":"device2"}]' -X POST https://localhost:8080/addMsgs.json
{"result":"OK"}
```
