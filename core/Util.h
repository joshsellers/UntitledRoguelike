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

bool stringStartsWith(std::string str, std::string start);

std::string generateUID();

#endif