#include "MessageManager.h"
#include "SoundManager.h"

void MessageManager::start() {
    _isHalted = false;

    std::thread managementThread(MessageManager::manageMessages);
    managementThread.detach();
}

void MessageManager::stop() {
    _isHalted = true;
}

void MessageManager::displayMessage(std::string text, int timeout, int messageType, std::string sound, int delay) {
    if (messageType != NORMAL && messageType != SPECIAL && messageType != TUTORIAL) text = "[" + MESSAGE_TYPES[messageType].name + "] " + text;
    std::cout << text << std::endl;
    Logger::log(text);

    if (messageType == SPECIAL && sound == "message") sound = "itemunlock";

    std::shared_ptr<Message> message = std::shared_ptr<Message>(new Message(text, timeout, messageType, sound, delay));
    if (delay != 0) message->active = false;
    _messages.push_back(message);

    if (messageType != DEBUG && !((message->messageType == WARN || message->messageType == ERR) && !DISPLAY_DEBUG_MESSAGES) 
        && delay == 0 && sound != "NONE" && timeout != 0) SoundManager::playSound(sound);

    if (messageType != TUTORIAL) {
        std::thread deactivationThread(&MessageManager::scheduleMessageDeactivation, message);
        deactivationThread.detach();
    }
}

void MessageManager::displayMessage(int message, int timeout, int messageType, std::string sound, int delay) {
    displayMessage(std::to_string(message), timeout, messageType, sound, delay);
}

void MessageManager::displayMessage(float message, int timeout, int messageType, std::string sound, int delay) {
    displayMessage(std::to_string(message), timeout, messageType, sound, delay);
}

void MessageManager::displayMessage(double message, int timeout, int messageType, std::string sound, int delay) {
    displayMessage(std::to_string(message), timeout, messageType, sound, delay);
}

void MessageManager::displayMessage(long message, int timeout, int messageType, std::string sound, int delay) {
    displayMessage(std::to_string(message), timeout, messageType, sound, delay);
}

void MessageManager::displayMessage(long long message, int timeout, int messageType, std::string sound, int delay) {
    displayMessage(std::to_string(message), timeout, messageType, sound, delay);
}

void MessageManager::displayMessage(unsigned int message, int timeout, int messageType, std::string sound, int delay) {
    displayMessage(std::to_string(message), timeout, messageType, sound, delay);
}

std::vector<std::shared_ptr<Message>> MessageManager::getMessages() {
    return _messages;
}

int MessageManager::getMessageCount() {
    int messageCount = 0;
    for (auto& message : getMessages()) {
        if (message->active && !(message->messageType == DEBUG && !DISPLAY_DEBUG_MESSAGES)) messageCount++;
    }

    return messageCount;
}

bool MessageManager::isPurging() {
    return _isPurging;
}

void MessageManager::clearTutorialMessage(int tutorialStepId) {
    for (int i = 0; i < _messages.size(); i++) {
        if (_messages[i]->active && _messages[i]->messageType == TUTORIAL && _messages[i]->timeout == tutorialStepId) {
            _messages[i]->active = false;
            break;
        }
    }
}

void MessageManager::clearAllTutorialMessages() {
    for (int i = 0; i < _messages.size(); i++) {
        if (_messages[i]->messageType == TUTORIAL) {
            _messages[i]->active = false;
        }
    }
}

void MessageManager::manageMessages() {
    while (!_isHalted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(PURGE_INTERVAL_SECONDS * 1000));

        _isPurging = true;
        for (int i = 0; i < _messages.size(); i++) {
            auto& message = _messages[i];
            if (!message->active && message->delay == 0) {
                _messages.erase(_messages.begin() + i);
            }
        }
        _isPurging = false;
    }
}

void MessageManager::scheduleMessageDeactivation(std::shared_ptr<Message> message) {
    if (message->delay != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(message->delay * 1000));
        message->active = true;
        message->delay = 0;
        if (message->messageType != DEBUG && message->sound != "NONE") SoundManager::playSound(message->sound);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(message->timeout * 1000));
    message->active = false;
}