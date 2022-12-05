getRecordMeta.json {#getRecordMeta}
==========

Receiving metadata for a media record from the server.

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:", "rid":integer}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/getRecordMeta.json?shash=string&rid=integer`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**rid**_ - the id of a media record (required). Value can be obtained with [getRecordCount.json](\ref getRecordCount) request

Response
------------

<h3>JSON_OK response</h3>

~~~~~~~~~~~~~{.java}
{"device":":string:","meta":":string:","stamp":":string:","result":"OK"}
~~~~~~~~~~~~~

* _**device**_  - media record emitter device name.
* _**meta**_ - text data assigned to the media record by the client. For example, the format of the media record can be specified here. By default, the media record format is JPEG. 
* _**stamp**_  - the timestamp of a media record. The timestamp is in a format determined by the server and may not match the client's local settings.

<h3>JSON_BAD response</h3>
[JSON_BAD response format and description of error codes](\ref JSON_BAD) 

Example
------------
```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","rid":34}' -X POST https://localhost:8080/getRecordMeta.json
{"device":"34945423FA90","record":"","stamp":"2022-02-07 12:29:36","result":"OK"}
```
