#include "FileIntegrityManager.h"
#include <fstream>
#include "MessageManager.h"

constexpr const char SALT[10] = "saltyPeas";

void FileIntegrityManager::verifyFiles() {
    bool verifiedAll = true;

    for (const auto& pathRaw : _paths) {
        std::string path = pathRaw;
        bool localLow = false;
        if (stringStartsWith(path, "LOCALLOW")) {
            replaceAll(path, "LOCALLOW", getLocalLowPath());
            if (!std::filesystem::is_directory(path + "\\")) continue;
            localLow = true;
        }

        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            const std::vector<std::string> folders = splitString(entry.path().string(), "\\");
            if (folders.size() <= 1) continue;
            else if (splitString(folders.at(folders.size() - 1), ".").size() != 2) continue;
            else if (splitString(folders.at(folders.size() - 1), ".")[1] != "item" 
                && splitString(folders.at(folders.size() - 1), ".")[1] != "projectile" 
                && splitString(folders.at(folders.size() - 1), ".")[1] != "config") continue;

            files.push_back(folders.at(folders.size() - 1));
        }

        for (const auto& file : files) {
            const std::string fullpath = pathRaw + (localLow ? "\\" : "/") + file;
            
            if (!verifyFile(fullpath)) {
                verifiedAll = false;
                MessageManager::displayMessage("Verification failed for " + fullpath, 5, WARN);
            }
        }
    }

    if (!verifiedAll) {
        MessageManager::displayMessage("One or more modified game files were detected. Achievements will be disabled.", 10, SPECIAL);
        DISABLE_ACHIEVEMENTS = true;
    }
}

bool FileIntegrityManager::verifyFile(std::string path) {
    bool allowNotExist = false;
    if (stringStartsWith(path, "LOCALLOW")) {
        replaceAll(path, "LOCALLOW", getLocalLowPath());
        allowNotExist = true;
    }

    std::ifstream in(path);

    if (!in.good()) {
        MessageManager::displayMessage("Could not open " + path + " for verifying", 5, WARN);
        in.close();

        if (allowNotExist) return true;
    } else {
        std::string fullText = "";
        std::string line;
        std::string readSignature = "";
        while (getline(in, line)) {
            if (stringStartsWith(line, _sigPrefix)) {
                readSignature = line;
                continue;
            }
            fullText += line;
        }

        in.close();

        const std::string signature = _sigPrefix + hash(std::string(SALT) + fullText);

        return signature == readSignature;
    }

    return false;
}

void FileIntegrityManager::signFiles() {
    for (const auto& pathRaw : _paths) {
        std::string path = pathRaw;
        bool localLow = false;
        if (stringStartsWith(path, "LOCALLOW")) {
            replaceAll(path, "LOCALLOW", getLocalLowPath());
            if (!std::filesystem::is_directory(path + "\\")) continue;
            localLow = true;
        }

        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            const std::vector<std::string> folders = splitString(entry.path().string(), "\\");
            if (folders.size() <= 1) continue;
            else if (splitString(folders.at(folders.size() - 1), ".").size() != 2) continue;
            else if (splitString(folders.at(folders.size() - 1), ".")[1] != "item"
                && splitString(folders.at(folders.size() - 1), ".")[1] != "projectile"
                && splitString(folders.at(folders.size() - 1), ".")[1] != "config") continue;

            files.push_back(folders.at(folders.size() - 1));
        }

        for (const auto& file : files) {
            const std::string fullpath = path + (localLow ? "\\" : "/") + file;
            signFile(fullpath);
        }
    }
}

void FileIntegrityManager::signFile(std::string path) {
    std::ifstream in(path);

    if (!in.good()) {
        MessageManager::displayMessage("Could not open " + path + " for signing", 5, WARN);
        in.close();
    } else {
        std::vector<std::string> lines;
        std::string fullText = "";
        std::string line;
        while (getline(in, line)) {
            if (stringStartsWith(line, _sigPrefix)) continue;
            fullText += line;
            lines.push_back(line);
        }

        in.close();

        const std::string signature = _sigPrefix + hash(std::string(SALT) + fullText);

        try {
            if (!std::filesystem::remove(path)) {
                MessageManager::displayMessage("Could not remove " + path, 5, WARN);
                return;
            }
        } catch (const std::filesystem::filesystem_error& err) {
            MessageManager::displayMessage("Could not remove " + path + ": " + (std::string)err.what(), 5, ERR);
            return;
        }

        try {
            std::ofstream out(path);
            out << signature << std::endl;
            for (const auto& line : lines) {
                out << line << std::endl;
            }

            out.close();
        } catch (std::exception ex) {
            MessageManager::displayMessage("Exception while signing " + path + ": " + (std::string)ex.what(), 5, WARN);
        }
    }
}
