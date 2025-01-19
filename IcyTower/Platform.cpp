#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

class Platform {
public:
    sf::Sprite sprite;
    sf::Texture texture;

    Platform(const std::string& textureFile, float x, float y) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load platform texture!" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void move(float dx, float dy) {
        sprite.move(dx, dy);
    }

    bool isOutOfScreen(float windowHeight) {
        return sprite.getPosition().y > windowHeight;
    }
};