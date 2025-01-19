#include <SFML/Graphics.hpp>
using namespace std;

class Platform {
public:
    sf::Sprite sprite;
    sf::Texture texture;

    Platform(const std::string& textureFile, float x, float y) {
        if (!texture.loadFromFile("building.jpg")) {
            //std::cerr << "Failed to load platform texture!" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void update() {
        // Logic for moving platforms can be added later
    }
};