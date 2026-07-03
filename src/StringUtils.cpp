#include "StringUtils.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace StringUtils {

std::string trim(const std::string& s) {
    size_t start = 0;
    size_t end = s.size();
    while (start < end && std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }
    return s.substr(start, end - start);
}

std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

bool containsCaseInsensitive(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    std::string h = toLower(haystack);
    std::string n = toLower(needle);
    return h.find(n) != std::string::npos;
}

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) {
        fields.push_back(trim(field));
    }
    return fields;
}

bool tryParseInt(const std::string& s, int& outValue) {
    std::string t = trim(s);
    if (t.empty()) return false;
    try {
        size_t pos = 0;
        int val = std::stoi(t, &pos);
        if (pos != t.size()) return false;
        outValue = val;
        return true;
    } catch (...) {
        return false;
    }
}

} 
