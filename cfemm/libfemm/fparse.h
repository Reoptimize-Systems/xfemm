#ifndef FEMM_FPARSE_H
#define FEMM_FPARSE_H

#include <string>
#include <iostream>
#include <algorithm>
#include <locale>

//#define DEBUG_PARSER
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
char *parseString(char *t, std::string *s);
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
bool expectToken(std::istream &input, const std::string str, std::ostream &err = std::cerr);
void nextToken(std::istream &input, std::string *token);
/**
 * @brief parseString reads a string literal (using the delimiter '"') from input.
 * If the next token of the input stream is not a delimiter,
 * the stream position and string \c s remain unchanged.
 * Use this variant (the one without an \p err parameter) when a string is optional.
 * @param input
 * @param s a string.
 */
void parseString(std::istream &input, std::string *s);
/**
 * @brief parseString reads a string literal (using the delimiter '"') from input.
 * If the next token of the input stream is not a delimiter,
 * the stream position and string \c s remain unchanged and an error message is printed to \p err.
 * @param input
 * @param s
 * @param err
 * @return \c true, if a string literal was read, \c false otherwise.
 */
bool parseString(std::istream &input, std::string *s, std::ostream &err);
/**
 * @brief parseValue reads a double value from input.
 * All characters until the end of line are consumed.
 * @param input
 * @param val
 * @param err
 * @return \c true, if the conversion worked, \c false otherwise
 */
bool parseValue(std::istream &input, double &val, std::ostream &err = std::cerr);
/**
 * @brief parseValue reads an int value from input.
 * All characters until the end of line are consumed.
 * @param input
 * @param val
 * @param err
 * @return \c true, if the conversion worked, \c false otherwise
 */
bool parseValue(std::istream &input, int &val, std::ostream &err = std::cerr);
/**
 * @brief parseValue reads a bool value from input.
 * All characters until the end of line are consumed.
 * @param input
 * @param val
 * @param err
 * @return \c true, if the conversion worked, \c false otherwise
 */
bool parseValue(std::istream &input, bool &val, std::ostream &err = std::cerr);

// declare a default warning message function
void PrintWarningMsg(const char* message);

// taken from https://stackoverflow.com/posts/217605/revisions :
// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

}
#endif
