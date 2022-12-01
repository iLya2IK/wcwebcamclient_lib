/*===============================================================*/
/* Threadsafe wrapper for client settings                        */
/* wcHTTP2SettingsIntf.cpp                                       */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include "wcHTTP2SettingsIntf.h"
#include "wcConstsTypes.h"

using namespace std;

wcHTTP2SettingsIntf::wcHTTP2SettingsIntf()
{
    proxy_prot = "http://";
}

string wcHTTP2SettingsIntf::getSID()
{
    lock();
    string aStr = sid;
    unlock();
    return aStr;
}

string wcHTTP2SettingsIntf::getEncodedSID()
{
    lock();
    string aStr = encSid;
    unlock();
    return aStr;
}

string wcHTTP2SettingsIntf::getHost()
{
    lock();
    string aStr = host;
    unlock();
    return aStr;
}

string wcHTTP2SettingsIntf::getDevice()
{
    lock();
    string aStr = device;
    unlock();
    return aStr;
}

string wcHTTP2SettingsIntf::getMetaData()
{
    lock();
    string aStr = meta;
    unlock();
    return aStr;
}

void wcHTTP2SettingsIntf::setSID(const std::string & aSID)
{
    lock();
    sid = aSID;
    encSid = encodeHTTP(aSID);
    unlock();
}

void wcHTTP2SettingsIntf::setHost(const std::string & aHost)
{
    lock();
    host = aHost;
    unlock();
}

void wcHTTP2SettingsIntf::setDevice(const std::string & aDevice)
{
    lock();
    device = aDevice;
    unlock();
}

void wcHTTP2SettingsIntf::setProxy(const std::string& aValue)
{
    lock();
    decodeProxy(aValue, proxy_prot, proxy_host, proxy_port, proxy_enc_aname, proxy_enc_apass);
    unlock();
}

void wcHTTP2SettingsIntf::setProxyProt(const std::string& aValue)
{
    lock();
    proxy_prot = aValue;
    unlock();
}

void wcHTTP2SettingsIntf::setProxyHost(const std::string& aValue)
{
    lock();
    proxy_host = aValue;
    unlock();
}

void wcHTTP2SettingsIntf::setProxyPort(const std::string& aValue)
{
    lock();
    proxy_port = aValue;
    unlock();
}

void wcHTTP2SettingsIntf::setProxyAuthName(const std::string& aValue)
{
    lock();
    proxy_enc_aname = encodeHTTP(aValue);
    unlock();
}

void wcHTTP2SettingsIntf::setProxyAuthPass(const std::string& aValue)
{
    lock();
    proxy_enc_apass = encodeHTTP(aValue);
    unlock();
}

void wcHTTP2SettingsIntf::setMetaData(const std::string& aMeta)
{
    lock();
    meta = aMeta;
    unlock();
}

bool wcHTTP2SettingsIntf::hasMetaData()
{
    lock();
    bool aRes = !meta.empty();
    unlock();
    return aRes;
}

string wcHTTP2SettingsIntf::getProxyStr()
{
    lock();
    string aStr (proxy_prot + proxy_host);
    if (!proxy_port.empty()) {
        aStr += ":" + proxy_port;
    }
    unlock();
    return aStr;
}

string wcHTTP2SettingsIntf::getProxyAuth()
{
    lock();
    string aStr (proxy_enc_aname + ":" + proxy_enc_apass);
    unlock();
    return aStr;
}

bool wcHTTP2SettingsIntf::hasProxy() {
    lock();
    bool res = (!proxy_host.empty());
    unlock();
    return res;
}

bool wcHTTP2SettingsIntf::hasProxyAuth() {
    lock();
    bool res = (!proxy_enc_aname.empty());
    unlock();
    return res;
}

void wcHTTP2SettingsIntf::setVerifyTLS(bool aValue)
{
    lock();
    verify_tsl = aValue;
    unlock();
}

bool wcHTTP2SettingsIntf::getVerifyTLS()
{
    lock();
    bool res = verify_tsl;
    unlock();
    return res;
}
