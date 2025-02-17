#ifndef _VIEWPORT_H
#define _VIEWPORT_H

#include "SFML/Graphics/View.hpp"
#include "Globals.h"

class Viewport {
public:
    static void setCamera(sf::View* camera) {
        _camera = camera;
    }

    static void setWindow(sf::RenderWindow* window) {
        _window = window;
    }

    static void setCullingEnabled(bool enabled) {
        _enableCulling = enabled;
    }

    static bool isCullingEnabled() {
        return _enableCulling;
    }

    static sf::FloatRect getBounds() {
        sf::FloatRect bounds;
        
        if (isCullingEnabled()) {
            bounds.left = _camera->getCenter().x - WIDTH / 2;
            bounds.top = _camera->getCenter().y - HEIGHT / 2;
            bounds.width = WIDTH;
            bounds.height = HEIGHT;
        } else {
            sf::Vector2f camCoords = _window->mapPixelToCoords(sf::Vector2i(0, 0), *_camera);
            sf::Vector2f camBounds = _camera->getSize();
            bounds.left = camCoords.x;
            bounds.top = camCoords.y;
            bounds.width = camBounds.x;
            bounds.height = camBounds.y;
        }
        return bounds;
    }
private:
    static inline sf::View* _camera;
    static inline sf::RenderWindow* _window;

    static inline bool _enableCulling = true;
};

#endif