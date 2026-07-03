#pragma once

#include <string>
#include "MusicLibrary.h"

class CsvLoader {
public:
    static int load(const std::string& path, MusicLibrary& library);

private:
    static bool parseLine(const std::string& line, MusicLibrary& library);
};
