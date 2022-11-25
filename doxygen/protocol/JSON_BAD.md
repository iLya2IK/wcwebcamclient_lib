JSON_BAD message format {#JSON_BAD}
======================

~~~~~~~~~~~~~{.java}
{"result":"BAD","code":integer}
~~~~~~~~~~~~~
* _**code**_ - value of error code returned by server

<h3>Possible error codes</h3>
```
0 - NO_ERROR               - no error detected
1 - UNSPECIFIED            - unspecified error, can be used inside 
                             client applications to determinate internal 
                             client errors
2 - INTERNAL_UNKNOWN_ERROR - internal server error (server system error)
3 - DATABASE_FAIL          - error inside sqlite3 engine occurred
4 - JSON_PARSER_FAIL       - wrong JSON format
5 - JSON_FAIL              - other handled server errors, caused by 
                             misrepresentation of parameters inside 
                             request
6 - NO_SUCH_SESSION        - shash (session id) passed by client is 
                             wrong or expired
7 - NO_SUCH_USER           - no such account found (wrong user's name or 
                             password)
8 - NO_DEVICES_ONLINE      - no devices found
9 - NO_SUCH_RECORD         - rid (record id) passed by client is wrong 
                             or erased
10 - NO_DATA_RETURNED      - the requested record inside database is 
                             empty
11 - EMPTY_REQUEST         - client sends an empty request
12 - MALFORMED_REQUEST     - client sends malformed request (some 
                             required parameters are missing or 
                             malformed) 
13 - NO_CHANNEL            - the requested stream does not exist
14 - ERRORED_STREAM        - the stream has errors
15 - NO_SUCH_DEVICE        - device is not founded
```
