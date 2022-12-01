getRecordCount.json {#getRecordCount}
==========

Receiving a list of media records from the server. The server sends only a list of records, to get the data of the record, the client must request each record separately by [getRecordData.json](\ref getRecordData) request.

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:", "stamp":":string:"}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/getRecordCount.json?shash=string&stamp=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**stamp**_ - the timestamp of the last received media record. The server will send all media records that arrived later (not including) than the given timestamp (optional)

Response
------------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"records":[{"rid":integer,"device":":string:","stamp":":string:"},...],"result":"OK"}
~~~~~~~~~~~~~

* _**records**_ - the array of media records received
* _**rid**_ - the integer id of a media record
* _**device**_  - record emitter device name.
* _**stamp**_  - the timestamp of a media record. The timestamp is in a format determined by the server and may not match the client's local settings.

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE="}' -X POST https://localhost:8080/getRecordCount.json
{"records":[{"rid":34,"device":"d1","stamp":"2022-02-07 12:29:36"}],"result":"OK"}

> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","stamp":"2022-02-07 12:29:36"}' -X POST https://localhost:8080/getRecordCount.json
{"records":[],"result":"OK"}
```
