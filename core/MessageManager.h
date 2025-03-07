#ifndef _MESSAGE_MANAGER_H
#define _MESSAGE_MANAGER_H

#include <string>
#include <queue>
#include "Util.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Logger.h"

inline bool DISPLAY_DEBUG_MESSAGES = DEBUG_MODE;

struct MessageType {
    MessageType(std::string name, sf::Uint32 color) {
        this->name = name;
        this->color = color;
    }

    std::string name;
    sf::Uint32 color;
};

constexpr int NORMAL = 0;
constexpr int WARN = 1;
constexpr int ERR = 2;
constexpr int DEBUG = 3;
constexpr int SPECIAL = 4;
const MessageType MESSAGE_TYPES[5] = {
    MessageType("NORMAL", 0xFFFFFFFF),
    MessageType("WARN", 0xFFFF00FF),
    MessageType("ERR", 0xFF0000FF),
    MessageType("DEBUG", 0x00FF00FF),
    MessageType("SPECIAL", 0xFFD700FF)
};

struct Message {
    Message(std::string text, int timeout, int messageType) {
        this->text = text;
        this->timeout = timeout;
        this->messageType = messageType;

        queueTimeStamp = currentTimeMillis();
    }

    std::string text;
    int timeout;
    int messageType;
    int queueTimeStamp;

    bool active = true;
};

constexpr unsigned int PURGE_INTERVAL_SECONDS = 60;

class MessageManager {
public:
    static void start() {
        _isHalted = false;

        std::thread managementThread(MessageManager::manageMessages);
        managementThread.detach();
    }

    static void stop() {
        _isHalted = true;
    }

    static void displayMessage(std::string text, int timeout, int messageType = NORMAL) {
        if (messageType != NORMAL && messageType != SPECIAL) text = "[" + MESSAGE_TYPES[messageType].name + "] " + text;
        std::cout << text << std::endl;
        Logger::log(text);

        std::shared_ptr<Message> message = std::shared_ptr<Message>(new Message(text, timeout, messageType));
        _messages.push_back(message);

        std::thread deactivationThread(&MessageManager::scheduleMessageDeactivation, message);
        deactivationThread.detach();
    }

    static void displayMessage(int message, int timeout, int messageType = NORMAL) {
        displayMessage(std::to_string(message), timeout, messageType);
    }

    static void displayMessage(float message, int timeout, int messageType = NORMAL) {
        displayMessage(std::to_string(message), timeout, messageType);
    }

    static void displayMessage(double message, int timeout, int messageType = NORMAL) {
        displayMessage(std::to_string(message), timeout, messageType);
    }

    static void displayMessage(long message, int timeout, int messageType = NORMAL) {
        displayMessage(std::to_string(message), timeout, messageType);
    }

    static void displayMessage(long long message, int timeout, int messageType = NORMAL) {
        displayMessage(std::to_string(message), timeout, messageType);
    }

    static void displayMessage(unsigned int message, int timeout, int messageType = NORMAL) {
        displayMessage(std::to_string(message), timeout, messageType);
    }

    static std::vector<std::shared_ptr<Message>> getMessages() {
        return _messages;
    }

    static int getMessageCount() {
        int messageCount = 0;
        for (auto& message : getMessages()) {
            if (message->active && !(message->messageType == DEBUG && !DISPLAY_DEBUG_MESSAGES)) messageCount++;
        }

        return messageCount;
    }

    static bool isPurging() {
        return _isPurging;
    }

private:
    inline static std::vector<std::shared_ptr<Message>> _messages;

    inline static bool _isHalted = false;
    inline static bool _isPurging = false;

    static void manageMessages() {
        while (!_isHalted) {
            std::this_thread::sleep_for(std::chrono::milliseconds(PURGE_INTERVAL_SECONDS * 1000));

            _isPurging = true;
            for (int i = 0; i < _messages.size(); i++) {
                auto& message = _messages[i];
                if (!message->active) {
                    _messages.erase(_messages.begin() + i);
                }
            }
            _isPurging = false;
        }
    }

    static void scheduleMessageDeactivation(std::shared_ptr<Message> message) {
        std::this_thread::sleep_for(std::chrono::milliseconds(message->timeout * 1000));
        message->active = false;
    }
};

#endif
