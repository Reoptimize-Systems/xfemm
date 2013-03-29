#include <string>

using namespace std;

#ifndef FPARSE_H
#define FPARSE_H

// define an enum for the problem type
enum ProblemType { PLANAR, AXISYMMETRIC };

// define an enum for the coordinate system type of a problem
enum CoordsType { CART, POLAR };

char* StripKey(char *c);
char *ParseDbl(char *t, double *f);
char *ParseInt(char *t, int *f);
char *ParseString(char *t, string *s);
std::string string_format(const std::string &fmt, ...);

#endif
