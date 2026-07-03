#pragma once

#include <string>
#include <map>

class ConfigManager {
public:
    explicit ConfigManager(const std::string& path);
    void load();
    bool save() const;
    std::string get(const std::string& key, const std::string& defaultValue = "") const;
    void set(const std::string& key, const std::string& value);

private:
    std::string path_;
    std::map<std::string, std::string> values_;
};
