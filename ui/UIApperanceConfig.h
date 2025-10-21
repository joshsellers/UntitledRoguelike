#ifndef _UI_APPEARANCE_CONFIG_H
#define _UI_APPEARANCE_CONFIG_H

#include <SFML/Graphics/Rect.hpp>

struct UIAppearanceConfig {
    UIAppearanceConfig(float pixelSize, sf::IntRect topLeftCorner, sf::IntRect leftEdge, sf::IntRect bottomLeftCorner,
        sf::IntRect centerTop, sf::IntRect center, sf::IntRect centerBottom,
        sf::IntRect topRightCorner, sf::IntRect rightEdge, sf::IntRect bottomRightCorner) {
    this->topLeftCorner = topLeftCorner;
    this->leftEdge = leftEdge;
    this->bottomLeftCorner = bottomLeftCorner;

    this->centerTop = centerTop;
    this->center = center;
    this->centerBottom = centerBottom;

    this->topRightCorner = topRightCorner;
    this->rightEdge = rightEdge;
    this->bottomRightCorner = bottomRightCorner;

    this->pixelSize = pixelSize;
}

    sf::IntRect topLeftCorner;
    sf::IntRect leftEdge;
    sf::IntRect bottomLeftCorner;

    sf::IntRect centerTop;
    sf::IntRect center;
    sf::IntRect centerBottom;

    sf::IntRect topRightCorner;
    sf::IntRect rightEdge;
    sf::IntRect bottomRightCorner;

    // How big a pixel of the texture should appear on-screen
    float pixelSize;

    UIAppearanceConfig offsetBy(int x, int y) {
        return {
            pixelSize,
            {topLeftCorner.left + x, topLeftCorner.top + y, topLeftCorner.width, topLeftCorner.height},
            {leftEdge.left + x, leftEdge.top + y, leftEdge.width, leftEdge.height},
            {bottomLeftCorner.left + x, bottomLeftCorner.top + y, bottomLeftCorner.width, bottomLeftCorner.height},
            {centerTop.left + x, centerTop.top + y, centerTop.width, centerTop.height},
            {center.left + x, center.top + y, center.width, center.height},
            {centerBottom.left + x, centerBottom.top + y, centerBottom.width, centerBottom.height},
            {topRightCorner.left + x, topRightCorner.top + y, topRightCorner.width, topRightCorner.height},
            {rightEdge.left + x, rightEdge.top + y, rightEdge.width, rightEdge.height},
            {bottomRightCorner.left + x, bottomRightCorner.top + y, bottomRightCorner.width, bottomRightCorner.height},
        };
    }
};

#endif