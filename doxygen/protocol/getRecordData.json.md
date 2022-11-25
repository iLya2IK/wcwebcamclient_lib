getRecordData.json {#getRecordData}
==========

Receiving blob data for a media record from the server.

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:", "rid":integer}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/getRecordData.json?shash=string&rid=integer`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**rid**_ - the id of a media record (required). Value can be obtained with [getRecordCount.json](\ref getRecordCount) request

Response
------------

<h3>JSON_OK response</h3>

Server returns a raw blob data for selected media record.

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","rid":34}' -X POST https://localhost:8080/getRecordData.json -o out.jpg
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100 41480  100 41431  100    49   299k    363 --:--:-- --:--:-- --:--:--  302k
```
