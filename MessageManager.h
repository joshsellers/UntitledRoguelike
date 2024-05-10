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

inline bool DISPLAY_DEBUG_MESSAGES = false;

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
const MessageType MESSAGE_TYPES[4] = {
    MessageType("NORMAL", 0xFFFFFFFF),
    MessageType("WARN", 0xFFFF00FF),
    MessageType("ERR", 0xFF0000FF),
    MessageType("DEBUG", 0x00FF00FF)
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
        std::thread managementThread(MessageManager::manageMessages);
        managementThread.detach();
    }

    static void displayMessage(std::string text, int timeout, int messageType = NORMAL) {
        if (messageType != NORMAL) text = "[" + MESSAGE_TYPES[messageType].name + "] " + text;
        std::cout << text << std::endl;

        std::shared_ptr<Message> message = std::shared_ptr<Message>(new Message(text, timeout, messageType));
        _messages.push_back(message);

        std::thread deactivationThread(&MessageManager::scheduleMessageDeactivation, message);
        deactivationThread.detach();
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

private:
    inline static std::vector<std::shared_ptr<Message>> _messages;

    inline static bool _isHalted = false;

    static void manageMessages() {
        while (!_isHalted) {
            std::this_thread::sleep_for(std::chrono::milliseconds(PURGE_INTERVAL_SECONDS * 1000));

            for (int i = 0; i < _messages.size(); i++) {
                auto& message = _messages[i];
                if (!message->active) {
                    _messages.erase(_messages.begin() + i);
                }
            }
        }
    }

    static void scheduleMessageDeactivation(std::shared_ptr<Message> message) {
        std::this_thread::sleep_for(std::chrono::milliseconds(message->timeout * 1000));
        message->active = false;
    }
};

#endif
