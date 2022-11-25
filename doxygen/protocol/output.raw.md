output.raw {#output}
==========

Open data stream from device.

Request
------------
JSON object in request content is not allowed. 
Query string
`https://localhost/output.raw?shash=string`
The content of the request must contain [formatted](Data-stream-format.md) frames data.

<h3>Parameters</h3>

* _**shash**_ - session id (required)

Response
------------

There is no formatted response. If an error occurs, the stream is closed by the host.

Example
------------
```
curl --http2 -k --data-binary '@data.raw' -X POST https://localhost:8080/addRecordData.json?shash=ra5PNJRgzrqpwbfw8JvHIkWuxJE%3D

```
