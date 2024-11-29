#include "Parser.h"
#include <iostream>
#include "../../core/MessageManager.h"

std::vector<EXPR> Parser::parse(std::vector<std::string> tokens) {
    _tokens = tokens;
    _currentTokenIndex = 0;
    _currentToken = _tokens[_currentTokenIndex];

    std::vector<EXPR> statements;
    while (_currentToken != "EOF") {
        statements.push_back(endif());
    }

    return statements;
}

void Parser::eat(std::string expectedToken) {
    if (_currentToken == expectedToken || expectedToken == "LIT") {
        _currentTokenIndex++;
        if (_currentTokenIndex >= _tokens.size()) return;
        _currentToken = _tokens.at(_currentTokenIndex);
    } else {
        MessageManager::displayMessage("Unexpected token: " + _currentToken, 5, ERR);
    }
}

EXPR Parser::factor() {
    std::shared_ptr<Expression> result = nullptr;

    if (_currentToken == "-" || _currentToken == "not" || _currentToken == "!") {
        std::string op = _currentToken;
        eat(_currentToken);

        result = std::shared_ptr<UnaryExpression>(new UnaryExpression(op, factor()));
    } else if (_currentToken == "(") {
        eat("(");
        result = bool_or();
        eat(")");
    } else {
        result = std::shared_ptr<LiteralExpression>(new LiteralExpression(_currentToken));
        eat("LIT");
    }
    return result;
}

EXPR Parser::term() {
    std::shared_ptr<Expression> result = factor();

    while (_currentToken == "*" || _currentToken == "/" || _currentToken == "%") {
        if (_currentToken == "*") {
            eat("*");
            result = std::shared_ptr<BinaryExpression>(new BinaryExpression(result, factor(), "*"));
        } else if (_currentToken == "/") {
            eat("/");
            result = std::shared_ptr<BinaryExpression>(new BinaryExpression(result, factor(), "/"));
        } else if (_currentToken == "%") {
            eat("%");
            result = std::shared_ptr<BinaryExpression>(new BinaryExpression(result, factor(), "%"));
        }
    }

    return result;
}

EXPR Parser::expression() {
    std::shared_ptr<Expression> result = term();

    while (_currentToken == "+" || _currentToken == "-") {
        if (_currentToken == "+") {
            eat("+");
            result = std::shared_ptr<BinaryExpression>(new BinaryExpression(result, term(), "+"));
        } else if (_currentToken == "-") {
            eat("-");
            result = std::shared_ptr<BinaryExpression>(new BinaryExpression(result, term(), "-"));
        }
    }

    return result;
}

EXPR Parser::comp() {
    EXPR result = expression();

    while (_currentToken == ">" || _currentToken == "<" || _currentToken == ">=" || _currentToken == "<=" || _currentToken == "==" || _currentToken == "!=") {
        std::string op = _currentToken;
        eat(_currentToken);
        result = std::shared_ptr<BinaryExpression>(new BinaryExpression(result, expression(), op));
    }

    return result;
}

EXPR Parser::bool_or() {
    EXPR result = bool_and();

    while (_currentToken == "or") {
        eat("or");
        result = std::shared_ptr<BinaryExpression>(new BinaryExpression(result, bool_and(), "or"));
    }

    return result;
}

EXPR Parser::bool_and() {
    EXPR result = comp();

    while (_currentToken == "and") {
        eat("and");
        result = std::shared_ptr<BinaryExpression>(new BinaryExpression(result, comp(), "and"));
    }

    return result;
}

EXPR Parser::assignment() {
    if (_currentToken == "break" || _currentToken == "continue") {
        std::string controlFlowType = _currentToken;
        eat(_currentToken);
        return EXPR(new ControlFlowExpression(controlFlowType));
    } else if (_currentToken == "return") {
        eat("return");
        return EXPR(new ReturnExpression(bool_or()));
    }

    auto variableName = bool_or();

    if (_currentToken == "=") {
        eat("=");
        auto value = bool_or();
        if (_currentToken == "(") {
            _currentTokenIndex -= 1;
            _currentToken = _tokens.at(_currentTokenIndex);
            return std::shared_ptr<AssignmentExpression>(new AssignmentExpression(variableName, assignment()));
        }
        return std::shared_ptr<AssignmentExpression>(new AssignmentExpression(variableName, value));
    } else if (_currentToken == ":") {
        eat(":");
        auto value = bool_or();
        eat("{");
        auto result = std::shared_ptr<AssignmentExpression>(new AssignmentExpression(variableName, value));
        return result;
    } else if (_currentToken == "(") {
        eat("(");
        auto value = bool_or();
        std::vector<EXPR> parameters;
        parameters.push_back(value);

        while (_currentToken == ",") {
            eat(",");
            parameters.push_back(bool_or());
            if (_currentToken == ")") break;
        }
        eat(")");

        auto parameterExpr = std::shared_ptr<GameFuncExpression>(new GameFuncExpression(variableName, parameters));
        return parameterExpr;
    }

    return std::shared_ptr<LiteralExpression>(new LiteralExpression(bool_or()->evaluate()));
}

EXPR Parser::statement() {
    auto expr = assignment();

    if (_currentToken == ";") {
        eat(";");
    }

    return expr;
}

EXPR Parser::endif() {
    if (_currentToken == "}") {
        eat("}");
        return std::shared_ptr<EndIfExpression>(new EndIfExpression(nullptr));
    }
    auto expr = statement();

    if (_currentToken == "}") {
        eat("}");
        return std::shared_ptr<EndIfExpression>(new EndIfExpression(expr));
    }

    return expr;
}

