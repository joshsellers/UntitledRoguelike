#ifndef _INTERPRETER_H
#define _INTERPRETER_H
#include <cassert>
#include <vector>
#include <string>

#include "../world/entities/Entity.h"
#include "../core/MessageManager.h"

class Interpreter {
public:
    static int interpret(std::vector<int> bytecode, Entity* entity);
private:
    static inline const int MAX_STACK = 128;
    static inline int _stackSize;
    static inline float _stack[MAX_STACK];

    static void push(float value) {
        assert(_stackSize < MAX_STACK);
        _stack[_stackSize++] = value;
    }

    static float pop() {
        if (_stackSize > 0) {
            return _stack[--_stackSize];
        } else {
            MessageManager::displayMessage("Stack underflow", 5, WARN);
            return 0;
        }
    }

    static inline int _strStackSize;
    static inline std::string _strStack[MAX_STACK];

    static void strPush(std::string value) {
        assert(_strStackSize < MAX_STACK);
        _strStack[_strStackSize++] = value;
    }

    static std::string strPop() {
        if (_strStackSize > 0) {
            return _strStack[--_strStackSize];
        } else {
            MessageManager::displayMessage("String stack underflow", 5, WARN);
            return "";
        }
    }

    static inline int _callStackSize;
    static inline int _callStack[MAX_STACK];
    static void callStackPush(int value) {
        assert(_callStackSize < MAX_STACK);
        _callStack[_callStackSize++] = value;
    }

    static int callStackPop() {
        if (_callStackSize > 0) {
            return _callStack[--_callStackSize];
        } else {
            MessageManager::displayMessage("Call stack underflow", 5, WARN);
            return -1;
        }
    }

    static inline const int MAX_VARIABLES = 256;
    static inline float _register[MAX_VARIABLES];
    static inline std::string _strRegister[MAX_VARIABLES];


    static void skipStringLit(int& index, std::vector<int> bytecode);
};

#endif