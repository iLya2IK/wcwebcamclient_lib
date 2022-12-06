/*! \file main.cpp
 *  A simple test program to demonstrate how to save and load media records.
 */

/*===============================================================*/
/* This is an example of how to use the wcWebCamClient library.  */
/* In this example, a client is created, authorized on the       */
/* server, uploads a media record and downloads it to disk.      */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include <iostream>
#include <fstream>
#include <wcwebcamclient.h>
#include <chrono>
#include <thread>
#include <wcJSON.h>
#include "../commline_tools.h"

using namespace std;

static uint8_t running = 0;

static const string input_media_file("morti.png");
static const string cRID("rid");
static const string cMETA("meta");
static const string cTIMESTAMP("stamp");

static string metadata("");
static int rid;

/* Callback. CURL was initialized successfully. */
void onCURLinit(wcHandle self, wcTask task)
{
    cout << "CURL initialized" << endl;
}

/* Callback. Authorization was successful. */
void onAuth(wcHandle self, wcTask task)
{
    char * res = NULL;
    /* Get a new session id and display it on the screen. */
    if (wcClientGetStrValue(self, wcstSID, &res) == WC_OK) {
        if (res) {
            cout << res << endl;
            free(res);
        }
    }
    running = 2;
}

/* Callback. The client has been disabled. */
void onDisconnect(wcHandle self, wcTask task)
{
    cout << "Client disconnected" << endl;
    running = 5;
}

/* Callback. Connection state changed. */
void onConnChanged(wcHandle self, int state)
{
    cout << "Client connection changed " << state << endl;
}

/* Callback. Log was changed. */
void onLog(wcHandle self, const char * str)
{
    /* Display new log entry. */
    cout << str << endl;
}


/* Callback. The list of media records was changed. */
void getRecords(wcHandle self, wcTask task, const char * jArr)
{
    wcJSON jarr(jArr);
    if (jarr.isArray()) {
        int i = jarr.length() - 1;
        if (i >= 0) {
            wcJSON obj = jarr.getArrayItem(i);
            if (obj.isObject()) {
                wcJSON jrid = obj.getObjItem(cRID.c_str());
                if (jrid.isNumber()) {
                     rid = jrid.valueInt();
                     running = 5;
                     return;
                }
            }
        }
    }
    // error in server response
    running = 10;
}

/* Callback. The media record metadata received. */
void getRecordMeta(wcHandle self, wcTask task, const char * jObj)
{
    wcJSON j(jObj);
    if (j.isValid()) {
        wcJSON meta = j.getObjItem(cMETA.c_str());
        if (meta.isString()) {
            metadata = string(meta.valueString());
            running = 6;
            return;
        }
    }
    // error in server response
    running = 10;
}

/* Callback. The request to save the media record has been completed. The response has arrived. */
void afterSaveRecord(wcHandle self, wcTask task)
{
    running = 3;
}

/* Callback. The request to get the media record has been completed. The response has arrived. */
void getRecordData(wcHandle client, wcTask tsk, void* data, size_t sz)
{
    string output_file = "record" + to_string(rid) + "." + metadata;
    ofstream outfile(output_file, ios::out | ios::trunc | ios::binary);
    if (!outfile.is_open())
    {
        cerr << "Can't open output file. Wrong file name or the file is blocked: " << output_file << endl;
        return;
    }
    outfile.write((const char*)data, sz);
    outfile.close();
    running = 10;
}

int main(int argc, char * argv[])
{
    /* Parse command line */
    InputParser input(argc, argv);
    const std::string &host = input.getHost();
    const std::string &proxy = input.getProxy();
    const std::string &user = input.getUserName();
    const std::string &pwrd = input.getUserPassword();
    std::string device = input.getDevice();
    if (device.empty()) device = "device_test001";

    if (input.showHelp()) {
        /* Show help doc */
        input.printCommonHelp();
        return 0;
    }

    if (!host.empty()){
        /* Main part */
        wcHandle client = wcClientCreate();

        /* Configure certificate */
        if (input.ignoreTLSCert())
            wcClientSetBoolState(client, wcstVerifyTLS, WC_FALSE);
        /* Configure proxy */
        if (!proxy.empty())
            wcClientSetStrValue(client, wcstProxy, proxy.c_str());
        /* Configure host */
        wcClientSetStrValue(client, wcstHostName, host.c_str());

        /* Configure callbacks */
        wcSetTaskCallback(client, wccbkSuccessAuth, onAuth);
        wcSetCStringCallback(client, wccbkAddLog, onLog);
        wcSetNotifyCallback(client, wccbkInitCURL, onCURLinit);
        wcSetNotifyCallback(client, wccbkDisconnect, onDisconnect);
        wcSetConnCallback(client, onConnChanged);

        /* Configure callbacks to work with media records */
        wcSetJSONStrCallback(client, wccbkSuccessUpdateRecords, getRecords);
        wcSetJSONStrCallback(client, wccbkSuccessRequestRecordMeta, getRecordMeta);
        wcSetTaskCallback(client, wccbkSuccessSaveRecord, afterSaveRecord);
        wcSetAltDataCallback(client, wccbkSuccessRequestRecord, getRecordData);

        /* Start client */
        wcClientStart(client);

        int time_out = 100;
        while (running < 10)
        {
            switch (running)
            {
            case 0:
            {
                /* Authorize client */
                wcClientSetStrValue(client, wcstDeviceName, device.c_str());
                wcClientAuth(client, user.c_str(), pwrd.c_str());
                running = 1;
                break;
            }
            case 2:
            {
                /* Read input_media_file and send to server */

                ifstream fp (input_media_file, ios::binary);
                if (!fp.is_open()) {
                    cerr << input_media_file << ": error: " << errno << endl;
                    return 1;
                }

                fp.seekg (0, ios::end);
                long fsize = fp.tellg();
                fp.seekg (0, ios::beg);

                char * mem = (char*)malloc(fsize);

                fp.read (mem, fsize);

                fp.close();

                wcSaveRecord(client, mem, fsize, "PNG", mem);

                running = 4;
                break;
            }
            case 3:
            {
                /* Update list of media records to get the rid of the last one */
                wcClientInvalidateState(client, wcstRecords);
                running = 4;
                break;
            }
            case 5:
            {
                /* Get the record metadata */
                wcRequestRecordMeta(client, rid, NULL);
                running = 4;
                break;
            }
            case 6:
            {
                /* Get the record data */
                wcRequestRecord(client, rid, NULL);
                running = 4;
                break;
            }
            default:
            {
                break;
            }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            /* Proceed client */
            wcClientProceed(client);
            wcClientTasksProceed(client);
            time_out--;
            if (time_out <= 0) {
                cout << "timeout" << endl;
                break;
            }
        }
        /* Disconnect client */
        wcClientDisconnect(client);
        /* Destroy client */
        wcClientDestroy(client);
        return 0;
    } else {
        cout << "should contain at least --host (host-name) option";
    }
}
