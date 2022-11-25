#ifndef TSCURLM_H
#define TSCURLM_H

#include <tsObject.h>
#include <curl/curl.h>

class tsCURLM : public tsObject {
  private:
    CURLM * mValue{NULL};
  public:
    virtual ~tsCURLM();
    void initCURLM();
    void doneCURLM();
    CURLM * getValue();
    CURLM * getValueUnsafe();
};

#endif // TSCURLM_H
