#ifndef _PARSER_H
#define _PARSER_H

#include <vector>
#include <string>
#include "Expression.h"

#define EXPR std::shared_ptr<Expression>

class Parser {
public:
    std::vector<EXPR> parse(std::vector<std::string> tokens);
private:
    int _currentTokenIndex = 0;
    std::string _currentToken = "";
    std::vector<std::string> _tokens;

    void eat(std::string expectedToken);

    EXPR factor();
    EXPR term();
    EXPR expression();
    EXPR comp();
    EXPR bool_and();
    EXPR bool_or();
    EXPR assignment();
    EXPR statement();
    EXPR endif();
};

#endif