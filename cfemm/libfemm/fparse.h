#ifndef FEMM_FPARSE_H
#define FEMM_FPARSE_H

#include <string>
#include <iostream>
#include <algorithm>
#include <locale>

//#define DEBUG_PARSER
namespace femm
{


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
 * @param s the result string. May be null, will be cleared.
 */
void parseString(std::istream &input, std::string *s);
/**
 * @brief parseString reads a string literal (using the delimiter '"') from input.
 * If the next token of the input stream is not a delimiter,
 * the stream position and string \c s remain unchanged and an error message is printed to \p err.
 * If a string is found, the whole remaining line is consumed.
 *
 * @param input
 * @param s the result string. May be null, will be cleared.
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
int PrintWarningMsg(const char* message);

}
#endif
