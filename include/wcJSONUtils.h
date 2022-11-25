#ifndef WCJSONUTILS_H_INCLUDED
#define WCJSONUTILS_H_INCLUDED

#include <wcJSON.h>

wcJSON json_parse(const char * aStr);
wcJSON jsonNewObject();
wcJSON jsonNewArray();

#endif // WCJSONUTILS_H_INCLUDED
