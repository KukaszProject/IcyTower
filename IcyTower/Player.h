#include <SFML/Graphics.hpp>
using namespace std;

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;

    float velocityY = 0.0f;
    bool isJumping = false;
    float gravity = 0.5f;
    float jumpStrength = -12.0f;
    float speed = 5.0f;

    Player(const string& textureFile) {
        if (!texture.loadFromFile("hero.png")) {
            //cout << "Failed to load player texture!" << endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(200, 500);  // Start position
    }

    void jump() {
        if (!isJumping) {
            velocityY = jumpStrength;
            isJumping = true;
        }
    }

    void move(float dirX) {
        sprite.move(dirX * speed, 0);
    }

    void update() {
        velocityY += gravity;
        sprite.move(0, velocityY);

        // Collision with the bottom of the screen
        if (sprite.getPosition().y > 500) {
            sprite.setPosition(sprite.getPosition().x, 500);
            isJumping = false;
        }
    }
};
