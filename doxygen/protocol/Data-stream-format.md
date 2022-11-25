Data stream format {#DataStreamFormat}
=======


A valid data stream is a sequence of frames of the following format:

```
+--------------+
| Data frame 1 |
+--------------+
| Data frame 2 |
+--------------+
.......
+--------------+
| Data frame N |
+--------------+
```

Each data frame has the format shown below (the number of bits is indicated in parentheses).
```
+-------------------+
|  Header seq (16)  |  = 0xaaaa
+-------------------+
| Payload size (32) |
+-------------------+
|   Payload (0..*)  |
+-------------------+
```

