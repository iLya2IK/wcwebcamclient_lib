getConfig.json {#getConfig}
==========

Receiving a set of configurations from server. Description of the server configuration you can find [here](server-config.md).

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:"}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/getConfig.json?shash=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)

Response
------------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"config":[{"kind":0,"descr":":string:","miv":0.0,"mav":0.0,"dv":0.0,"fv":0.0},...],"result":"OK"}
~~~~~~~~~~~~~

* _**config**_ - the array of configurations received
* _**kind**_ - the integer id of a specific configuration
* _**descr**_  - description for a specific configuration
* _**miv**_  - minimum allowed value for a specific configuration
* _**mav**_  - maximum allowed value for a specific configuration
* _**dv**_  - default value for a specific configuration
* _**fv**_  - current value for a specific configuration that applied for account

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE="}' -X POST https://localhost:8080/getConfig.json
{"config":[{"kind":1,"descr":"old records timeout (days)","miv":4.0E-003,"mav":1.68E+002,"dv":3.0E+001,"fv":3.0E+001},{"kind":2,"descr":"dead sessions timeout (minutes)","miv":3.0E+000,"mav":3.0E+001,"dv":1.0E+001,"fv":1.0E+001},{"kind":3,"descr":"max requests for session","miv":2.0E+002,"mav":1.0E+004,"dv":1.5E+003,"fv":1.5E+003},{"kind":4,"descr":"max reqs per min for session","miv":2.0E+001,"mav":2.0E+002,"dv":5.0E+001,"fv":5.0E+001}],"result":"OK"}
```
