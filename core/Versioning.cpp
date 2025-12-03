#include "Versioning.h"
#include "Globals.h"

const std::string VERSION = "1.1.2";

const std::string BUILD_NUMBER_RAW =
std::to_string(BUILD_DATE_YEAR_INT - 2000)
+ CONSONANTS_MONTH.at(BUILD_DATE_MONTH_INT - 1)
+ LETTERS_DAY.at(BUILD_DATE_DAY_INT - 1)
+ LETTERS_HOUR.at(BUILD_TIME_HOURS_INT)
+ "-" + WORDS_MINUTE.at(BUILD_TIME_MINUTES_INT)
+ "-" + WORDS_MINUTE.at(BUILD_TIME_SECONDS_INT);

#ifdef DBGBLD
const std::string BUILD_NUMBER = BUILD_NUMBER_RAW + ".D";
#endif
#ifndef DBGBLD
const std::string BUILD_NUMBER = BUILD_NUMBER_RAW + ".R";
#endif