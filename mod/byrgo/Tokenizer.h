#ifndef _TOKENIZER_H
#define _TOKENIZER_H

#include <vector>
#include <string>

class Tokenizer {
public:
    static std::vector<std::string> tokenize(std::string script);
private:
    static std::vector<std::string> splitOperators(std::string bareToken);
    static inline std::vector<std::string> operators = { "!=", "==", ">=", "<=", "+", "-", "*", "/", "=", ";", ",", ":", "(", ")", "{", "}", ">", "<", "!", "%" };
};

#endif