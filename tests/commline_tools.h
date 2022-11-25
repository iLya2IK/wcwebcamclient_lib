/*===============================================================*/
/* This is an example of how to use the wcWebCamClient library.  */
/* Simple class for parsing command line                         */
/* commandline_tools.h                                           */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/


#ifndef COMMLINE_TOOLS_H_INCLUDED
#define COMMLINE_TOOLS_H_INCLUDED

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

class InputParser{
    public:
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        /// @author iain
        const std::string& getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        /// @author iain
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }

        const std::string& getHost() {
            static const std::string CMD_HOST("--host");
            static const std::string CMD_HOST_SYN("-u");
            const std::string & res = getCmdOption(CMD_HOST);
            if (res.empty()) {
                return getCmdOption(CMD_HOST_SYN);
            } else
                return res;
        }

        const std::string& getProxy() {
            static const std::string CMD_PROXY("--proxy");
            static const std::string CMD_PROXY_SYN("-p");
            const std::string & res = getCmdOption(CMD_PROXY);
            if (res.empty()) {
                return getCmdOption(CMD_PROXY_SYN);
            } else
                return res;
        }

        const std::string& getDevice() {
            static const std::string CMD_DEVICE("--device");
            static const std::string CMD_DEVICE_SYN("-d");
            const std::string & res = getCmdOption(CMD_DEVICE);
            if (res.empty()) {
                return getCmdOption(CMD_DEVICE_SYN);
            } else
                return res;
        }

        const std::string& getUserName() {
            static const std::string CMD_USER_NAME("--name");
            static const std::string CMD_USER_NAME_SYN("-n");
            const std::string & res = getCmdOption(CMD_USER_NAME);
            if (res.empty()) {
                return getCmdOption(CMD_USER_NAME_SYN);
            } else
                return res;
        }

        const std::string& getUserPassword() {
            static const std::string CMD_USER_PWRD("--pwrd");
            static const std::string CMD_USER_PWRD_SYN("-x");
            const std::string & res = getCmdOption(CMD_USER_PWRD);
            if (res.empty()) {
                return getCmdOption(CMD_USER_PWRD_SYN);
            } else
                return res;
        }

        bool ignoreTSLCert() {
            static const std::string OPT_TSL_IGNORE("-k");
            return cmdOptionExists(OPT_TSL_IGNORE);
        }

        bool showHelp() {
            static const std::string OPT_SHOW_HELP("-h");
            return cmdOptionExists(OPT_SHOW_HELP);
        }

        void printCommonHelp() {
            if (tokens.size() > 0)
                std::cout << "Usage:" << tokens.at(0) << " [options...]" << std::endl;
            std::cout << " -u, --host       URL for server host in format https://hostname:port" << std::endl;
            std::cout << " -p, --proxy      Proxy in format [[username][:password]@][proxyurl:port]" << std::endl;
            std::cout << " -d, --device     Device name" << std::endl;
            std::cout << " -n, --name       Login name" << std::endl;
            std::cout << " -x, --pwrd       Login password" << std::endl;
            std::cout << " -k               Ignore TSL certificate errors" << std::endl;
            std::cout << " -h               Show this help" << std::endl;
        }
    private:
        std::vector <std::string> tokens;
};

#endif // COMMLINE_TOOLS_H_INCLUDED
