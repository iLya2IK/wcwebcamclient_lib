#ifndef WCHTTP2SETTINGSINTF_H
#define WCHTTP2SETTINGSINTF_H

#include "tsObject.h"
#include <string>

class wcHTTP2SettingsIntf : public tsObject
{
    public:
        wcHTTP2SettingsIntf();
        void setMetaData(const std::string & aMeta);
        void setDevice(const std::string & aDevice);
        void setHost(const std::string & aHost);
        void setSID(const std::string & aSID);
        void setProxy(const std::string & aValue);
        void setProxyProt(const std::string & aValue);
        void setProxyHost(const std::string & aValue);
        void setProxyPort(const std::string & aValue);
        void setProxyAuthName(const std::string & aValue);
        void setProxyAuthPass(const std::string & aValue);

        bool hasMetaData();

        void setVerifyTSL(bool aValue);
        bool getVerifyTSL();

        bool hasProxy();
        bool hasProxyAuth();
        std::string getProxyStr();
        std::string getProxyAuth();

        std::string getMetaData();
        std::string getDevice();
        std::string getHost();
        std::string getSID();
        std::string getEncodedSID();
    private:
        std::string sid, encSid, host, device, meta;
        std::string proxy_prot, proxy_host, proxy_port, proxy_enc_aname, proxy_enc_apass;
        bool verify_tsl {true};
};

#endif // WCHTTP2SETTINGSINTF_H
