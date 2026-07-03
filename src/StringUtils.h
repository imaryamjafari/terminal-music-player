#pragma once

#include <string>
#include <vector>

namespace StringUtils {
    std::string trim(const std::string& s);
    std::string toLower(const std::string& s);

    bool containsCaseInsensitive(const std::string& haystack, const std::string& needle);
    std::vector<std::string> splitCsvLine(const std::string& line);
    bool tryParseInt(const std::string& s, int& outValue);
}
