#include "UITextField.h"
#include "UIHandler.h"

UITextField::UITextField(const sf::String label, float x, float y, sf::Font font, bool centerOnCoords) 
    : UIElement(x, y, 1, 1, false, false, font, centerOnCoords, true) {
    //_disableAutomaticTextAlignment = true;

    setAppearance(TEXTFIELD_CONFIG, centerOnCoords);

    float fontSize = 1;
    int relativeFontSize = getRelativeWidth(fontSize);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color(0xFFFFFFFF));

    _label.setString(label);
    _label.setFont(_font);
    _label.setCharacterSize(relativeFontSize);
    _label.setFillColor(sf::Color(0xFFFFFFFF));

    _width = _text.getGlobalBounds().width;

    _rTexture.create(100, 16 * 3);
    _rTexture.display();

    _defaultTexture = sf::IntRect(0, 0, _width, 16);
    _hoverTexture = sf::IntRect(0, 16, _width, 16);
    _clickTexture = sf::IntRect(0, 32, _width, 16);

    _shape.setSize(sf::Vector2f(_width, _height));
    _shape.setTexture(&_rTexture.getTexture());
    _shape.setTextureRect(sf::IntRect(0, 0, _width, 16)); 
    
    _padding = getRelativeWidth(1.f);
    _origin = { _pos.x + _size.x / 2.f + _padding, _pos.y + _size.y / 2.f };
}

void UITextField::update() {
    if (!_isSelected && _lastSelected) _isArmed = false;
    _lastSelected = _isSelected;
}

void UITextField::draw(sf::RenderTexture& surface) {
    _defaultTexture = sf::IntRect(0, 0, _width, 16);
    _hoverTexture = sf::IntRect(0, 16, _width, 16);
    _clickTexture = sf::IntRect(0, 32, _width, 16);

    if (((getBounds().contains(_mPos.x, _mPos.y) && !_mouseDown) || _isSelected) && !_isArmed) {
        setAppearance(TEXTFIELD_HOVER_CONFIG);
    } else if ((!_mouseDown || !getBounds().contains(_mPos.x, _mPos.y)) && !_isArmed) {
        setAppearance(TEXTFIELD_CONFIG);
    } else if (_isArmed) {
        setAppearance(TEXTFIELD_ARMED_CONFIG);
    }

    _size.x = _text.getGlobalBounds().width + _padding * 2;
    _size.y = std::max(3.f, _text.getGlobalBounds().height);

    _pos.x = _origin.x - _size.x / 2.f - _padding;
    _pos.y = _origin.y - _size.y / 2.f;

    float padding = getRelativeWidth(1.f);

    float width = _text.getGlobalBounds().width;
    const bool clampWidth = width < 165;
    if (clampWidth) width = getRelativeWidth(8.f);
    float height = _text.getGlobalBounds().height;

    float x = getRelativeWidth(_x) - width / 2;
    float y = getRelativeHeight(_y);
    //_text.setPosition(sf::Vector2f(clampWidth ? getRelativeWidth(_x) - _text.getGlobalBounds().width / 2 : x, y));
    //_label.setPosition(sf::Vector2f(_pos.x + getBounds().width / 2.f + (_label.getGlobalBounds().width / 2), _pos.y - getBounds().height));
    _label.setOrigin(_label.getLocalBounds().width / 2.f + _label.getLocalBounds().left, _label.getLocalBounds().height / 2.f + _label.getLocalBounds().top);
    const sf::FloatRect bounds = getBounds();
    const float w = bounds.width;
    const float h = bounds.height;
    _label.setPosition(
        bounds.left + w / 2.f,
        bounds.top - h / 2.f
    );

    _rTexture.create(width, 16 * 3);
    sf::RectangleShape center;
    center.setPosition(16, 1);
    center.setSize(sf::Vector2f(width - 16 * 2, 14));
    center.setTexture(UIHandler::getUISpriteSheet().get());
    center.setTextureRect(sf::IntRect(112, 1, 1, 14));
    _rTexture.draw(center);
    center.setPosition(16, 17);
    _rTexture.draw(center);
    center.setPosition(16, 33);
    _rTexture.draw(center);

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape borderLeft;
        borderLeft.setPosition(0, 16 * i);
        borderLeft.setSize(sf::Vector2f(16, 16));
        borderLeft.setTexture(UIHandler::getUISpriteSheet().get());
        borderLeft.setTextureRect(sf::IntRect(128 + 32 * i, 0, 4, 16));
        _rTexture.draw(borderLeft);
    }

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape borderRight;
        borderRight.setPosition(width - 17, 16 * i);
        borderRight.setSize(sf::Vector2f(16, 16));
        borderRight.setTexture(UIHandler::getUISpriteSheet().get());
        borderRight.setTextureRect(sf::IntRect(144 + 32 * i, 0, 4, 16));
        _rTexture.draw(borderRight);
    }

    _rTexture.display();

    _shape.setSize(sf::Vector2f(width + padding * 2, getRelativeHeight(5.f)));
    _shape.setPosition(x - padding, y - padding);

    _width = width;

    if (_isArmed) {
        sf::Text cursor;
        cursor.setString(" |");
        cursor.setFont(*_text.getFont());
        cursor.setCharacterSize(_text.getCharacterSize() + getRelativeWidth(0.5f));
        cursor.setFillColor(_text.getFillColor());
        cursor.setOrigin(cursor.getLocalBounds().width / 2.f + cursor.getLocalBounds().left, cursor.getLocalBounds().height / 2.f + cursor.getLocalBounds().top);
        cursor.setPosition(_text.getPosition().x + _text.getGlobalBounds().width / 2.f, _text.getPosition().y);
        constexpr unsigned int blinkRate = 24;
        if ((_cursorBlinkTimer / blinkRate) % 2) surface.draw(cursor);

        _cursorBlinkTimer++;
    }
    //surface.draw(_shape);
    surface.draw(_text);
    surface.draw(_label);
}

void UITextField::setDefaultText(sf::String text) {
    _text.setString(text);
}

void UITextField::mouseButtonPressed(const int mx, const int my, const int button) {
    _mouseDown = true;
}

void UITextField::mouseButtonReleased(const int mx, const int my, const int button) {
    _isArmed = getBounds().contains(mx, my);
    _mouseDown = false;
}

void UITextField::controllerButtonPressed(GAMEPAD_BUTTON button) {
    if (_isSelected && button == GAMEPAD_BUTTON::A) _mouseDown = true;
}

void UITextField::controllerButtonReleased(GAMEPAD_BUTTON button) {
    if (_isSelected && button == GAMEPAD_BUTTON::A) {
        _mouseDown = false;
        _isArmed = true;

        if (_listener != nullptr) _listener->buttonPressed("textfieldarmedbygamepad:" + getId());
    }
}

void UITextField::mouseMoved(const int mx, const int my) {
    _mPos.x = mx;
    _mPos.y = my;
}

void UITextField::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UITextField::keyReleased(sf::Keyboard::Key& key) {
}

void UITextField::textEntered(const sf::Uint32 character) {
    if (_isArmed) {
        sf::String userInput = _text.getString();
        if (character == '\b' && userInput.getSize()) {
            userInput.erase(userInput.getSize() - 1, 1);
        } else if (character != '\b') {
            userInput += character;
        }
        _text.setString(userInput);
    }
}

std::string UITextField::getText() const {
    return ((std::string)_text.getString());
}

void UITextField::setCharacterSize(float size) {
    _text.setCharacterSize(getRelativeWidth(size));
    _label.setCharacterSize(getRelativeWidth(size));
}

void UITextField::setId(std::string id) {
    _id = id;
}

const std::string UITextField::getId() const {
    return _id;
}

void UITextField::setListener(UIButtonListener* listener) {
    _listener = listener;
}
