#ifndef FEMM_FPARSE_H
#define FEMM_FPARSE_H

#include <string>
#include <iostream>
namespace femm
{

// define an enum for the problem type
enum ProblemType { PLANAR = 0, AXISYMMETRIC = 1};

// define an enum for the coordinate system type of a problem
enum CoordsType { CART = 0, POLAR = 1 };

// Unit "lfac" for lengths
enum LengthUnit {
    LengthInches = 0,
    LengthMillimeters = 1,
    LengthCentimeters = 2,
    LengthMeters = 3,
    LengthMils = 4,
    LengthMicrometers = 5
};

// Conversion table to meters:
const double LengthConv[6] =
{
    0.0254,   //inches
    0.001,    //millimeters
    0.01,     //centimeters
    1.,       //meters
    2.54e-05, //mils
    1.e-06   //micrometers
};

// declare some functions used to parse files
char* StripKey(char *c);
char *ParseDbl(char *t, double *f);
char *ParseInt(char *t, int *f);
char *ParseString(char *t, std::string *s);
std::string string_format(const std::string &fmt, ...);
bool expectChar(std::istream &input, const char c, std::ostream &err = std::cerr);
/**
 * @brief expectToken reads a string token from input
 * Note: the token is expected to be lowercase.
 * @param input
 * @param str the lowercase token
 * @param err output stream for error messages
 * @return \c true, if the token was found, \c false otherwise
 */
bool parseToken(std::istream &input, const std::string str, std::ostream &err = std::cerr);
void nextToken(std::istream &input, std::string *token);
/**
 * @brief ParseString
 * Read a string literal (using the delimiter '"') from input.
 * If the next token of the input stream is not a delimiter,
 * the stream position and string \c s remain unchanged.
 * @param input
 * @param s a string.
 */
void ParseString(std::istream &input, std::string *s);

// declare a default warning message function
void PrintWarningMsg(const char* message);

}
#endif
