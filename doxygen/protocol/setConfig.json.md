setConfig.json {#setConfig}
==========

Send a set of configurations to server. Description of the server configuration you can find [here](server-config.md).

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:","config":[{"kind":0,"fv":0.0},...]}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/setConfig.json?shash=string&config=%5Bobject_array%5D`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**config**_ - array of sent configurations
* _**kind**_ - integer id of a specific configuration
* _**fv**_  - current value for a specific configuration that applied for account

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
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","config":[{"kind":1,"fv":1.0},{"kind":4,"fv":100}]}' -X POST https://localhost:8080/setConfig.json
{"result":"OK"}
```
