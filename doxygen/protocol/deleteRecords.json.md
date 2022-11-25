deleteRecords.json {#deleteRecords}
==========

Delete specified media records from server.

Request
------------

The JSON object

~~~~~~~~~~~~~{.java}
{"shash":":string:", "records":[integer_array]}
~~~~~~~~~~~~~

Possible (alternative) query string
`https://localhost/getRecordData.json?shash=string&records=%5Binteger_array%5D`

<h3>Parameters</h3>

* _**shash**_ - session id (required)
* _**records**_ - an array of integers, each element of which is a media record ID (required). Media record Ids can be obtained using [getRecordCount.json](\ref getRecordCount) request. if the array contains a negative number, the server in addition will delete all records that have a timestamp older than the oldest record passed with the request. 

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
The following records exist on the server:

<table>
  <tr>
    <th>rid<br></th>
    <th>stamp</th>
  </tr>
  <tr>
    <td>30</td>
    <td>2022-02-07 12:29:32</td>
  </tr>
  <tr>
    <td>32</td>
    <td>2022-02-07 12:30:00</td>
  </tr>
  <tr>
    <td>33</td>
    <td>2022-02-07 13:00:00</td>
  </tr>
  <tr>
    <td>34</td>
    <td>2022-02-07 13:30:00</td>
  </tr>
  <tr>
    <td>40</td>
    <td>2022-02-07 14:00:00</td>
  </tr>
  <tr>
    <td>42</td>
    <td>2022-02-07 14:30:00</td>
  </tr>
</table>

```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","records":[40]}' -X POST https://localhost:8080/deleteRecords.json
{"result":"OK"}
```

After the request, the following records remained on the server:

<table>
  <tr>
    <th>rid<br></th>
    <th>stamp</th>
  </tr>
  <tr>
    <td>30</td>
    <td>2022-02-07 12:29:32</td>
  </tr>
  <tr>
    <td>32</td>
    <td>2022-02-07 12:30:00</td>
  </tr>
  <tr>
    <td>33</td>
    <td>2022-02-07 13:00:00</td>
  </tr>
  <tr>
    <td>34</td>
    <td>2022-02-07 13:30:00</td>
  </tr>
  <tr>
    <td>42</td>
    <td>2022-02-07 14:30:00</td>
  </tr>
</table>

```
> curl --http2 -k -d '{"shash":"ra5PNJRgzrqpwbfw8JvHIkWuxJE=","records":[-1,34,32]}' -X POST https://localhost:8080/deleteRecords.json
{"result":"OK"}
```

After the last request, the following records remained on the server:

<table>
  <tr>
    <th>rid<br></th>
    <th>stamp</th>
  </tr>
  <tr>
    <td>33</td>
    <td>2022-02-07 13:00:00</td>
  </tr>
  <tr>
    <td>42</td>
    <td>2022-02-07 14:30:00</td>
  </tr>
</table>
