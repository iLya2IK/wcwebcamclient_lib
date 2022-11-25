getStreams.json {#getStreams}
==========

Obtaining a list of streaming devices authorized under the same account

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:"}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/getStreams.json?shash=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)

Response
------------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"result":"OK","devices":[":string:",...]}
~~~~~~~~~~~~~

* _**devices**_ - array of streaming device names received

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE="}' -X POST https://localhost:8080/getStreams.json
{"result":"OK","devices":["user_pc","camera1"]}
```
 
