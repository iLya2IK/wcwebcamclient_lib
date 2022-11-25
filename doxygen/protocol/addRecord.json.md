addRecord.json {#addRecord}
==========

Send blob data and create new media record.

Request
------------

JSON object in request content is not allowed. The content of the request must contain raw blob data.

Query string
`https://localhost/addRecord.json?shash=string&meta=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**meta**_ - text data assigned to the media record by the client. For example, the format of the media record can be specified here. By default, the media record format is JPEG.

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
> curl --http2 -k --data-binary '@inp.png' -X POST https://localhost:8080/addRecordData.json?shash=ra5PNJRgzrqpwbfw8JvHIkWuxJE%3D&meta=PNG
{"result":"OK"}
```
