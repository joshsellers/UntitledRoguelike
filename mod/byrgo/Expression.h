#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include <string>
#include <memory>
#include <vector>
#include "../../core/Util.h"

class Expression {
public:
    virtual ~Expression() {}
    virtual std::string evaluate() = 0;
};

class LiteralExpression : public Expression {
public:
    LiteralExpression(std::string value)
        : value(value) {
    }

    virtual std::string evaluate() {
        return "LIT:" + value;
    }

private:
    std::string value;
};

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, std::string operation)
        : left(left),
        right(right),
        operation(operation) {
    }

    virtual std::string evaluate() {
        return left->evaluate() + ":" + right->evaluate() + ":" + operation;
    }

private:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
    std::string operation;
};

class AssignmentExpression : public Expression {
public:
    AssignmentExpression(std::shared_ptr<Expression> variableName, std::shared_ptr<Expression> assignment)
        : variableName(variableName),
        assignment(assignment) {
    }

    virtual std::string evaluate() {
        std::string variableString = variableName->evaluate();
        std::string outputString = ":ASSIGN:" + variableString;
        std::string prefix = "";
        if (splitString(variableString, ":")[1] == "if") {
            outputString = ":IF";
        } else if (splitString(variableString, ":")[1] == "while") {
            outputString = ":WHILE";
            prefix = "STWH:";
        }
        return prefix + assignment->evaluate() + outputString;
    }
private:
    std::shared_ptr<Expression> variableName;
    std::shared_ptr<Expression> assignment;
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(std::string op, std::shared_ptr<Expression> expression)
        : op(op),
        expression(expression) {
    }

    virtual std::string evaluate() {
        return expression->evaluate() + ":UNARY:" + op;
    }
private:
    std::string op;
    std::shared_ptr<Expression> expression;
};

class GameFuncExpression : public Expression {
public:
    GameFuncExpression(std::shared_ptr<Expression> funcName, std::vector<std::shared_ptr<Expression>> parameters)
        : funcName(funcName),
        parameters(parameters) {
    }

    virtual std::string evaluate() {
        std::string output = "";
        for (auto& parameter : parameters) {
            output += parameter->evaluate() + ":";
        }
        return output + splitString(funcName->evaluate(), ":")[1];
    }
private:
    std::shared_ptr<Expression> funcName;
    std::vector<std::shared_ptr<Expression>> parameters;
};

class EndIfExpression : public Expression {
public:
    EndIfExpression(std::shared_ptr<Expression> previousStatement)
        : previousStatement(previousStatement) {
    }

    virtual std::string evaluate() {
        if (previousStatement != nullptr) {
            return previousStatement->evaluate() + ":ENDIF";
        } else {
            return ":ENDIF";
        }
    }
private:
    std::shared_ptr<Expression> previousStatement;
};

class ControlFlowExpression : public Expression {
public:
    ControlFlowExpression(std::string type)
        : type(type) {
    }

    virtual std::string evaluate() {
        if (type == "break") type = "BREAK";
        else if (type == "continue") type = "CONT";
        return ":" + type;
    }
private:
    std::string type;
};

class ReturnExpression : public Expression {
public:
    ReturnExpression(std::shared_ptr<Expression> returnedExpression)
        : returnedExpression(returnedExpression) {
    }

    virtual std::string evaluate() {
        return returnedExpression->evaluate() + ":RETURN";
    }
private:
    std::shared_ptr<Expression> returnedExpression;
};

#endif