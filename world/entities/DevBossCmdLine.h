#ifndef _DEV_BOSS_CMD_LINE
#define _DEV_BOSS_CMD_LINE

#include <string>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

class DevBossCmdLine {
public:
    DevBossCmdLine();

    void update(const sf::Vector2f bossPos);
    void draw(sf::RenderTexture& surface);

    void typeCommand(const std::string text);

    bool isActive() const;
private:
    sf::Vector2f _pos;

    sf::RectangleShape _bg;
    sf::Font _font;
    sf::Text _displayText;

    bool _isActive = false;

    std::string _currentCommandString = "";
    unsigned int _currentCommandIndex = 0;
    long long _lastCharacterTypeTime = 0LL;
};

#endif