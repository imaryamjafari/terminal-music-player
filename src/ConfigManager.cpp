#include "ConfigManager.h"
#include "StringUtils.h"
#include <fstream>

ConfigManager::ConfigManager(const std::string& path) : path_(path) {
}

void ConfigManager::load() {
    std::ifstream file(path_);
    if (!file.is_open()) {
        return;
    }

    try {
        std::string line;
        while (std::getline(file, line)) {
            std::string trimmed = StringUtils::trim(line);
            if (trimmed.empty() || trimmed[0] == '#') continue;

            size_t eqPos = trimmed.find('=');
            if (eqPos == std::string::npos) continue;

            std::string key = StringUtils::trim(trimmed.substr(0, eqPos));
            std::string value = StringUtils::trim(trimmed.substr(eqPos + 1));
            if (!key.empty()) {
                values_[key] = value;
            }
        }
    } catch (const std::exception&) {}

    file.close();
}

bool ConfigManager::save() const {
    try {
        std::ofstream file(path_);
        if (!file.is_open()) return false;

        for (const auto& kv : values_) {
            file << kv.first << "=" << kv.second << "\n";
        }
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ConfigManager::get(const std::string& key, const std::string& defaultValue) const {
    auto it = values_.find(key);
    if (it == values_.end()) return defaultValue;
    return it->second;
}

void ConfigManager::set(const std::string& key, const std::string& value) {
    values_[key] = value;
}
