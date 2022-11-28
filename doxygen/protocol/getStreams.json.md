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
{"result":"OK","devices":[{"device":":string:","subproto":":string:","delta":integer},...]}
~~~~~~~~~~~~~

* _**devices**_ - array of streaming devices received

* _**device**_ - the streaming device name
* _**subproto**_ - the stream sub-protocol description
* _**delta**_ - the minimal delta time between two frames in ms

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE="}' -X POST https://localhost:8080/getStreams.json
{"result":"OK","devices":[{"device":"user_pc","subproto":"DEFLATE_JSON","delta":30000},{"device":"camera1","subproto":"RAW_THEORA","delta":400}]}
```
 
