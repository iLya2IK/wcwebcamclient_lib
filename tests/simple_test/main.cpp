#include <iostream>
#include "wcConstsTypes.h"
#include "wcCURLClient.h"
#include "wcHTTP2BackgroundTask.h"
#include "wcJSON.h"
#include <string>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include "../commline_tools.h"


using namespace std;

static uint8_t running = 0;

void onCURLinit(wcCallbackClient self, wcCallbackTask task)
{
    cout << "CURL initialized" << endl;
}

void onAuth(wcCallbackClient self, wcCallbackTask task)
{
    cout << static_cast<wcCURLClient *>(self)->getSID().c_str() << endl;
    running = 2;
}

void onDisconnect(wcCallbackClient self, wcCallbackTask task)
{
    cout << "Client disconnected" << endl;
    running = 5;
}

void onConnChanged(wcCallbackClient self, bool state)
{
    cout << "Client connection changed " << state << endl;
}

void getDevices(wcCallbackClient self, wcCallbackTask task, const char * jArr)
{
    cout << jArr << endl;
    running = 4;
}

void onLog(wcCallbackClient self, const std::string & str)
{
    cout << str.c_str() << endl;
}

int main(int argc, char * argv[])
{
    InputParser input(argc, argv);
    const std::string &host = input.getHost();
    const std::string &proxy = input.getProxy();
    const std::string &user = input.getUserName();
    const std::string &pwrd = input.getUserPassword();
    std::string device = input.getDevice();
    if (device.empty()) device = "device_test002";

    if (input.showHelp()) {
        input.printCommonHelp();
        return 0;
    }

    if (!host.empty()){

        wcCURLClient client;

        if (input.ignoreTLSCert())
            client.setVerifyTLS(false);

        if (!proxy.empty()) {
            client.setProxy(proxy);
        }

        client.setHost(host);

        client.setOnSuccessAuth(onAuth);
        client.setOnAltStrSuccessUpdateDevices(getDevices);
        client.setOnAddLog(onLog);
        client.setOnInitCURL(onCURLinit);
        client.setOnDisconnect(onDisconnect);
        client.setOnConnectedChanged(onConnChanged);
        client.start();

        int time_out = 100;
        while (running < 4)
        {
            switch (running)
            {
            case 0:
            {
                client.setDevice(device);
                client.authorize(user, pwrd);
                running = 1;
                break;
            }
            case 2:
            {
                client.setNeedToUpdateDevices(true);
                running = 3;
                break;
            }
            default:
            {
                break;
            }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            client.proceed();
            client.tasksProceed();
            time_out--;
            if (time_out <= 0) {
                cout << "timeout" << endl;
                break;
            }
        }
        client.disconnect();
    } else {
        cout << "should contain at least --host (host-name) option";
    }

    return 0;
}
