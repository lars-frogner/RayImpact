#pragma once
#include "precision.hpp"
#include <vector>
#include <string>
#include <ostream>
#include <cstdarg>

namespace Impact {

// String utility function declarations

void trim(std::string& s);
void trimLeft(std::string& s);
void trimRight(std::string& s);

std::vector<std::string> split(const std::string& s, char delim = ' ');

std::string join(const std::vector<std::string>& sequence,
                 const std::string& separator,
                 int start = 0, int end = -1);

std::string formatString(const char* format, ...);

} // Impact
