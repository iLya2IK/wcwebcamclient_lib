input.raw {#input}
==========

Receiving data stream from selected device.

Request
------------
JSON object in request content is not allowed. 
Query string
`https://localhost/input.raw?shash=string&device=string`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**device**_ - selected device name (required). Value can be obtained with [getDevicesOnline.json](\ref getDevicesOnline) and [getStreams.json](\ref getStreams) requests

Response
------------

<h3>JSON_OK response</h3>

Server open the data stream from selected device. The description of data stream format you can find [here](Data-stream-format.md)

Example
------------
```
> curl --http2 -k -X POST https://localhost:8080/input.raw?shash=ra5PNJRgzrqpwbfw8JvHIkWuxJE%3D&device=camera_device

```
