#include "fparse.h"

#include <algorithm>
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>

using namespace std;

namespace femm
{

char* StripKey(char *c)
{
    char *d;
    int i,k;

    k=strlen(c);

    for(i=0; i<k; i++)
    {
        if (c[i] == '=')
        {
            d=c+i+1;
            return d;
        }
    }

    return c+k;
}

char *ParseDbl(char *t, double *f)
{
    if (t==NULL) return NULL;

    int i,j,k,u,ws;
    static char w[]="\t, \n";
    char *v;

    k=strlen(t);
    if(k==0) return NULL;

    for(i=0,u=0,v=NULL; i<k; i++)
    {
        for(j=0,ws=0; j<4; j++)
        {
            if (t[i]==w[j])
            {
                ws=1;
                if (u==1) u=2;
            }
        }
        if ((ws==0) && (u==0)) u=1;
        if ((ws==0) && (u==2))
        {
            v=t+i;
            break;
        }
    }

    if (u==0) return NULL;	//nothing left in the string;
    if (v==NULL) v=t+k;

    sscanf(t,"%lf",f);

    return v;
}

char *ParseInt(char *t, int *f)
{
    if (t==NULL) return NULL;

    int i,j,k,u,ws;
    static char w[]="\t, \n";
    char *v;

    k=strlen(t);
    if(k==0) return NULL;

    for(i=0,u=0,v=NULL; i<k; i++)
    {
        for(j=0,ws=0; j<4; j++)
        {
            if (t[i]==w[j])
            {
                ws=1;
                if (u==1) u=2;
            }
        }
        if ((ws==0) && (u==0)) u=1;
        if ((ws==0) && (u==2))
        {
            v=t+i;
            break;
        }
    }

    if (u==0) return NULL;	//nothing left in the string;
    if (v==NULL) v=t+k;

    sscanf(t,"%i",f);

    return v;
}

char *ParseString(char *t, string *s)
{
    if (t==NULL) return NULL;
    if (std::strlen(t)==0) return t;

    int n1,n2,k;

    // find first quote in the source string
    for(k=0,n1=-1; k< (int) std::strlen(t); k++)
    {
        if (t[k]=='\"')
        {
            n1=k;
            break;
        }
    }

    if (n1<0) return t;

    // find second quote in the source string
    for(k=n1+1,n2=-1; k< (int) std::strlen(t); k++)
    {
        if (t[k]=='\"')
        {
            n2=k;
            break;
        }
    }

    if (n2<0) return t;

    *s=t;
    *s=s->substr(n1+1,n2-n1-1);

    return (t+n2+1);
}

// default function for displaying warning messages
void PrintWarningMsg(const char* message)
{
    printf("%s", message);
}

bool expectChar(istream &input, char c,  std::ostream &err)
{
    input >> std::ws;
    if ( input.peek() == c )
    {
        input.ignore();
        return true;
    }
    err << "Expected char code" << (int)c
        << "(" << c << "), but got " << input.peek();
    return false;
}

bool parseToken(std::istream &input, const std::string str, std::ostream &err)
{
    string token;

    nextToken(input, &token);
    if ( token != str )
    {
        err << "Expected token " <<str<< ", but got " <<token;
        return false;
    }
    return true;
}

void nextToken(istream &input, string *token)
{
    input >> *token;
    // transform token to lower case
    transform(token->begin(), token->end(), token->begin(), ::tolower);
}

void ParseString(istream &input, string *s)
{
    char c;
    // use dummy stream to suppress output
    std::stringstream dummy;
    if (!expectChar(input, '"', dummy))
    {
        // in some places the string is optional -> don't throw an error
        return;
    }

    input >> c;
    while( input.good() && c != '"')
    {
        // in case somebody just wants to skip the string literal, a NULL string may be passed
        if (s)
            s->push_back(c);
        input >> c;
    }
}

}
