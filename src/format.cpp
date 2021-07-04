#include <string>

#include "format.h"

using std::string;
using std::to_string;

string Format::OutputString(string s, char c)
{
  s.insert(s.begin(), 2 - s.size(), c);
  return s;
}

string Format::ElapsedTime(long seconds) 
{
  const int hour = seconds / (3600);
  const int minute = (seconds / 60) % 60;
  const long sec = seconds % 60;

  return string(Format::OutputString(to_string(hour), '0') + ":" +
                Format::OutputString(to_string(minute), '0') + ":"+
                Format::OutputString(to_string(sec),'0'));
}