# wcWebCamClient Library

This is a library for convenient client work with the wcWebCamServer server via the JSON protocol.

## The structure of the client-server software package
The server designed to collect images and data streams from cameras (devices) and forwards messages between devices to control the periphery via an HTTP 2 connection is [wcwebcamserver (Lazarus/Free Pascal)](https://github.com/iLya2IK/wcwebcamserver).
Abstract client for Lazarus is [wccurlclient (Lazarus/Free Pascal)](https://github.com/iLya2IK/wccurlclient).
A detailed implementation of an external device based on "ESP32-CAM" is given in the example [webcamdevice (С)](https://github.com/iLya2IK/webcamdevice).
The example of a desktop application for external device controlling and viewing images is [webcamclientviewer (Lazarus)](https://github.com/iLya2IK/webcamclientviewer).
An example of an Android application for controlling external devices, chatting and streaming is [wcwebcameracontrol (Java)](https://github.com/iLya2IK/wcwebcameracontrol).

## Documents

[wcWebCamClient library API User's Guide - Doxygen](https://ilya2ik.github.io/wcwebcamclient_lib/index.html)

## Development environment
* [Code::Blocks](https://www.codeblocks.org/)
* [GNU Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html)

## How to build library with automake

````
make -f wcwebcamclient.cbp.mak [clean_]{build_target}_{platform}
````

### Build targets:
1. `release`
2. `debug`

### Platfroms:
1. `linux`
2. `win`

### Example:
````
make -f wcwebcamclient.cbp.mak release_linux
make -f wcwebcamclient.cbp.mak clean_release_linux
````

## Necessary libraries
1. OpenSSL (v1.1.0 or higher)
2. cURL (with http2 support)
3. cJSON

# Copyrights and contributions
* [OpenSSL - TLS/SSL and crypto library](https://www.openssl.org/)
* [cURL - A command library for transferring data with URL syntax](https://curl.se/libcurl)
* [cJSON - Ultralightweight JSON parser in ANSI C](https://github.com/DaveGamble/cJSON)
