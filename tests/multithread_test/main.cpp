/*! \file main.cpp
 *  A simple test program to demonstrate how to send and get
 *  messages
 */

/*===============================================================*/
/* This is an example of how to use the wcWebCamClient library.  */
/* This example emulates a simple chat.                          */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include <iostream>
#include <fstream>
#include <curses.h>
#include <wcwebcamclient.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <wcJSON.h>
#include <wcThread.h>
#include "../commline_tools.h"

using namespace std;

static uint8_t running = 0;

static const string EMPTY_STR("");
static const string cMSG("msg");
static const string cDEVICE("device");
static const string cTARGET("target");

#define WORKING (running < 10)
#define STOP_EXECUTION running = 10

void CHECK(wcRCode res) {
    if (res != WC_OK) STOP_EXECUTION;
}

#define CHAT_WIN_COLOR 1
#define LOG_WIN_COLOR 2
#define DEV_WIN_COLOR 3
#define INP_WIN_COLOR 4
#define DEV_SEL_COLOR 5
#define INP_SEL_COLOR 6
#define CHAT_MSG_COLOR 7
#define CHAT_IMSG_COLOR 8
#define CHAT_OMSG_COLOR 9

static const char * WIN_CHAT_TITLE = "Chat";
static const char * WIN_LOG_TITLE = "Log";
static const char * WIN_DEVS_TITLE = "Devices [UP]/[DOWN]";
static const char * WIN_INP_TITLE = "Input message here, [Enter] to send to target device";

WINDOW * win_chat_input = NULL;
WINDOW * win_device_list = NULL;
WINDOW * win_log = NULL;
WINDOW * win_chat = NULL;

static int logHeight = 0;
static int logWidth = 0;
static int devHeight = 0;
static int devWidth = 0;
static int inpWidth = 0;
static int chatHeight = 0;
static int chatWidth = 0;
static int msgWidth = 0;

static vector<string> log;
static vector<string> devices;
static string input_str("");
static int cursor_pos = 0;
static int input_offset = 0;
static string selected_device("");
const char  blank_line[] = "                               "
                           "                               "
                           "                               "
                           "                               "
                           "                               "
                           "                               ";

static bool log_updated = false;
static bool dev_updated = false;
static bool msg_updated = false;

/* Class to store input/output msgs */
class wcMessage {
private:
    wcJSON * msg {NULL};
public:
    wcMessage(const wcMessage & aMsg) {
        msg = new wcJSON(*(aMsg.msg));
    }

    explicit wcMessage(const char * obj) {
        msg = new wcJSON(obj);
    }
    wcMessage(const char * aMsg, const char * aDevice, const char * aTarget) {
        msg = new wcJSON(jsonObject, NULL);

        if (aMsg) msg->addString(cMSG.c_str(), aMsg);
        if (aTarget) msg->addString(cTARGET.c_str(), aTarget);
        if (aDevice) msg->addString(cDEVICE.c_str(), aDevice);
    }
    wcMessage(const char * aMsg, const char * aTarget) {
        msg = new wcJSON(jsonObject, NULL);

        if (aMsg) msg->addString(cMSG.c_str(), aMsg);
        if (aTarget) msg->addString(cTARGET.c_str(), aTarget);
    }
    ~wcMessage() {
        if (msg) free(msg);
    }
    void addDevice(const string & aDevice) {
        msg->addString(cDEVICE.c_str(), aDevice.c_str());
    }
    string getMessage() {
        wcJSON m = msg->getObjItem(cMSG.c_str());
        if (m.isValid()) {
            return string(m.valueString());
        }
        return EMPTY_STR;
    }
    string getTarget() {
        wcJSON m = msg->getObjItem(cTARGET.c_str());
        if (m.isValid()) {
            return string(m.valueString());
        }
        return EMPTY_STR;
    }
    string getDevice() {
        wcJSON m = msg->getObjItem(cDEVICE.c_str());
        if (m.isValid()) {
            return string(m.valueString());
        }
        return EMPTY_STR;
    }
    string toJson() { return msg->toString(); }
};

static vector<wcMessage> wcmessages;

/* Get the position of selected device in the device list */
int devicePos() {
    for (int i = 0; i < (int)devices.size(); i++) {
        if (selected_device.compare(devices[i]) == 0) {
            return i;
        }
    }
    return -1;
}

/* Get the previous device in the list */
void prevDevice() {
    if (devices.size() == 0) return;

    int k = devicePos() - 1;
    if (k < 0)
        k = devices.size()-1;
    selected_device = devices[k];
    dev_updated = true;
}

/* Get the next device in the list */
void nextDevice() {
    if (devices.size() == 0) return;

    int k = devicePos() + 1;
    if (k >= (int)devices.size())
        k = 0;
    selected_device = devices[k];
    dev_updated = true;
}

/* Add a new line in the log  */
void addToLog(const string & val) {
    log.push_back(val);
    log_updated = true;
}

/* Refresh the chat window */
void updateChat(const string & myDevice) {
    msg_updated = false;

    vector<string> msg_lines;
    int k = chatHeight;

    for (int i = (int)wcmessages.size()-1; i >= 0; i--) {
        if (k > 0) {
            wcMessage & msg = wcmessages.at(i);

            msg_lines.clear();
            int dir = 0;

            if (myDevice.compare(msg.getDevice()) == 0) {
                string v = msg.getTarget();
                if ((myDevice.compare(v) == 0) || (v.empty()))
                    msg_lines.push_back(string(myDevice + string("--") + myDevice));
                else {
                    msg_lines.push_back(string(myDevice + string("->") + v));
                    dir = 1;
                }
            } else {
                msg_lines.push_back(string(myDevice + string("<-") + msg.getDevice()));
                dir = 2;
            }

            int max_len = 0;
            string msg_text = msg.getMessage();
            max_len = (int) msg_text.size();
            while (max_len > msgWidth) {
                string new_line("");

                int last_sep = 0;
                int total_len = 0;

                for (int j = 0; j < (int)msg_text.size();j++) {
                    char chr = msg_text.at(j);

                    if ((chr == ' ') || (chr == ':') || (chr == ',') || (chr == '.') || (chr == ';')) {
                        last_sep = j;
                    }
                    total_len++;

                    if (total_len > msgWidth) {
                        if (last_sep > 0 ) break;
                    }
                }
                if (total_len > msgWidth) {
                    if (last_sep == 0) {
                        new_line = msg_text.substr(0, msgWidth - 3);
                        new_line.append("...");
                        msg_text = EMPTY_STR;
                    } else {
                        new_line = msg_text.substr(0, last_sep + 1);
                        msg_text.erase((size_t)0, (size_t)(last_sep + 1));
                    }
                    msg_lines.push_back(new_line);
                } else {
                    msg_lines.push_back( msg_text );
                    msg_text = EMPTY_STR;
                }
                max_len = (int) msg_text.size();
            }

            if (msg_text.size() > 0) {
                msg_lines.push_back( msg_text );
            }

            max_len = 0;
            for (int j = 0; j < (int)msg_lines.size(); j++) {
                if ((int)msg_lines.at(j).size() > max_len) {
                    max_len = (int)msg_lines.at(j).size();
                }
            }

            int x = 0;
            int w = (chatWidth + 2);
            switch (dir) {
                case 0 : {x = (w - max_len)  / 2;
                          break;}
                case 1 : {x = 2;
                          break;}
                case 2 : {x = w - max_len - 2;
                          break;}
            }

            for (int j = ((int)msg_lines.size()-1); j >= 0; j--) {
                int sz = (int) msg_lines[j].size();
                if (k > 0) {
                    wattrset(win_chat, COLOR_PAIR(CHAT_WIN_COLOR));
                    mvwprintw(win_chat, k, 1, "%.*s", chatWidth, blank_line);
                    switch (dir) {
                        case 0 : {wattrset(win_chat, COLOR_PAIR(CHAT_MSG_COLOR));
                                  break;}
                        case 1 : {wattrset(win_chat, COLOR_PAIR(CHAT_OMSG_COLOR));
                                  break;}
                        case 2 : {wattrset(win_chat, COLOR_PAIR(CHAT_IMSG_COLOR));
                                  break;}
                    }
                    mvwprintw(win_chat, k, x, "%s", msg_lines[j].c_str());
                    mvwprintw(win_chat, k, x + sz, "%.*s", max_len - sz, blank_line);
                    k--;
                } else
                  break;
            }
            if (k > 0) {
                wattrset(win_chat, COLOR_PAIR(CHAT_WIN_COLOR));
                mvwprintw(win_chat, k, 1, "%.*s", chatWidth, blank_line);
                k--;
            }
        } else
          break;
    }
    wattrset(win_chat, COLOR_PAIR(CHAT_WIN_COLOR));
    for (int i = k; i >= 1; i--)
        mvwprintw(win_chat, k, 1, "%.*s", chatWidth, blank_line);

    wrefresh(win_chat);
}

/* Refresh the input text prompt window */
void updateInput() {
    int k = input_offset;
    wattrset(win_chat_input, COLOR_PAIR(INP_WIN_COLOR));
    for (int i = k; i < (cursor_pos); i++) {
        mvwaddch(win_chat_input, 1, (i - input_offset) + 1, input_str.at(i) );
    }
    wattrset(win_chat_input, COLOR_PAIR(INP_SEL_COLOR));
    if ((int)input_str.size() > cursor_pos)
      mvwaddch(win_chat_input, 1, (cursor_pos - input_offset) + 1, input_str.at(cursor_pos) );
    else
      mvwaddch(win_chat_input, 1, (cursor_pos - input_offset) + 1, ' ' );

    wattrset(win_chat_input, COLOR_PAIR(INP_WIN_COLOR));
    int last = input_str.size();
    if ((last - input_offset) > inpWidth) last = inpWidth + input_offset;
    for (int i = (cursor_pos + 1); i < last; i++) {
        mvwaddch(win_chat_input, 1, (i - input_offset) + 1, input_str.at(i) );
    }
    k = input_str.size();
    if (k <= cursor_pos) k = cursor_pos + 1;
    for (int i = (k - input_offset); i <= inpWidth; i++) {
        mvwaddch(win_chat_input, 1, i + 1, ' ');
    }
    wrefresh(win_chat_input);
}

/* Refresh the log window */
void updateLog() {
    log_updated = false;
    int lst = log.size()-1;
    int st = lst - logHeight + 1;
    if (st < 0) st = 0;
    int k = 1;
    for (int i = st; i <= lst; i++) {
        mvwprintw(win_log, k, 1, "%.*s", logWidth, log[i].c_str());
        k++;
    }
    wrefresh(win_log);
}

/* Send the new message to selected device */
void send(wcHandle client, const string & aDevice) {
    if (input_str.empty()) return;

    wcMessage msg (input_str.c_str(), selected_device.c_str());
    CHECK(wcSendMsg(client, msg.toJson().c_str(), (void*)aDevice.c_str()));
    msg.addDevice(aDevice);
    wcmessages.push_back(msg);
    msg_updated = true;

    input_str = EMPTY_STR;
    cursor_pos = 0;
    input_offset = 0;
    updateInput();
}

/* Clean up ncurses */
void removeWindows() {
    if (win_chat_input) delwin(win_chat_input);
    if (win_device_list) delwin(win_device_list);
    if (win_log) delwin(win_log);
    if (win_chat) delwin(win_chat);
}

/* Rebuild ncurses windows */
int rebuildWindows() {
    int h, w;
    getmaxyx(stdscr, h, w);

    removeWindows();

    logHeight = h  / 3 - 2;
    logWidth = w / 2 - 3;
    devHeight = logHeight;
    devWidth = logWidth;
    inpWidth = w - 3;

    chatWidth = w - 2;
    chatHeight = h * 2 / 3 - 5;
    msgWidth = chatWidth * 3 / 4;

    win_device_list = newwin(h / 3, w / 2, 0, 0);
    win_chat = newwin(chatHeight + 2, w, h / 3, 0);
    win_log = newwin(logHeight + 2, logWidth + 3, 0, w / 2);
    win_chat_input = newwin(3, w, h - 3, 0);

    if ( !win_device_list ||  !win_chat || !win_log || !win_chat_input ) {
        return 1;
    }
    init_pair(CHAT_WIN_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(CHAT_MSG_COLOR, COLOR_BLACK, COLOR_WHITE);
    init_pair(CHAT_IMSG_COLOR, COLOR_YELLOW, COLOR_BLUE);
    init_pair(CHAT_OMSG_COLOR, COLOR_RED, COLOR_CYAN);
    wbkgd(win_chat, COLOR_PAIR(CHAT_WIN_COLOR));
    box(win_chat, 0, 0);
    mvwprintw(win_chat, 0, (chatWidth - strlen(WIN_CHAT_TITLE))/2 + 1, WIN_CHAT_TITLE);
    wrefresh(win_chat);

    init_pair(INP_WIN_COLOR, COLOR_BLACK, COLOR_WHITE);
    init_pair(INP_SEL_COLOR, COLOR_WHITE, COLOR_BLACK);
    wbkgd(win_chat_input, COLOR_PAIR(INP_WIN_COLOR));
    box(win_chat_input, 0, 0);
    w = strlen(WIN_INP_TITLE);
    if (w > inpWidth) w = inpWidth;
    mvwprintw(win_chat_input, 0, 1, "%.*s", w, WIN_INP_TITLE);
    wrefresh(win_chat_input);
    nodelay(win_chat_input, FALSE);
    notimeout(win_chat_input, FALSE);
    wtimeout(win_chat_input, 250);
    keypad(win_chat_input, TRUE);

    init_pair(DEV_WIN_COLOR, COLOR_YELLOW, COLOR_BLUE);
    init_pair(DEV_SEL_COLOR, COLOR_BLACK, COLOR_WHITE);
    wbkgd(win_device_list, COLOR_PAIR(DEV_WIN_COLOR));
    box(win_device_list, 0, 0);
    mvwprintw(win_device_list, 0, (devWidth - strlen(WIN_DEVS_TITLE))/2 + 1, WIN_DEVS_TITLE);
    wrefresh(win_device_list);

    init_pair(LOG_WIN_COLOR, COLOR_RED, COLOR_CYAN);
    wbkgd(win_log, COLOR_PAIR(LOG_WIN_COLOR));
    box(win_log, 0, 0);
    mvwprintw(win_log, 0, (logWidth - strlen(WIN_LOG_TITLE))/2 + 1, WIN_LOG_TITLE);
    wrefresh(win_log);

    return 0;
}

/* Refresh the device list window */
void updateDevices() {
    dev_updated = false;
    int lst = devices.size()-1;
    int st = lst - devHeight + 1;
    if (st < 0) st = 0;
    int k = 1;
    for (int i = st; i <= lst; i++)
    {
        if (selected_device.compare(devices[i]) == 0)
            wattrset(win_device_list, COLOR_PAIR(DEV_SEL_COLOR));

        mvwprintw(win_device_list, k, 1, "%.*s", devWidth, blank_line);
        mvwprintw(win_device_list, k, 1, "%.*s", devWidth, devices[i].c_str());

        wattrset(win_device_list, COLOR_PAIR(DEV_WIN_COLOR));
        k++;
    }
    wrefresh(win_device_list);
}

/* Callback. Authorization was successful. */
void onAuth(wcHandle self, wcTask task)
{
    char * res = NULL;
    /* Get a new session id and display it on the screen. */
    if (wcClientGetStrValue(self, wcstSID, &res) == WC_OK) {
        if (res) {
            addToLog(string(res));
            free(res);
            addToLog("Client successfully connected");
        }
    }
    running = 2;
}

/* Callback. The client has been disabled. */
void onDisconnect(wcHandle self, wcTask task)
{
    addToLog(string("Client disconnected"));
    STOP_EXECUTION;
}

/* Callback. Log was changed. */
void onLog(wcHandle self, const char * str)
{
    /* Display new log entry. */
    addToLog(string(str));
}

/* Callback. A new list of devices has arrived. */
void getDevices(wcHandle self, wcTask tsk, const char * json)
{
    devices.clear();
    devices.push_back("");
    dev_updated = true;
    wcJSON jarr(json);
    if (jarr.isArray()) {
        for (int i = 0; i < jarr.length(); i++) {
            wcJSON obj = jarr.getArrayItem(i);
            if (obj.isObject()) {
                wcJSON name = obj.getObjItem(cDEVICE.c_str());
                if (name.isString()) {
                    devices.push_back(name.valueString());
                }
            }
        }
    }
}

/* Callback. A new list of messages has arrived. */
void getMsgs(wcHandle self, wcTask tsk, const char * json)
{
    char * trg = NULL;
    CHECK(wcTaskGetUserData(tsk, (void**)&trg));

    msg_updated = true;
    wcJSON jarr(json);
    if (jarr.isArray()) {
        for (int i = 0; i < jarr.length(); i++) {
            wcJSON obj = jarr.getArrayItem(i);
            if (obj.isObject()) {
                wcJSON name = obj.getObjItem(cDEVICE.c_str());
                wcJSON jmsg = obj.getObjItem(cMSG.c_str());
                if (name.isString() && jmsg.isString()) {
                    wcMessage nmsg(jmsg.valueString(), name.valueString(), trg);
                    wcmessages.push_back(nmsg);
                }
            }
        }
    }
}

class MyIdleThread : public wcThread {
private:
    wcHandle mClient;
    int devices_counter;
    int msgs_counter;
    bool mActive {false};
    void CHECK_THREAD(wcRCode res) {
        if (res != WC_OK) terminate();
    }
public:
    explicit MyIdleThread(wcHandle client) : devices_counter (0), msgs_counter (0) {
        mClient = client;
    }
    virtual void execute() override {
        mActive = true;
        while (!terminated()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            devices_counter++;
            msgs_counter++;
            /* Proceed client */
            if (devices_counter >= 10) {
                CHECK_THREAD(wcClientInvalidateState(mClient, wcstDevices));
                devices_counter = 0;
            }
            if (msgs_counter >= 5) {
                if (!terminated()) CHECK_THREAD(wcClientInvalidateState(mClient, wcstMsgs));
                msgs_counter = 0;
            }
            if (!terminated()) CHECK_THREAD(wcClientProceed(mClient));
        }
        mActive = false;
    };
    bool isActive() {return mActive;}
};

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

        initscr();
        cbreak();
        noecho();
        nodelay(stdscr, TRUE);
        keypad(stdscr, TRUE);
        curs_set(0);
        start_color();

        rebuildWindows();

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
        wcSetNotifyCallback(client, wccbkDisconnect, onDisconnect);

        /* Configure callbacks to work with devices and messages */
        wcSetJSONStrCallback(client, wccbkSuccessUpdateDevices, getDevices);
        wcSetJSONStrCallback(client, wccbkSuccessUpdateMsgs, getMsgs);

        /* Start client */
        CHECK(wcClientStart(client));

        MyIdleThread idlethread(client);

        while (WORKING)
        {
            if (running == 2) {
                int ch;
                if ((ch = wgetch(win_chat_input)) != ERR) {
                    switch (ch) {
                        case 27: {
                            STOP_EXECUTION;
                            break;}
                        case KEY_LEFT: {
                           if (cursor_pos > 0) {
                              cursor_pos--;
                              if (cursor_pos < input_offset) input_offset--;
                           }
                           updateInput();
                           break;}
                        case KEY_RIGHT: {
                           if (cursor_pos < (int)input_str.size()) {
                              cursor_pos++;
                              if ((cursor_pos - input_offset) > inpWidth) input_offset++;
                           }
                           updateInput();
                           break;}
                        case KEY_UP: {
                           prevDevice();
                           break;}
                        case KEY_DOWN: {
                           nextDevice();
                           break;}
                        case KEY_BACKSPACE: {
                           if (cursor_pos > 0) {
                               input_str.erase(cursor_pos-1, 1);
                               cursor_pos--;
                               if (cursor_pos < input_offset) input_offset--;
                           }
                           updateInput();
                           break;}
                        case KEY_DC: {
                           if (cursor_pos < (int)input_str.size())
                               input_str.erase(cursor_pos, 1);
                           updateInput();
                           break;}
                        case KEY_ENTER:
                        case '\n': {
                           send(client, device);
                           break;}
                        default: {
                            if (isprint((char)ch)) {
                                input_str.insert(input_str.begin()+(size_t)(cursor_pos), (char)ch);
                                cursor_pos++;
                                if ((cursor_pos - input_offset) > inpWidth) input_offset++;
                                updateInput();
                            }
                        }
                    }
                }
            } else {
                if (running == 0) {
                    /* Authorize client */
                    CHECK(wcClientSetStrValue(client, wcstDeviceName, device.c_str()));
                    CHECK(wcClientAuth(client, user.c_str(), pwrd.c_str()));
                    idlethread.start();
                    running = 1;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }

            /* Proceed client (synchro step) */
            wcClientTasksProceed(client);

            if (log_updated) updateLog();
            if (dev_updated) updateDevices();
            if (msg_updated) updateChat(device);
        }
        if (idlethread.isActive()) {
            idlethread.terminate();
            idlethread.join();
        }

        /* Disconnect client */
        wcClientDisconnect(client);
        /* Destroy client */
        wcClientDestroy(client);

        removeWindows();
        refresh();
        endwin();

        return 0;
    } else {
        cout << "should contain at least --host (host-name) option";
    }
}
