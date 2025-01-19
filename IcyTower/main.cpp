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
        if (sprite.getPosition().x > 400) {
            sprite.setPosition(0, sprite.getPosition().y);
        }
        else if (sprite.getPosition().x < 0) {
            sprite.setPosition(400, sprite.getPosition().y);
        }

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

    bool isPaused = false;
    bool gameStarted = false;
    int checkForPause = 0;

    for (int i = 0; i < 5; ++i) {
        platforms.emplace_back("building.jpg", static_cast<float>(rand() % 300), 100.0f + i * 100);
    }


    sf::Font font;
    if (!font.loadFromFile("consolab.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    sf::Text startText, pauseText;
    startText.setFont(font);
    startText.setString("Press Up to Start");
    startText.setCharacterSize(30);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(50, 250);

    pauseText.setFont(font);
    pauseText.setString("Press Esc to resume");
    pauseText.setCharacterSize(30);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(50, 50);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameStarted && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up) {
                gameStarted = true; // Start the game when Enter is pressed
            }


            // PAUZA NIE DZIALA 

            //if (!isPaused && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            //    isPaused = !isPaused; // Start the game when Enter is pressed
            //}

            
           /* while (checkForPause % 2 == 1) {
                window.display();
                window.draw(pauseText);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                        checkForPause++;
                }
                continue;
            }*/

            

        }

        if (!gameStarted) {
            //window.clear();
            window.draw(startText);
            window.display();
            continue;
        }

        //if (isPaused) {
        //    //window.clear();
        //    window.draw(pauseText);
        //    window.display();
        //    continue;
        //}

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
                if (gameStarted) {
                    platform.move(0, 4);  // Move platforms down
                }
            }
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

