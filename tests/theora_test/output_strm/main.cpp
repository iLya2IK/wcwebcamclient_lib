/*! \file main.cpp
 *  A simple test program to demonstrate the streaming of output data.
 */

/*===============================================================*/
/* This is an example of how to use the wcWebCamClient library.  */
/* In this example, a client is created, authorized on the       */
/* server, and starts to streaming. Each outgoing frame is an    */
/* PNG-image from the specified directory.                       */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*

 The example uses code from the Theora project (test png2theora).

 https://github.com/xiph/theora/blob/master/examples/png2theora.c
 *****************************************************************/

#include <iostream>
#include <fstream>
#include <wcwebcamclient.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <libgen.h>
#include <dirent.h>
#include "../../commline_tools.h"

using namespace std;

/* The max delta time between two frames */
static const uint32_t MAX_DELTA = 300;
/* The total program timeout in seconds */
static const uint32_t TIME_OUT = 60;

static uint8_t running = 0;

static chrono::time_point<chrono::high_resolution_clock> lst_timestamp;
static chrono::time_point<chrono::high_resolution_clock> cur_timestamp;

static string input_filter;
static string input_directory;
static char input_png[1024];
static dirent **png_files;

static char * mem_frame_buffer = NULL;
static long frame_size = 0, frame_buffer = 0;

static int16_t loc_frame = 0;
static int16_t total_frames = 0;

bool need_to_load_next_frame = false;

/* Read png-image to memory */
static int png_read(const char *pathname)
{
    streampos begin,end;
    ifstream fp (pathname, ios::binary);
    if (!fp.is_open()) {
        cerr << pathname << ": error: " << errno << endl;
        return 1;
    }

    fp.seekg (0, ios::end);
    long fsize = fp.tellg();
    fp.seekg (0, ios::beg);

    if (frame_buffer == 0) {
        frame_buffer = (((fsize + WC_STREAM_FRAME_HEADER_SIZE) >> 12) + 1) << 12;
        mem_frame_buffer = (char*)malloc(frame_buffer);
    } else {
        if (frame_buffer < fsize) {
            frame_buffer = (((fsize + WC_STREAM_FRAME_HEADER_SIZE) >> 12) + 1) << 12;
            mem_frame_buffer = (char*)realloc(mem_frame_buffer, frame_buffer);
        }
    }
    frame_size = fsize + WC_STREAM_FRAME_HEADER_SIZE;
    *(uint16_t*)mem_frame_buffer = WC_FRAME_START_SEQ;
    *(uint32_t*)(&(mem_frame_buffer[sizeof(uint16_t)])) = (uint32_t) fsize;

    fp.read (&(mem_frame_buffer[WC_STREAM_FRAME_HEADER_SIZE]), fsize);

    fp.close();
    return 0;
}

/* Check whether the file name (de->d_name) matches
   the specified filter (input_filter).
   src: xiph/theora/blob/master/examples/png2theora.c */
static int include_files (const struct dirent *de)
{
  static char name[1024];
  int number = -1;
  sscanf(de->d_name, input_filter.c_str(), &number);
  sprintf(name, input_filter.c_str(), number);
  return !strcmp(name, de->d_name);
}

/* Loads new data from the specified file, writes it to a frame,
   then inserts a new frame into the output stack.*/
bool load_next_frame(wcHandle client) {
    cur_timestamp = chrono::system_clock::now();
    auto int_ms = chrono::duration_cast<chrono::milliseconds>(cur_timestamp - lst_timestamp);

    if (need_to_load_next_frame && (int_ms.count() >= MAX_DELTA)) {
        snprintf(input_png, 1023, "%s/%s", input_directory.c_str(), png_files[loc_frame]->d_name);
        if(png_read(input_png)) {
          cerr << "could not read " << input_png << endl;
          return true;
        }
        loc_frame++;
        if (loc_frame >= total_frames) loc_frame = 0;
        if (mem_frame_buffer && (frame_size > 0)) {
            if (wcClientFrameLock(client) == WC_OK) {
                wcClientFramePushData(client, mem_frame_buffer, frame_size);
                wcClientFrameUnLock(client);
            }
        }
        lst_timestamp = cur_timestamp;
        need_to_load_next_frame = false;
    }
    return false;
}

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

/* Callback. IO stream closed. */
void onIOTaskFinished(wcHandle client, wcTask tsk)
{
    cout << "Output stream closed" << endl;
    running = 6;
}

/* Callback. IO stream started. */
void onIOTaskStarted(wcHandle client, wcTask tsk)
{
    cout << "Output stream started" << endl;
    need_to_load_next_frame = true;
}

/* Callback. Task updated. */
void onTaskSynchronized(wcHandle client, wcTask tsk)
{
    wcTaskClass tcid = WC_BASE_TASK;
    /* Get the class for `tsk` */
    if (wcTaskGetClass(tsk, &tcid) == WC_OK) {
        if (tcid == WC_OUT_STREAM_TASK)
        /* The output stream task updated (the previous frame was sent)  */
            need_to_load_next_frame = true;
    }
}

class MyInputParser : public InputParser {
    public:
        using InputParser::InputParser;

        const std::string& getInput() {
            static const std::string CMD_INPUT("--input");
            static const std::string CMD_INPUT_SYN("-i");
            const std::string & res = getCmdOption(CMD_INPUT);
            if (res.empty()) {
                return getCmdOption(CMD_INPUT_SYN);
            } else
                return res;
        }

        virtual void printCommonHelp() override
        {
            InputParser::printCommonHelp();
            cout << " -i, --input      The input argument uses C printf format to represent a list of files," << endl;
            cout << "  i.e. file-%%06d.png to look for files file000001.png to file9999999.png" << endl;
        }
};

int main(int argc, char * argv[])
{
    /* Parse command line */
    MyInputParser input(argc, argv);
    const std::string &host = input.getHost();
    const std::string &proxy = input.getProxy();
    const std::string &user = input.getUserName();
    const std::string &pwrd = input.getUserPassword();
    std::string device = input.getDevice();
    if (device.empty()) device = "device_test001_output";
    std::string input_mask = input.getInput();
    if (input_mask.empty()) input_mask = "file-%%02d.png";

    if (input.showHelp()) {
        /* Show help doc */
        input.printCommonHelp();
        return 0;
    }

    /* dirname and basename must operate on scratch strings */
    char * dirc = strdup(input_mask.c_str());
    char * basec = strdup(input_mask.c_str());
    input_directory = std::string(dirname(dirc));
    input_filter = std::string(basename(basec));
    free(dirc);
    free(basec);

    total_frames = scandir(input_directory.c_str(), &png_files, include_files, alphasort);

    if (total_frames <= 0) {
        cerr << "no input files found; run with -h for help." << endl;
        return 1;
    }

    lst_timestamp = std::chrono::system_clock::now();

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
        /* Configure streaming callbacks */
        wcSetTaskCallback(client, wccbkAfterLaunchOutStream, onIOTaskStarted);
        wcSetTaskCallback(client, wccbkSynchroUpdateTask, onTaskSynchronized);
        wcSetTaskCallback(client, wccbkSuccessIOStream, onIOTaskFinished);

        /* Start client */
        wcClientStart(client);

        auto start_timestamp = std::chrono::system_clock::now();
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
                /* Start output stream */
                wcLaunchOutStream(client, "RAW_PNG", MAX_DELTA, NULL);
                running = 3;
                break;
            }
            default:
            {
                break;
            }
            }

            /* load next frame and push to the output data stack */
            if (running == 3) {
                if (load_next_frame(client)) return 1;
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            } else
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            /* Proceed client */
            wcClientProceed(client);
            wcClientTasksProceed(client);

            auto current_timestamp = std::chrono::system_clock::now();
            auto int_s = chrono::duration_cast<chrono::seconds>(current_timestamp - start_timestamp);

            if (int_s.count() >= TIME_OUT) {
                cout << "timeout" << endl;
                break;
            }
        }
        /* Disconnect client */
        wcClientDisconnect(client);
        /* Destroy client */
        wcClientDestroy(client);

        if (mem_frame_buffer)
            free(mem_frame_buffer);

        while (total_frames--) free(png_files[total_frames]);
        free(png_files);

        return 0;
    } else {
        cout << "should contain at least --host (host-name) option";
    }
}

