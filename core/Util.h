#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h>
#include <chrono>
#include <string>
#include <vector>

int randomInt(int min, int max);

long long currentTimeMillis();

long long currentTimeNano();

std::string trimString(std::string str);

std::vector<std::string> splitString(std::string str, std::string delimiter);

void replaceAll(std::string& str, const std::string& from, const std::string& to);

bool stringStartsWith(std::string str, std::string start);
bool stringEndsWith(std::string const& fullString, std::string const& ending);

std::string generateUID();

std::string getLocalLowPath();

void updateSettingsFile(std::string path);
void updateSettingsFiles();

#endif