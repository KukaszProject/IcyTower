#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

enum GameState { MENU, PRESTART, GAME, PAUSE, GAME_OVER };

// Difficulty settings
enum Difficulty { EASY, MEDIUM, HARD };

class Platform {
public:
    sf::Sprite sprite;
    static sf::Texture texture;

    Platform(float x, float y) {
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void move(float dx, float dy) {
        sprite.move(dx, dy);
    }

    bool isOutOfScreen(float windowHeight) {
        return sprite.getPosition().y > windowHeight;
    }

    static bool loadTexture(const std::string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load platform texture!" << std::endl;
            return false;
        }
        return true;
    }
};

sf::Texture Platform::texture;

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY = 0.0f;
    bool isJumping = false;
    float gravity = 0.5f;
    float jumpStrength = -12.0f;
    float speed = 5.0f;
    float startingY = 500.0f; // Starting Y position of the player


    int platformCount = 0;

    Player(const std::string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load player texture!" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(200, startingY);  // Start position
    }

    void jump(Difficulty dif) {
        if (dif == EASY)
        {
            if (!isJumping) {
                velocityY = jumpStrength;
                isJumping = true;
            }
        }
        else if (dif == MEDIUM)
        {
            if (!isJumping) {
                velocityY = jumpStrength * .8;
                isJumping = true;
            }
        }
        else if (dif == HARD)
        {
            if (!isJumping) {
                velocityY = jumpStrength * .5;
                isJumping = true;
            }
        }

    }

    void move(float dirX, Difficulty dif) {
        if (dif == EASY)
        {
            sprite.move(dirX * speed, 0);
        }
        else if (dif == MEDIUM)
        {
            sprite.move(dirX * speed * .8, 0);
        }
        else if (dif == HARD)
        {
            sprite.move(dirX * speed * .5, 0);
        }
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

        if (sprite.getPosition().y > startingY) {
            sprite.setPosition(sprite.getPosition().x, startingY);
            isJumping = false;
        }
        
    }

    // Check if the player fell below the starting Y position
    bool fellFromPlatform() {
        return sprite.getPosition().y >= startingY;
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


class Background {
public:
    sf::Sprite sprite1, sprite2;
    static sf::Texture texture;

    Background(float windowHeight) {
        sprite1.setTexture(texture);
        sprite2.setTexture(texture);

        // Set the initial positions for the two backgrounds
        sprite1.setPosition(0, 0);
        sprite2.setPosition(0, +windowHeight);
    }

    static bool loadTexture(const std::string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load background texture!" << std::endl;
            return false;
        }
        return true;
    }
    void move(float dy, float windowHeight) {
        // Move both backgrounds
        sprite1.move(0, dy);
        sprite2.move(0, dy);

        // If background 1 moves off-screen, reset it
        if (sprite1.getPosition().y >= windowHeight) {
            sprite1.setPosition(0, sprite2.getPosition().y - windowHeight);
        }

        // If background 2 moves off-screen, reset it
        if (sprite2.getPosition().y >= windowHeight) {
            sprite2.setPosition(0, sprite1.getPosition().y - windowHeight);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite1);
        window.draw(sprite2);
    }
};

void generatePlatforms(std::vector<Platform>& platforms, float windowHeight, Difficulty difficulty, Player player) {
    float x = static_cast<float>(rand() % 300); // Random x position
    float y = -20; // Just above the screen

    if (difficulty == EASY) {
        platforms.emplace_back(x, y);
        player.platformCount++;
        player.speed = 5.0f;
        player.jumpStrength = 15.0f;
        player.gravity = .5f;
    }
    else if (difficulty == MEDIUM) {
        platforms.emplace_back(x, y);
        player.platformCount++;
        player.speed = 3.0f;
        player.jumpStrength = 12.0f;
        player.gravity = .6f;
    }
    else if (difficulty == HARD) {
        platforms.emplace_back(x, y);
        player.platformCount++;
        player.speed = 3.0f;
        player.jumpStrength = 10.0f;
        player.gravity = .7f;
    }
}

sf::Texture Background::texture;

int main() {
    srand(static_cast<unsigned>(time(0)));
    clock_t start;
    double duration;
    start = clock();
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;

    sf::RenderWindow window(sf::VideoMode(400, 600), "Icy Tower");
    window.setFramerateLimit(60);

    if (!Background::loadTexture("bg.png")) {
        return -1;
    }

    Background background(window.getSize().y);

    if (!Platform::loadTexture("platform.png")) {
        return -1;
    }

    Player player("hero.png");
    std::vector<Platform> platforms;

    GameState gameState = MENU;  // Start with the menu
    Difficulty difficulty = EASY;

    // Font for menu and game text
    sf::Font font;
    if (!font.loadFromFile("consolab.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    // Menu items
    sf::Text menuTitle, easyOption, mediumOption, hardOption, gameOverText, startMessage;
    menuTitle.setFont(font);
    menuTitle.setString("Icy Tower");
    menuTitle.setCharacterSize(50);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition(100, 100);

    easyOption.setFont(font);
    easyOption.setString("Easy");
    easyOption.setCharacterSize(25);
    easyOption.setPosition(150, 200);

    mediumOption.setFont(font);
    mediumOption.setString("Medium");
    mediumOption.setCharacterSize(25);
    mediumOption.setPosition(150, 250);

    hardOption.setFont(font);
    hardOption.setString("Hard");
    hardOption.setCharacterSize(25);
    hardOption.setPosition(150, 300);

    startMessage.setFont(font);
    startMessage.setString("Press any key to start.");
    startMessage.setCharacterSize(20);
    startMessage.setFillColor(sf::Color::White);
    startMessage.setPosition(60, 500);

    gameOverText.setFont(font);
    gameOverText.setString("Game Over! Press Enter to return to the Menu.");
    gameOverText.setCharacterSize(20);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(50, 300);

    int selectedOption = 0;

    int score = 0;

    // Pause menu items
    sf::Text resumeOption, mainMenuOption, scoreText;
    resumeOption.setFont(font);
    resumeOption.setString("Resume");
    resumeOption.setCharacterSize(25);
    resumeOption.setPosition(150, 100);

    mainMenuOption.setFont(font);
    mainMenuOption.setString("Main Menu");
    mainMenuOption.setCharacterSize(25);
    mainMenuOption.setPosition(150, 150);

    scoreText.setFont(font);
    scoreText.setString("Score" + score);
    scoreText.setCharacterSize(25);
    scoreText.setPosition(10, 20);


    int pauseMenuOption = 0;  // 0 = Resume, 1 = Main Menu

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameState == MENU) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedOption = (selectedOption - 1 + 3) % 3;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        selectedOption = (selectedOption + 1) % 3;
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        if (selectedOption == 0) difficulty = EASY;
                        else if (selectedOption == 1) difficulty = MEDIUM;
                        else if (selectedOption == 2) difficulty = HARD;

                        platforms.clear();
                        for (int i = 0; i < 5; ++i) {
                            platforms.emplace_back(static_cast<float>(rand() % 300), 100.0f + i * 100);
                        }

                        player.sprite.setPosition(200, player.startingY);  // Reset player position
                        gameState = PRESTART; // Change to PRESTART state, waiting for any key to begin
                    }
                }
            }
            else if (gameState == PRESTART) {
                // Wait for any key to start the game
                if (event.type == sf::Event::KeyPressed) {
                    gameState = GAME;  // Switch to the actual GAME state on key press
                }
            }
            else if (gameState == GAME) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    gameState = PAUSE;
                }
            }
            else if (gameState == PAUSE) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        pauseMenuOption = (pauseMenuOption - 1 + 2) % 2;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        pauseMenuOption = (pauseMenuOption + 1) % 2;
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        if (pauseMenuOption == 0) {
                            gameState = GAME;
                        }
                        else if (pauseMenuOption == 1) {
                            gameState = MENU;
                        }
                    }
                }
            }
            else if (gameState == GAME_OVER) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    gameState = MENU;
                }
            }
        }

        window.clear();

        if (gameState == MENU) {
            // Draw the menu
            easyOption.setFillColor(selectedOption == 0 ? sf::Color::Red : sf::Color::White);
            mediumOption.setFillColor(selectedOption == 1 ? sf::Color::Red : sf::Color::White);
            hardOption.setFillColor(selectedOption == 2 ? sf::Color::Red : sf::Color::White);
            window.draw(menuTitle);
            window.draw(easyOption);
            window.draw(mediumOption);
            window.draw(hardOption);
        }
        else if (gameState == PRESTART) {
            window.draw(startMessage);  // Show "Press any key to start" message
        }
        else if (gameState == GAME) {
            score = 0;
            
            // Handle player input for movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) player.move(-1, difficulty);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) player.move(1, difficulty);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) player.jump(difficulty);

            // Update player and platforms
            player.update();
            score = player.platformCount * 5;

            for (auto& platform : platforms) {
                checkCollision(player, platform);
            }

            if (platforms.back().sprite.getPosition().y > 100) {
                generatePlatforms(platforms, window.getSize().y, difficulty, player);
            }

            // Remove platforms that have moved off the screen
            platforms.erase(std::remove_if(platforms.begin(), platforms.end(),
                [&window](Platform& p) { return p.isOutOfScreen(window.getSize().y); }),
                platforms.end());

            // Move platforms down as the player moves up
            if (player.sprite.getPosition().y < 550) {
                for (auto& platform : platforms) {
                    if (difficulty == EASY)
                    {
                        platform.move(0, 2);
                    }
                    else if (difficulty == MEDIUM)
                    {
                        platform.move(0, 3);
                    }
                    else if (difficulty == HARD)
                    {
                        platform.move(0, 4);
                    }
                }
            }

            // Check if the player fell below the starting position
            if (player.fellFromPlatform()) {
                gameState = GAME_OVER;
            }
            background.move(1.0f, window.getSize().y);
            background.draw(window);

            window.draw(scoreText);
            window.draw(player.sprite);
            for (auto& platform : platforms) {
                window.draw(platform.sprite);
            }

        }
        else if (gameState == PAUSE) {
            resumeOption.setFillColor(pauseMenuOption == 0 ? sf::Color::Red : sf::Color::White);
            mainMenuOption.setFillColor(pauseMenuOption == 1 ? sf::Color::Red : sf::Color::White);

            window.draw(resumeOption);
            window.draw(mainMenuOption);
        }
        else if (gameState == GAME_OVER) {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}


