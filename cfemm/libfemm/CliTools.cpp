#include "CliTools.h"

void femmutils::splitArg(const char cstr[], std::string &arg, std::string &value)
{
    bool isValue=false;
    for(const char *c=cstr; *c!=0 ; c++)
    {
        if (*c=='=')
            isValue = true;
        else if (isValue)
            value += *c;
        else
            arg += *c;
    }
}
