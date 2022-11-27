/*===============================================================*/
/* Set of helpfull functions                                     */
/* wcUtils.cpp                                                   */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*/

#include <wcConstsTypes.h>
#include <set>
#include <exception>
#include <cstring>
#include <algorithm>
#include <cctype>

std::string vstr_format(const char * format, va_list args)
{
    size_t size = (strlen(format) + (size_t)0x400 + (size_t)0x3FF) & ~((size_t)0x3FF);
    char* buf = new char[ size ];
    size = std::vsnprintf( buf, size, format, args );
    std::string res( buf, buf + size );
    delete [] buf;
    return res;
}

std::string str_format(const char * format, ...)
{
    va_list args;
    va_start(args, format);
    std::string res = vstr_format(format, args);
    va_end(args);
    return res;
}

bool is_number(const std::string &s) {
  return !s.empty() && std::find_if(s.begin(),
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

void decodeProxy(const std::string & aStr,
                 std::string & aProtocol,
                 std::string & aHost, std::string & aPort, std::string & aUName, std::string & aPwrd)
{
    int L = aStr.size();

    static const std::string empty_str ("");
    if (L == 0) {
        aHost = empty_str;
        aPort = empty_str;
        aUName = empty_str;
        aPwrd = empty_str;
    }

    std::vector<std::string> parts;

    size_t unpos = 0;
    char* res = (char*)malloc(L+1);
    char * R = res;
    const char * S = aStr.c_str();
    const char * SS = S;
    while ( (S - SS) < L )
    {
        if ((*S == ':') || (*S == '@')) {
            *R = 0;
            parts.push_back(std::string(res));
            if (*S == '@') unpos = parts.size();
            R = res;
        }
        else {
            *R = *S;
            R++;
        }

        S++;
    }
    if ((R - res) > 0) {
        *R = 0;
        parts.push_back(std::string(res));
    }

    switch (unpos) {
        case 1: {
            aUName = parts.at(0);
            aPwrd = empty_str;
            break;
        }
        case 2: {
            aUName = parts.at(0);
            aPwrd = parts.at(1);
            break;
        }
        default : {
            aUName = empty_str;
            aPwrd = empty_str;
            unpos = 0;
            break;
        }
    }
    size_t address_len = parts.size() - unpos;

    switch (address_len) {
        case 1 : {
            aHost = parts.at(parts.size()-1);
            aPort = empty_str;
            break;
        }
        case 2 :
        case 3 : {
            if (is_number(parts.at(parts.size()-1))) {
                aPort = parts.at(parts.size()-1);
                address_len--;
            } else {
                aPort = empty_str;
            }
            if (address_len > 1) {
                aProtocol = std::string(parts.at(unpos) + "://");
                aHost = parts.at(unpos + 1);
                while (!aHost.empty() && (aHost.at(0) == '/'))
                    aHost.erase(0, 1);
            } else {
                aHost = parts.at(unpos);
            }
            break;
        }
        default : {
            aHost = empty_str;
            aPort = empty_str;
            break;
        }
    }
    free(res);
}

std::string encodeHTTP(const std::string & aStr)
{
    static const std::set<char> HTTPAllowed = { 'A','B','C','D','E','F','G','H','I','K','L',
                                                'M','N','O','P','Q','R','S','T','V','X','Y','Z',
                                                'a','b','c','d','e','f','g','h','i','k','l',
                                                'm','n','o','p','q','r','s','t','v','x','y','z',
                                                '*', '@', '.', '_', '-',
                                                '0','1','2','3','4','5','6','7','8','9', '$',
                                                '!', '\'', '(', ')' };

    static const char hexV[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    int L = aStr.size();

    if (L == 0)
      return std::string("");

    char * res = (char*)malloc(L * 3);
    char * R = res;
    const char * S = aStr.c_str();
    const char * SS = S;
    while ( (S - SS) < L )
    {
        auto search = HTTPAllowed.find(*S);
        if ( search != HTTPAllowed.end() )
            *R = *S;
        else
        {
            if (*S == ' ')
                *R = '+';
            else
            {
                *R = '%';
                uint8_t v = (uint8_t)(*S);
                R++;
                *R = hexV[(v >> 4) & 0x0F];
                R++;
                *R = hexV[v & 0x0F];
            }
        }
        R++;
        S++;
    }
    res[R - res] = 0;
    std::string sres(res);
    free(res);
    return sres;
}
