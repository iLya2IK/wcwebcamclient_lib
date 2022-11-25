authorize.json {#authorize}
==========

Authorizes the device on the server under the specified account. 
Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"name":":string:", "pass":":string:", "device":":string:", "meta":":string:"}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/authorize.json?name=string&pass=string&device=string`

<h3>Parameters</h3>

* _**name**_ - the name of user's account (required)
* _**pass**_ - the password for user's account (required)
* _**device**_ - device name - a unique value within one account to identify the target and source of the message - usually the device's mac address (required)
* _**meta**_ - metadata for this device (optional)

Response
------------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"shash":":string:", "result":"OK"}
~~~~~~~~~~~~~
* _**shash**_ - new session id returned

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"name":"user","pass":"111","device":"user_pc","meta":"some info"}' -X POST https://localhost:8080/authorize.json
{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","result":"OK"}
```
