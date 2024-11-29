#include "Tokenizer.h"
#include "../../core/Util.h"
#include <iostream>

std::vector<std::string> Tokenizer::tokenize(std::string inScript) {
    replaceAll(inScript, "()", "(NULL)");

    std::string script = "";
    bool replaceSpaces = false;
    for (auto symbol : inScript) {
        if (replaceSpaces && symbol == ' ') {
            script += "RPLSPC";
        } else if (replaceSpaces && symbol != '"') {
            for (int i = 4; i < operators.size(); i++) {
                if (symbol == operators.at(i).at(0)) {
                    script += "RPL" + std::to_string(i);
                    break;
                }
            }
        }

        if (symbol != ' ' || !replaceSpaces) {
            bool isOperator = false;
            for (int i = 4; i < operators.size(); i++) {
                if (symbol == operators.at(i).at(0)) {
                    isOperator = true;
                    break;
                }
            }
            if (!isOperator || !replaceSpaces) {
                script += std::string(1, symbol);
            }
        }
        if (symbol == '"') replaceSpaces = !replaceSpaces;
    }

    std::vector<std::string> tokens;

    std::vector<std::string> bareTokens = splitString(script, " ");
    for (std::string& bareToken : bareTokens) {
        replaceAll(bareToken, "RPLSPC", " ");
    }

    for (std::string bareToken : bareTokens) {
        std::vector<std::string> operatorTokens = splitOperators(bareToken);
        for (std::string token : operatorTokens) {
            if (token != "")
                tokens.push_back(token);
        }
    }

    for (std::string& token : tokens) {
        for (int i = 4; i < operators.size(); i++) {
            replaceAll(token, "RPL" + std::to_string(i), operators.at(i));
        }

        if (!stringStartsWith(token, "\"")) {
            replaceAll(token, "true", "1");
            replaceAll(token, "false", "0");
        }
    }

    tokens.push_back("EOF");

    return tokens;
}

std::vector<std::string> Tokenizer::splitOperators(std::string bareToken) {
    std::vector<std::string> operatorExpressionTokens;
    for (std::string operatorString : operators) {
        if (bareToken.find(operatorString) != std::string::npos) {
            std::vector<std::string> tokens = splitString(bareToken, operatorString);
            std::vector<std::string> previousTokens = splitOperators(tokens.at(0));
            for (std::string previousToken : previousTokens) {
                operatorExpressionTokens.push_back(previousToken);
            }

            std::vector<std::string> additionalExpressions;
            for (int i = 1; i < tokens.size(); i++) {
                additionalExpressions.push_back(operatorString);
                for (std::string subToken : splitOperators(tokens.at(i))) {
                    additionalExpressions.push_back(subToken);
                }
            }

            for (std::string expression : additionalExpressions) {
                operatorExpressionTokens.push_back(expression);
            }

            return operatorExpressionTokens;
        }
    }

    operatorExpressionTokens.push_back(bareToken);
    return operatorExpressionTokens;
}
