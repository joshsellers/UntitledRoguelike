#include "Util.h"
#include <random>
#include <sstream>
#include <filesystem>
#include <fstream>
#include "MessageManager.h"
#include "Tutorial.h"
#include <regex>

int randomInt(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

long long currentTimeMillis() {
    auto time = std::chrono::system_clock::now();

    auto since_epoch = time.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>
        (since_epoch);

    return millis.count();
}

long long currentTimeNano() {
    auto time = std::chrono::system_clock::now();

    auto since_epoch = time.time_since_epoch();
    auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>
        (since_epoch);
    return nano.count();
}

float norm_0_1(float x, float min, float max) {
    return (x - min) / (max - min);
}

std::string trimString(std::string str) {
    if (str.find(".") != std::string::npos) {
        for (std::string::size_type s = str.length() - 1; s > 0; --s) {
            if (str[s - 1] == '.') break;
            else if (str[s] == '0') str.erase(s, 1);
            else break;
        }
    }
    return str;
}

std::vector<std::string> splitString(std::string str, std::string delimiter) {
    size_t pos = 0;
    std::vector<std::string> parsedString;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        parsedString.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
    }
    parsedString.push_back(str);

    return parsedString;
}

bool stringStartsWith(std::string str, std::string start) {
    return str.rfind(start, 0) == 0;
}

std::string generateUID() {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };
    return ss.str();
}

std::string getLocalLowPath() {
    std::string pathStr = "err";
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "APPDATA") == 0 && buf != nullptr) {
        std::string temp(buf);
        pathStr =
            std::regex_replace(
                temp, std::regex("Roaming"),
                "LocalLow\\Rolmi"
            );
        free(buf);
    } else {
        MessageManager::displayMessage("Failed path retrieval", 5, ERR);
    }
    return pathStr;
}

void updateSettingsFile(std::string path) {
    try {
        if (!std::filesystem::remove(path)) {
            MessageManager::displayMessage("Could not replace settings file", 5, DEBUG);
        }
    } catch (const std::filesystem::filesystem_error& err) {
        MessageManager::displayMessage("Could not replace settings file: " + (std::string)err.what(), 5, ERR);
    }

    try {
        std::ofstream out(path);
        int fullscreenSetting = FULLSCREEN ? 1 : 0;
        int tutorialCompleted = Tutorial::isCompleted() ? 1 : 0;
        int vsyncEnabled = VSYNC_ENABLED ? 1 : 0;
        out << "fullscreen=" << std::to_string(fullscreenSetting) << std::endl;
        out << "tutorial=" << std::to_string(tutorialCompleted) << std::endl;
        out << "vsync=" << std::to_string(vsyncEnabled) << std::endl;
        out.close();
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error writing to settings file: " + (std::string)ex.what(), 5, ERR);
    }
}

void updateSettingsFiles() {
    updateSettingsFile("settings.config");

    std::string localLowPath = getLocalLowPath();
    if (localLowPath == "err") return;
    if (!std::filesystem::is_directory(localLowPath + "\\")) {
        std::filesystem::create_directory(localLowPath);
    }

    localLowPath += "\\settings.config";
    updateSettingsFile(localLowPath);
}
