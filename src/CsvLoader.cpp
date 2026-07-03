#include "CsvLoader.h"
#include "StringUtils.h"
#include <fstream>
#include <sstream>

int CsvLoader::load(const std::string& path, MusicLibrary& library) {
    std::ifstream file(path);
    int loadedCount = 0;

    if (!file.is_open()) return 0;

    std::string line;
    bool isHeader = true;

    while (std::getline(file, line)) {
        if (StringUtils::trim(line).empty()) continue;

        if (isHeader) {
            isHeader = false;
            continue;
        }

        if (parseLine(line, library)) 
            loadedCount++;
    }

    file.close();
    return loadedCount;
}

bool CsvLoader::parseLine(const std::string& line, MusicLibrary& library) {
    std::vector<std::string> fields = StringUtils::splitCsvLine(line);
    if (fields.size() < 7)
        return false;

    const std::string& title = fields[0];
    const std::string& artist = fields[1];
    const std::string& album = fields[2];
    const std::string& genre = fields[3];
    const std::string& yearStr = fields[4];
    const std::string& durationStr = fields[5];
    const std::string& filePath = fields[6];

    if (title.empty() || artist.empty() || filePath.empty())
        return false;

    int year = 0;
    if (!StringUtils::tryParseInt(yearStr, year)) 
        return false;
  
    int durationSec = 0;
    if (!StringUtils::tryParseInt(durationStr, durationSec) || durationSec < 0) 
        return false;
  
    library.addSong(title, artist, album, genre, year, durationSec, filePath);
    return true;
}
