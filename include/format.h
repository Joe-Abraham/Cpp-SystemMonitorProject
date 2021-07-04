#ifndef FORMAT_H
#define FORMAT_H

#include <string>

using std::string;

namespace Format {
std::string ElapsedTime(long times);
std::string OutputString(string s, char c);
}; 

#endif