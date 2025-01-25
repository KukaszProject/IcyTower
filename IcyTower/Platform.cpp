#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>

class Platform {
public:
    sf::Sprite sprite, firstPlatformSprite;
    static sf::Texture texture1;
    static sf::Texture texture2;
    static sf::Texture texture3;
    static sf::Texture firstPlatform;

    Platform(float x, float y) {
        int randomTexture = rand() % 3;
        if (randomTexture == 0) {
            sprite.setTexture(texture1);
        }
        else if (randomTexture == 1) {
            sprite.setTexture(texture2);
        }
        else {
            sprite.setTexture(texture3);
        }
        sprite.setPosition(x, y);
    }

    void move(float dx, float dy) {
        sprite.move(dx, dy);
    }

    bool isOutOfScreen(float windowHeight) {
        return sprite.getPosition().y > windowHeight;
    }

    static bool loadTextures(const std::string& textureFile1, const std::string& textureFile2, const std::string& textureFile3) {
        if (!texture1.loadFromFile(textureFile1) ||
            !texture2.loadFromFile(textureFile2) ||
            !texture3.loadFromFile(textureFile3)) {
            std::cerr << "Failed to load platform textures!" << std::endl;
            return false;
        }
        return true;
    }
};