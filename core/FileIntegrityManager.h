#ifndef _FILE_INTEGRITY_MANAGER_H
#define _FILE_INTEGRITY_MANAGER_H

#include <string>
#include <vector>


class FileIntegrityManager {
public:
    static void verifyFiles();
    static bool verifyFile(std::string path);
    static void signFiles();
    static void signFile(std::string path);
private:

    static inline const std::string _sigPrefix = "#!SIG";

    static inline const std::vector<std::string> _paths = {
        "data/items",
        "data/projectiles",
        "LOCALLOW\\save0",
        "LOCALLOW\\save1",
        "LOCALLOW\\save2"
    };
};

#endif