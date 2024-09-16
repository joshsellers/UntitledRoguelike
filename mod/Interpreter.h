#ifndef _INTERPRETER_H
#define _INTERPRETER_H
#include <cassert>
#include <vector>
#include <string>

#include "../world/entities/Entity.h"
#include "../core/MessageManager.h"

class Interpreter {
public:
    int interpret(std::vector<int> bytecode, Entity* entity);
private:
    static inline const int MAX_STACK = 128;
    int _stackSize = 0;
    float _stack[MAX_STACK];

    void push(float value) {
        if (_stackSize < MAX_STACK) {
            _stack[_stackSize++] = value;
        } else {
            MessageManager::displayMessage("Stack overflow", 5, WARN);
        }
    }

    float pop() {
        if (_stackSize > 0) {
            return _stack[--_stackSize];
        } else {
            MessageManager::displayMessage("Stack underflow", 5, WARN);
            return 0;
        }
    }

    int _strStackSize = 0;
    std::string _strStack[MAX_STACK];

    void strPush(std::string value) {
        if (_strStackSize < MAX_STACK) {
            _strStack[_strStackSize++] = value;
        } else {
            MessageManager::displayMessage("String stack overflow", 5, WARN);
        }
    }

    std::string strPop() {
        if (_strStackSize > 0) {
            return _strStack[--_strStackSize];
        } else {
            MessageManager::displayMessage("String stack underflow", 5, WARN);
            return "";
        }
    }

    int _callStackSize = 0;
    int _callStack[MAX_STACK];
    void callStackPush(int value) {
        if (_callStackSize < MAX_STACK) {
            _callStack[_callStackSize++] = value;
        } else {
            MessageManager::displayMessage("Call stack overflow", 5, WARN);
        }
    }

    int callStackPop() {
        if (_callStackSize > 0) {
            return _callStack[--_callStackSize];
        } else {
            MessageManager::displayMessage("Call stack underflow", 5, WARN);
            return -1;
        }
    }

    static inline const int MAX_VARIABLES = 256;
    float _register[MAX_VARIABLES];
    std::string _strRegister[MAX_VARIABLES];


    void skipStringLit(int& index, std::vector<int> bytecode);
};

#endif