#ifndef _LOGGER_H
#define _LOGGER_H

#include <string>
#include <queue>
#include <thread>
#include <fstream>
#include <filesystem>
#include <iostream>

constexpr float LOG_WRITE_INTERVAL_SECONDS = 1.f;

class Logger {
public:
    static void start() {
        _isHalted = false;

        try {
            if (!std::filesystem::remove(_logFileName)) {
                std::cout << "Could not remove log file because it does not exist" << std::endl;
            }
        } catch (const std::filesystem::filesystem_error& err) {
            std::cout << "Could not remove log file: " << err.what() << std::endl;
        }

        try {
            _outStream.open(_logFileName);
        } catch (std::exception ex) {
            std::cout << "Logging error: " << ex.what() << std::endl;
        }

        std::thread loggingThread(Logger::run);
        loggingThread.detach();
    }

    static void stop() {
        _isHalted = true;
        _outStream.close();
    }
    
    static void log(std::string message) {
        const auto now = std::chrono::system_clock::now();
        const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
        std::string timeString = std::ctime(&t_c);
        timeString = timeString.substr(0, timeString.length() - 5);
        timeString += "@" + std::to_string(currentTimeMillis());

        message = "(" + timeString + (std::string)") " + message;

        if (!_isWriting) _messageQueue.push(message);
        else _deferredMessageQueue.push(message);
    }

private:
    inline static std::queue<std::string> _messageQueue;
    inline static std::queue<std::string> _deferredMessageQueue;

    inline static bool _isHalted = false;
    inline static bool _isWriting = false;

    inline static const std::string _logFileName = "rolmi.log";
    inline static std::ofstream _outStream;

    static void run() {
        while (!_isHalted) {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(LOG_WRITE_INTERVAL_SECONDS * 1000.f)));

            _isWriting = true;
            try {
                while (!_messageQueue.empty()) {
                    _outStream << _messageQueue.front() << std::endl;
                    _messageQueue.pop();
                }
                _isWriting = false;

                while (!_deferredMessageQueue.empty()) {
                    _outStream << _deferredMessageQueue.front() << std::endl;
                    _deferredMessageQueue.pop();
                }

                _outStream.flush();

            } catch (std::exception ex) {
                std::cout << "Logging error: " << ex.what() << std::endl;
            }
        }
    }
};

#endif