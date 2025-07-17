#ifndef _END_GAME_SEQUENCE_H
#define _END_GAME_SEQUENCE_H

#include <vector>
#include <memory>
#include "EndGameSequenceListener.h"
#include <SFML/Graphics.hpp>

class EndGameSequence {
public:
    static void start();

    static void end();

    static void update();
    static void draw(sf::RenderTexture& surface);

    static bool isActive();
    static bool fadeComplete();

    static void addListener(std::shared_ptr<EndGameSequenceListener> listener);
private:
    inline static std::vector<std::shared_ptr<EndGameSequenceListener>> _listeners;

    inline static bool _isActive = false;
    inline static bool _fadeComplete = false;

    inline static sf::Font _font;
    inline static std::vector<std::string> _creditsStrings;
    inline static sf::Text _creditsLabel;
    inline static float _scrollPosY = 102;

    static void loadCreditsText();

    inline static unsigned int _bgAlpha;

    inline static long long _startTimeMillis = 0;
    inline static long long _lastFadeIncrease = 0;
};

#endif