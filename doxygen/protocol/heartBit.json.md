heartBit.json {#heartBit}
==========

Sends a "heart beat" message to refresh session state. After a certain time, the server deletes sessions if they do not send messages (for details, see the [article](\ref getConfig)). If the client needs to maintain a background connection to the server, "heart beat" messages must be sent periodically. 

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:"}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/heartBit.json?name=string&shash=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)

Response
------------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"result":"OK"}
~~~~~~~~~~~~~

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE="}' -X POST https://localhost:8080/heartBit.json
{"result":"OK"}
```
