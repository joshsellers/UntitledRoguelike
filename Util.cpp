#include "Util.h"

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
