getDevicesOnline.json {#getDevicesOnline}
==========

Obtaining a list of active (online) devices authorized under the same account

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:"}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/getDevicesOnline.json?shash=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)

Response
------------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"result":"OK","devices":[{"device":":string:","meta":":string:"},...]}
~~~~~~~~~~~~~

* _**devices**_ - array of online devices received
* _**device**_ - the device name
* _**meta**_ - the meta data for the device

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE="}' -X POST https://localhost:8080/getDevicesOnline.json
{"result":"OK","devices":[{"device":"user_pc","meta":"some info"},{"device":"d1c","meta":"{\"param\":1}"},{"device":"d2","meta":"cam v0.3"}]}
```
