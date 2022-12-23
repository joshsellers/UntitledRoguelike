#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h>
#include <chrono>
#include <string>

int randomInt(int min, int max);

long long currentTimeMillis();

std::string trimString(std::string str);

#endif