//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include "Player.h"
//#include "Platform.h"
//
//bool checkCollision(Player& player, Platform& platform) {
//    sf::FloatRect playerBounds = player.sprite.getGlobalBounds();
//    sf::FloatRect platformBounds = platform.sprite.getGlobalBounds();
//
//    if (playerBounds.intersects(platformBounds) && player.velocityY > 0) {
//        player.isJumping = false;
//        player.velocityY = 0;
//        player.sprite.setPosition(player.sprite.getPosition().x, platform.sprite.getPosition().y - playerBounds.height);
//        return true;
//    }
//
//    return false;
//}
//
//
//int main()
//{
//    sf::RenderWindow window(sf::VideoMode(400, 600), "Icy Tower");
//    window.setFramerateLimit(60);
//    sf::Texture backgroundTexture;
//    backgroundTexture.loadFromFile("bg.jpg");
//
//    sf::Sprite backgroundSprite;
//    backgroundSprite.setTexture(backgroundTexture);
//
//    // Create a player and platforms
//    Player player("hero.png");
//    Platform platform1("building.jpg", 100, 420);
//    Platform platform2("building.jpg", 200, 550);
//    Platform platform3("building.jpg", 250, 300);
//
//    std::vector<Platform> platforms = { platform1, platform2, platform3 };
//
//    while (window.isOpen())
//    {
//        sf::Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//
//        // Handle player input
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
//            player.move(-1);
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
//            player.move(1);
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
//            player.jump();
//
//        // Update player and check collisions
//        player.update();
//        for (auto& platform : platforms) {
//            checkCollision(player, platform);
//        }
//
//        // Render everything
//        window.clear();
//        window.draw(backgroundSprite);
//        window.draw(player.sprite);
//        for (auto& platform : platforms) {
//            window.draw(platform.sprite);
//        }
//        window.display();
//    }
//
//    return 0;
//}
//

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

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY = 0.0f;
    bool isJumping = false;
    float gravity = 0.5f;
    float jumpStrength = -12.0f;
    float speed = 5.0f;

    Player(const std::string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load player texture!" << std::endl;
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



bool checkCollision(Player& player, Platform& platform) {
    sf::FloatRect playerBounds = player.sprite.getGlobalBounds();
    sf::FloatRect platformBounds = platform.sprite.getGlobalBounds();

    if (playerBounds.intersects(platformBounds) && player.velocityY > 0) {
        player.isJumping = false;
        player.velocityY = 0;
        player.sprite.setPosition(player.sprite.getPosition().x, platform.sprite.getPosition().y - playerBounds.height);
        return true;
    }

    return false;
}

void generatePlatforms(std::vector<Platform>& platforms, sf::Texture& platformTexture, float windowHeight) {
    // Generate a platform at random X positions above the window
    float x = static_cast<float>(rand() % 350); // Random x position
    float y = -20; // Just above the screen
    platforms.emplace_back("building.jpg", x, y);
}

int main()
{
    srand(static_cast<unsigned>(time(0))); // Seed random generator

    sf::RenderWindow window(sf::VideoMode(400, 600), "Icy Tower");
    window.setFramerateLimit(60);
    sf::Texture backgroundTexture;
        backgroundTexture.loadFromFile("bg.jpg");
    
        sf::Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);

    // Create a player and initialize platforms
    Player player("hero.png");
    std::vector<Platform> platforms;

    // Load platform texture and generate initial platforms
    sf::Texture platformTexture;
    if (!platformTexture.loadFromFile("building.jpg")) {
        std::cerr << "Error loading platform texture!" << std::endl;
        return -1;
    }

    bool isGameStarted = false;

    for (int i = 0; i < 5; ++i) {
        platforms.emplace_back("building.jpg", static_cast<float>(rand() % 300), 100.0f + i * 100);
    }

    

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        /*if (!isGameStarted) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                isGameStarted = true; // Start the game when Enter is pressed
            }
            // Draw the "Press Enter to Start" message or other UI elements
            window.clear();
            sf::Font font;
            if (!font.loadFromFile("arial.ttf")) {  // Use any font you have
                std::cerr << "Error loading font!" << std::endl;
                return -1;
            }

            sf::Text text;
            text.setFont(font);
            text.setString("Press Enter to Start");
            text.setCharacterSize(30);
            text.setFillColor(sf::Color::White);
            text.setPosition(50, 250);

            window.draw(text); // Display the start message
            window.display();// Skip the rest of the loop and wait for Enter key press
        }*/

        // Handle player input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.move(-1);
            player.texture.loadFromFile("hero_j_l.png");
            player.sprite.setTexture(player.texture);
        }
            
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.move(1);
            player.texture.loadFromFile("hero_j_r.png");
            player.sprite.setTexture(player.texture);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            player.jump();

        // Update player and platforms
        player.update();

        for (auto& platform : platforms) {
            checkCollision(player, platform);
        }

        // Generate new platforms if needed
        if (platforms.back().sprite.getPosition().y > 120) {
            generatePlatforms(platforms, platformTexture, window.getSize().y);
        }

        // Remove platforms that have moved off the screen
        platforms.erase(std::remove_if(platforms.begin(), platforms.end(),
            [&window](Platform& p) { return p.isOutOfScreen(window.getSize().y); }),
            platforms.end());

        // Move platforms down as the player moves up
        if (player.sprite.getPosition().y < 600) {
            for (auto& platform : platforms) {
                platform.move(0, 4);  // Move platforms down
            }
        }

        if (player.sprite.getPosition().y == 570)
        {
            isGameStarted = false;
        }

        // Render everything
        window.clear();
        window.draw(backgroundSprite);
        window.draw(player.sprite);
        for (auto& platform : platforms) {
            window.draw(platform.sprite);
        }
        window.display();
    }

    return 0;
}

