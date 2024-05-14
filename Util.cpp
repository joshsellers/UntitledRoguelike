#include "Util.h"
#include <random>
#include <sstream>

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

std::string generateUUID() {
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
