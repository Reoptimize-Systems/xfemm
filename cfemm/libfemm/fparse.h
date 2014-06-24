#include <string>

#ifndef FPARSE_H
#define FPARSE_H

namespace femm
{

// define an enum for the problem type
enum ProblemType { PLANAR, AXISYMMETRIC };

// define an enum for the coordinate system type of a problem
enum CoordsType { CART, POLAR };

// declare some functions used to parse files
char* StripKey(char *c);
char *ParseDbl(char *t, double *f);
char *ParseInt(char *t, int *f);
char *ParseString(char *t, std::string *s);
std::string string_format(const std::string &fmt, ...);

// declare a default warning message function
void PrintWarningMsg(const char* message);

}
#endif
