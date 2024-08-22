#ifndef _CRASH_DETECTOR_H
#define _CRASH_DETECTOR_H

#include "MessageManager.h"

struct CrashData {
    CrashData(const bool probableCrash, const std::string autoSaveTimeString, const std::string saveFileName) :
        probableCrash(probableCrash), autoSaveTimeString(autoSaveTimeString), saveFileName(saveFileName)
    {}

    const bool probableCrash;
    const std::string autoSaveTimeString;
    const std::string saveFileName;
};

class CrashDetector {
public:
    static CrashData checkForCrash() {
        Logger::log("CDP");
        std::ifstream in(Logger::getLogFileName());

        if (!in.good()) {
            MessageManager::displayMessage("No log file found", 5, DEBUG);
            in.close();

            return { false, "", "" };
        } else {
            bool crashDetectorPresent = false;
            bool successfulShutdown = false;
            std::string autoSaveDataRaw = "NONE";
            std::string saveFileName = "NONE";

            std::string line;
            while (getline(in, line)) {
                if (stringEndsWith(line, "CDP")) crashDetectorPresent = true;
                else if (stringEndsWith(line, "SHUTDOWN")) successfulShutdown = true;
                else if (crashDetectorPresent && stringEndsWith(line, "Autosaved")) autoSaveDataRaw = line;
                else if (crashDetectorPresent && stringEndsWith(line, "SFN")) {
                    saveFileName = splitString(line, ">>")[1];
                }
            }
            in.close();

            if (crashDetectorPresent && !successfulShutdown) {
                std::string autoSaveTimeString = "NONE";
                if (autoSaveDataRaw != "NONE") {
                    const std::vector<std::string> autoSaveDataParsed = splitString(autoSaveDataRaw, " ");

                    if (autoSaveDataParsed.size() != 7) {
                        MessageManager::displayMessage("Bad autosave data: " + std::to_string(autoSaveDataParsed.size()), 5, DEBUG);
                        return { true, "NONE", "NONE" };
                    }

                    autoSaveTimeString = "";
                    autoSaveTimeString += splitString(autoSaveDataParsed[0], "(")[1] + ", ";
                    autoSaveTimeString += autoSaveDataParsed[1] + " ";
                    autoSaveTimeString += autoSaveDataParsed[2] + " at ";
                    autoSaveTimeString += splitString(autoSaveDataParsed[3], ":")[0] + ":";
                    autoSaveTimeString += splitString(autoSaveDataParsed[3], ":")[1];
                }

                return { true, autoSaveTimeString, saveFileName };
            }

            return { false, "", "" };
        }
    }
};

#endif

