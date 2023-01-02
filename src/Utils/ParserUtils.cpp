#include <string>
#include "ParserUtils.h"

std::string trim(const std::string &s) {
    int start = 0;
    while (s[start] == ' ') start++;
    int end = s.size() - 1;
    while (s[end] == ' ') end--;
    return s.substr(start, end - start + 1);
}

std::string addSpacesBetweenChars(const std::string &s) {
    std::string t;
    for (char c: s) {
        t.push_back(c);
        t.push_back(' ');
    }
    t.pop_back();
    return t;
}