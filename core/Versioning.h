#ifndef _VERSIONING_H
#define _VERSIONING_H

#include <string>
#include <vector>

extern const std::string VERSION;
/////////////////////////////////////

#define CURDATE __DATE__
#define CURTIME __TIME__

#define BUILD_DATE_YEAR_CH0 (CURDATE[ 7])
#define BUILD_DATE_YEAR_CH1 (CURDATE[ 8])
#define BUILD_DATE_YEAR_CH2 (CURDATE[ 9])
#define BUILD_DATE_YEAR_CH3 (CURDATE[10])


#define BUILD_DATE_MONTH_IS_JAN (CURDATE[0] == 'J' && CURDATE[1] == 'a' && CURDATE[2] == 'n')
#define BUILD_DATE_MONTH_IS_FEB (CURDATE[0] == 'F')
#define BUILD_DATE_MONTH_IS_MAR (CURDATE[0] == 'M' && CURDATE[1] == 'a' && CURDATE[2] == 'r')
#define BUILD_DATE_MONTH_IS_APR (CURDATE[0] == 'A' && CURDATE[1] == 'p')
#define BUILD_DATE_MONTH_IS_MAY (CURDATE[0] == 'M' && CURDATE[1] == 'a' && CURDATE[2] == 'y')
#define BUILD_DATE_MONTH_IS_JUN (CURDATE[0] == 'J' && CURDATE[1] == 'u' && CURDATE[2] == 'n')
#define BUILD_DATE_MONTH_IS_JUL (CURDATE[0] == 'J' && CURDATE[1] == 'u' && CURDATE[2] == 'l')
#define BUILD_DATE_MONTH_IS_AUG (CURDATE[0] == 'A' && CURDATE[1] == 'u')
#define BUILD_DATE_MONTH_IS_SEP (CURDATE[0] == 'S')
#define BUILD_DATE_MONTH_IS_OCT (CURDATE[0] == 'O')
#define BUILD_DATE_MONTH_IS_NOV (CURDATE[0] == 'N')
#define BUILD_DATE_MONTH_IS_DEC (CURDATE[0] == 'D')


#define BUILD_DATE_MONTH_CH0 \
((BUILD_DATE_MONTH_IS_OCT || BUILD_DATE_MONTH_IS_NOV || BUILD_DATE_MONTH_IS_DEC) ? '1' : '0')

#define BUILD_DATE_MONTH_CH1 \
( \
(BUILD_DATE_MONTH_IS_JAN) ? '1' : \
(BUILD_DATE_MONTH_IS_FEB) ? '2' : \
(BUILD_DATE_MONTH_IS_MAR) ? '3' : \
(BUILD_DATE_MONTH_IS_APR) ? '4' : \
(BUILD_DATE_MONTH_IS_MAY) ? '5' : \
(BUILD_DATE_MONTH_IS_JUN) ? '6' : \
(BUILD_DATE_MONTH_IS_JUL) ? '7' : \
(BUILD_DATE_MONTH_IS_AUG) ? '8' : \
(BUILD_DATE_MONTH_IS_SEP) ? '9' : \
(BUILD_DATE_MONTH_IS_OCT) ? '0' : \
(BUILD_DATE_MONTH_IS_NOV) ? '1' : \
(BUILD_DATE_MONTH_IS_DEC) ? '2' : \
/* error default */    '?' \
)

#define BUILD_DATE_DAY_CH0 ((CURDATE[4] >= '0') ? (CURDATE[4]) : '0')
#define BUILD_DATE_DAY_CH1 (CURDATE[ 5])



// Example of CURTIME string: "21:06:19"
//                              01234567

#define BUILD_TIME_HOUR_CH0 (CURTIME[0])
#define BUILD_TIME_HOUR_CH1 (CURTIME[1])

#define BUILD_TIME_MIN_CH0 (CURTIME[3])
#define BUILD_TIME_MIN_CH1 (CURTIME[4])

#define BUILD_TIME_SEC_CH0 (CURTIME[6])
#define BUILD_TIME_SEC_CH1 (CURTIME[7])

#define CHAR_TO_INT(character) ((character) -'0')
#define CHAR_MULTIPLY_TO_INT(character, multiplier) (CHAR_TO_INT(character)*(multiplier))

#define BUILD_TIME_SECONDS_INT (CHAR_MULTIPLY_TO_INT(BUILD_TIME_SEC_CH0,10) + CHAR_MULTIPLY_TO_INT(BUILD_TIME_SEC_CH1,1))
#define BUILD_TIME_MINUTES_INT (CHAR_MULTIPLY_TO_INT(BUILD_TIME_MIN_CH0,10) + CHAR_MULTIPLY_TO_INT(BUILD_TIME_MIN_CH1,1))
#define BUILD_TIME_HOURS_INT (CHAR_MULTIPLY_TO_INT(BUILD_TIME_HOUR_CH0,10) + CHAR_MULTIPLY_TO_INT(BUILD_TIME_HOUR_CH1,1))

#define BUILD_DATE_DAY_INT (CHAR_MULTIPLY_TO_INT(BUILD_DATE_DAY_CH0,10) + CHAR_MULTIPLY_TO_INT(BUILD_DATE_DAY_CH1,1))
#define BUILD_DATE_MONTH_INT (CHAR_MULTIPLY_TO_INT(BUILD_DATE_MONTH_CH0,10) + CHAR_MULTIPLY_TO_INT(BUILD_DATE_MONTH_CH1,1))
#define BUILD_DATE_YEAR_INT (CHAR_MULTIPLY_TO_INT(BUILD_DATE_YEAR_CH0,1000) + CHAR_MULTIPLY_TO_INT(BUILD_DATE_YEAR_CH1,100) +  \
                             CHAR_MULTIPLY_TO_INT(BUILD_DATE_YEAR_CH2,10) + CHAR_MULTIPLY_TO_INT(BUILD_DATE_YEAR_CH3,1))


/////////////////////////////////////

const std::string CONSONANTS_MONTH = "JFMPYNLGSCVD";
const std::string LETTERS_DAY = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcde";
const std::string LETTERS_HOUR = "ABCDEFGHIJKLMNOPQRSTUVWX";
const std::vector<std::string> WORDS_MINUTE = {
    "cat", "dog", "fish", "bird", "horse", "cow", "sheep",
    "goat", "duck", "hen", "pig", "fox", "bear", "deer",
    "lion", "tiger", "wolf", "shark", "whale", "dolphin",
    "ant", "bee", "frog", "bat", "owl", "rat", "mouse",
    "moose", "elk", "seal", "crab", "clam", "squid", "snail",
    "hawk", "crow", "swan", "squirrel", "rabbit", "gecko",
    "panda", "otter", "zebra", "giraffe", "camel", "llama",
    "ape", "chimp", "human", "skunk", "mole", "vole", "turkey",
    "robin", "sparrow", "finch", "eagle", "falcon", "raven", "puma"
};

/////////////////////////////////////
extern const std::string BUILD_NUMBER_RAW;
extern const std::string BUILD_NUMBER;

#endif