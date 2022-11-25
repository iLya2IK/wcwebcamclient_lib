#ifndef TSSTRINGLIST_H
#define TSSTRINGLIST_H

#include <wcDefines.h>

class tsStringList : public tsObject
{
    public:
        void add(std::string avalue);
        void clear();
        std::string getAt(int i);
        std::string pop();
        int size();
    protected:

    private:
        std::vector<std::string> vtr;
};

#endif // TSSTRINGLIST_H
