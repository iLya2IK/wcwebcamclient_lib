#ifndef TSINTEGER_H
#define TSINTEGER_H

#include <tsObject.h>

class tsInteger : public tsObject
{
    public:
        tsInteger(long value);
        long getValue();
        void setValue(long value);

    protected:

    private:
        long mValue;
};

#endif // TSINTEGER_H
