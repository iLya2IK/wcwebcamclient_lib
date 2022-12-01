/*! \file main.cpp
 *  A simple test program to demonstrate the API of the wcWebCamClient library.
 */

/*===============================================================*/
/* This is an example of how to use the wcWebCamClient library.  */
/* In this example, a client is created, authorized on the       */
/* server, and downloads a list of active devices.               */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include <iostream>
#include <wcwebcamclient.h>
#include <chrono>
#include <thread>
#include "../commline_tools.h"

using namespace std;

static uint8_t running = 0;

/* Callback. CURL was initialized successfully. */
void onCURLinit(wcHandle self, wcTask task)
{
    cout << "CURL initialized" << endl;
}

/* Callback. Authorization was successful. */
void onAuth(wcHandle self, wcTask task)
{
    char * res;
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
void onConnChanged(wcHandle self, bool state)
{
    cout << "Client connection changed " << state << endl;
}

/* Callback. The list of devices was changed. */
void getDevices(wcHandle self, wcTask task, const char * jArr)
{
    /* Display a list of online devices on the screen. */
    cout << jArr << endl;
    running = 4;
}

/* Callback. Log was changed. */
void onLog(wcHandle self, const char * str)
{
    /* Display new log entry. */
    cout << str << endl;
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
        wcSetJSONStrCallback(client, wccbkSuccessUpdateDevices, getDevices);
        wcSetCStringCallback(client, wccbkAddLog, onLog);
        wcSetNotifyCallback(client, wccbkInitCURL, onCURLinit);
        wcSetNotifyCallback(client, wccbkDisconnect, onDisconnect);
        wcSetConnCallback(client, onConnChanged);

        /* Start client */
        wcClientStart(client);

        int time_out = 100;
        while (running < 4)
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
                /* Update list of active devices */
                wcClientInvalidateState(client, wcstDevices);
                running = 3;
                break;
            }
            default:
            {
                break;
            }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
