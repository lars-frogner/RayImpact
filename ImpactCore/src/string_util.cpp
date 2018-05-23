#include "string_util.hpp"
#include "error.hpp"
#include <vector>
#include <algorithm>
#include <cctype>
#include <locale>
#include <iterator>
#include <sstream>
#include <cstdio>

namespace Impact {

// String utility function definitions

void trim(std::string& s)
{
    trimLeft(s);
    trimRight(s);
}

void trimLeft(std::string& s)
{
    s.erase(s.begin(),
            std::find_if(s.begin(),
                         s.end(),
                         [](int ch){ return !std::isspace(ch); }));
}

void trimRight(std::string& s)
{
    s.erase(std::find_if(s.rbegin(),
                         s.rend(),
                         [](int ch) { return !std::isspace(ch); }).base(),
            s.end());
}

template<typename Out>
void split(const std::string& s, char delim, Out result)
{
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (!item.empty())
            *(result++) = item;
    }
}

std::vector<std::string> split(const std::string& s, char delim /* = ' '*/)
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string join(const std::vector<std::string>& sequence,
                 const std::string& separator,
                 int start /* = 0 */, int end /* = -1 */)
{
    int sequence_length = (int)(sequence.size());

    if (end < 0)
        end += sequence_length;

    imp_assert(end >= start && start >= 0 && end < sequence_length);

    if (sequence_length == 0)
        return std::string();

    std::string concatenated = sequence[start];

    for (int idx = start + 1; idx <= end; idx++)
    {
        concatenated += separator + sequence[idx];
    }

    return concatenated;
}

std::string formatString(const char* format, ...)
{
    va_list arguments;
    char buffer[256];

    va_start(arguments, format);
    int n_chars_written = vsprintf_s(buffer, 256, format, arguments);
    va_end(arguments);

    imp_assert(n_chars_written < 256);

    return std::string(buffer);
}

} // Impact
