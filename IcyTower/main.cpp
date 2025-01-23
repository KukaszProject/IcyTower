#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>

enum GameState { MENU, PRESTART, SKIN_SELECTION, GAME, PAUSE, GAME_OVER };

// Difficulty settings
enum Difficulty { EASY, MEDIUM, HARD };

class Platform {
public:
    sf::Sprite sprite;
    static sf::Texture texture1;
    static sf::Texture texture2;
    static sf::Texture texture3;

    Platform(float x, float y) {
        // Randomly select a texture for the platform
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

sf::Texture Platform::texture1;
sf::Texture Platform::texture2;
sf::Texture Platform::texture3;

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

    Player() {
        if (!texture.loadFromFile("hero.png")) {
            std::cerr << "Failed to load player textures!" << std::endl;
        }
        sprite.setTexture(texture); // Default texture facing right
        sprite.setPosition(200, startingY);  // Start position
    }

    void jump(Difficulty dif) {
        if (dif == EASY) {
            if (!isJumping) {
                velocityY = jumpStrength;
                isJumping = true;
                //sprite.setTexture(textureJump); // Set jump texture
            }
        }
        else if (dif == MEDIUM) {
            if (!isJumping) {
                velocityY = jumpStrength * 0.8f;
                isJumping = true;
                //sprite.setTexture(textureJump); // Set jump texture
            }
        }
        else if (dif == HARD) {
            if (!isJumping) {
                velocityY = jumpStrength * 0.62f;
                isJumping = true;
                //sprite.setTexture(textureJump); // Set jump texture
            }
        }
    }

    void move(float dirX, Difficulty dif) {
        //if (dirX < 0) {
        //    //sprite.setTexture(textureLeft); // Set texture facing left
        //}
        //else if (dirX > 0) {
        //    //sprite.setTexture(textureRight); // Set texture facing right
        //}

        if (dif == EASY) {
            sprite.move(dirX * speed, 0);
        }
        else if (dif == MEDIUM) {
            sprite.move(dirX * speed * 0.8f, 0);
        }
        else if (dif == HARD) {
            sprite.move(dirX * speed * 0.5f, 0);
        }
    }

    void update(bool isGameActive) {
        if (isGameActive) {
            velocityY += gravity;
            sprite.move(0, velocityY);
        }

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

bool checkCollision(Player& player, Platform& platform, bool isGameActive) {
    if (!isGameActive) return false; // Don't check collision if the game is not active

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

void generateInitialPlatforms(std::vector<Platform>& platforms, float windowHeight) {
    float x;
    float y = 550; // Starting Y position for platforms
    for (int i = 0; i < 6; ++i) {
        // Ensure platforms are spaced out
        if (i == 0) {
            x = 170;
        }
        else {
            x = static_cast<float>(rand() % 300); // Random x position
        }

        platforms.emplace_back(x, y);
        y -= 100; // Increase Y position for the next platform
    }
}

void generatePlatforms(std::vector<Platform>& platforms, float windowHeight, Difficulty difficulty) {
    float x = static_cast<float>(rand() % 300); // Random x position
    float y = -20; // Just above the screen
    platforms.emplace_back(x, y);
}

struct ScoreEntry {
    std::string name;
    int score;
    Difficulty difficulty; // Add difficulty information

    bool operator<(const ScoreEntry& other) const {
        return score < other.score; // Sort in ascending order
    }

    // Add comparison operator for sorting
    bool operator>(const ScoreEntry& other) const {
        return score > other.score; // Sort in descending order
    }
};

void saveScore(const std::string& name, int score, Difficulty difficulty) {
    std::ofstream file("score.txt", std::ios::app);
    if (file.is_open()) {
        file << name << " " << score << " " << static_cast<int>(difficulty) << std::endl; // Save score with difficulty
        file.close();
    }
}

std::vector<ScoreEntry> loadScores() {
    std::vector<ScoreEntry> scores;
    std::ifstream file("score.txt");
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        ScoreEntry entry;
        int difficulty;
        if (iss >> entry.name >> entry.score >> difficulty) {
            entry.difficulty = static_cast<Difficulty>(difficulty); // Assign difficulty
            scores.push_back(entry);
        }
    }

    // Sort scores in descending order
    std::sort(scores.begin(), scores.end(), std::greater<ScoreEntry>());
    return scores;
}

void displayTopScores(sf::RenderWindow& window, const std::vector<ScoreEntry>& scores) {
    sf::Font font;
    if (!font.loadFromFile("ThaleahFat.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setPosition(260, 440); // Position in the lower left corner

    std::ostringstream oss;
    oss << "Top Scores:\n";

    // Variables to store top scores
    std::string easyName, mediumName, hardName;
    int easyScore = -1, mediumScore = -1, hardScore = -1;

    // Collecting top scores
    for (const auto& entry : scores) {
        if (entry.difficulty == EASY && (easyScore == -1 || entry.score > easyScore)) {
            easyScore = entry.score;
            easyName = entry.name;
        }
        else if (entry.difficulty == MEDIUM && (mediumScore == -1 || entry.score > mediumScore)) {
            mediumScore = entry.score;
            mediumName = entry.name;
        }
        else if (entry.difficulty == HARD && (hardScore == -1 || entry.score > hardScore)) {
            hardScore = entry.score;
            hardName = entry.name;
        }
    }

    // Create and draw text for scores
    if (easyScore != -1) {
        sf::Text easyText;
        sf::Text easyLabel;

        easyLabel.setFont(font);
        easyLabel.setString("Easy:");
        easyLabel.setFillColor(sf::Color::Red);
        easyLabel.setCharacterSize(20);
        easyLabel.setPosition(20, 450); // Set position

        easyText.setFont(font);
        easyText.setString(easyName + " : " + std::to_string(easyScore));
        easyText.setFillColor(sf::Color::White);
        easyText.setCharacterSize(18);
        easyText.setPosition(20, 470); // Set position
        window.draw(easyText);
        window.draw(easyLabel);
    }

    if (mediumScore != -1) {
        sf::Text mediumText;
        sf::Text mediumLabel;

        mediumLabel.setFont(font);
        mediumLabel.setString("Medium:");
        mediumLabel.setFillColor(sf::Color::Red);
        mediumLabel.setCharacterSize(20);
        mediumLabel.setPosition(20, 495); // Set position

        mediumText.setFont(font);
        mediumText.setString(mediumName + ": " + std::to_string(mediumScore));
        mediumText.setFillColor(sf::Color::White);
        mediumText.setCharacterSize(18);
        mediumText.setPosition(20, 515); // Set position
        window.draw(mediumText);
        window.draw(mediumLabel);
    }

    if (hardScore != -1) {
        sf::Text hardText;
        sf::Text hardLabel;

        hardLabel.setFont(font);
        hardLabel.setString("Hard:");
        hardLabel.setFillColor(sf::Color::Red);
        hardLabel.setCharacterSize(20);
        hardLabel.setPosition(20, 540); // Set position

        hardText.setFont(font);
        hardText.setString(hardName + ": " + std::to_string(hardScore));
        hardText.setFillColor(sf::Color::White);
        hardText.setCharacterSize(18);
        hardText.setPosition(20, 560); // Set position
        window.draw(hardText);
        window.draw(hardLabel);
    }

    scoreText.setString(oss.str());
    //window.draw(scoreText);
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    sf::RenderWindow window(sf::VideoMode(400, 600), "Icy Tower");
    window.setFramerateLimit(60);

    // Load platform textures
    if (!Platform::loadTextures("platform.png", "perla.png", "romper.png")) {
        return -1;
    }

    // Load different skins (textures)
    sf::Texture skin1, skin2, skin3;
    if (!skin1.loadFromFile("hero.png") || !skin2.loadFromFile("hero1.png") || !skin3.loadFromFile("hero2.png")) {
        std::cerr << "Failed to load skin textures!" << std::endl;
        return -1;
    }

    // Store skins in a vector for easy cycling
    std::vector<sf::Texture> skins = { skin1, skin2, skin3 };

    Player player;
    std::vector<Platform> platforms;

    // Game state and skin selection variables
    GameState gameState = MENU;
    int selectedSkinIndex = 1;

    // Text for skin selection menu
    sf::Font font;
    if (!font.loadFromFile("ThaleahFat.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }


    sf::Text instructionText;
    instructionText.setFont(font);
    instructionText.setString("Use Left/Right arrow keys to choose.\n\t\t\tPress Enter to start.");
    instructionText.setCharacterSize(20);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setPosition(40, 400);

    // Player sprite for preview in the skin selection menu
    sf::Sprite previewSprite;
    previewSprite.setTexture(skins[selectedSkinIndex]);
    previewSprite.setPosition(170, 200);

    // Load background texture
    sf::Texture backgroundTexture, mainMenuTexture, scoreLabelTexture, gameOverTexture, pauseTexture, skinChooseTexture;
    if (!backgroundTexture.loadFromFile("bgg.jpg")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return -1;
    }

    mainMenuTexture.loadFromFile("mainMenu.png");
    gameOverTexture.loadFromFile("gover.png");
    scoreLabelTexture.loadFromFile("scoreLabel.png");
    pauseTexture.loadFromFile("restBg.png");
    skinChooseTexture.loadFromFile("skinChoose.png");
    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Sprite mainMenuSprite(mainMenuTexture);
    sf::Sprite scoreLabel(scoreLabelTexture);
    sf::Sprite gameOverSprite(gameOverTexture);
    sf::Sprite pauseSprite(pauseTexture);
    sf::Sprite skinChooseSprite(skinChooseTexture);
    float backgroundY = 0; // Y position of the background

    Difficulty difficulty = EASY;

    // Flag to check if the game is moving
    bool isMoving = false;
    bool isGameActive = false; // Flag to control game activity

    // Menu items
    sf::Text menuTitle, easyOption, mediumOption, hardOption, gameOverText, startMessage, nickLabel, skinSelection;
    menuTitle.setFont(font);
    menuTitle.setString("Select Difficulty");
    menuTitle.setCharacterSize(30);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition(70, 100);

    easyOption.setFont(font);
    easyOption.setString("Easy");
    easyOption.setCharacterSize(32);
    easyOption.setPosition(100, 200);

    mediumOption.setFont(font);
    mediumOption.setString("Medium");
    mediumOption.setCharacterSize(32);
    mediumOption.setPosition(100, 245);

    hardOption.setFont(font);
    hardOption.setString("Hard");
    hardOption.setCharacterSize(32);
    hardOption.setPosition(100, 290);

    skinSelection.setFont(font);
    skinSelection.setString("Skin");
    skinSelection.setCharacterSize(25);
    skinSelection.setPosition(100, 335);

    startMessage.setFont(font);
    startMessage.setString("Press SPACE to start.");
    startMessage.setCharacterSize(20);
    startMessage.setFillColor(sf::Color::White);
    startMessage.setPosition(85, 560);

    gameOverText.setFont(font);
    gameOverText.setString("Game Over!");
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(50, 270);

    nickLabel.setFont(font);
    nickLabel.setString("Enter your name:");
    nickLabel.setCharacterSize(25);
    nickLabel.setFillColor(sf::Color::Green);
    nickLabel.setPosition(100, 220);

    int selectedOption = 0;

    // Pause menu items
    sf::Text resumeOption, mainMenuOption, scoreText;
    resumeOption.setFont(font);
    resumeOption.setString("Resume");
    resumeOption.setCharacterSize(30);
    resumeOption.setPosition(100, 200);

    mainMenuOption.setFont(font);
    mainMenuOption.setString("Main Menu");
    mainMenuOption.setCharacterSize(30);
    mainMenuOption.setPosition(100, 240);

    scoreText.setFont(font);
    scoreText.setString("Score: 0");
    scoreText.setCharacterSize(25);
    scoreText.setPosition(10, 5);

    int pauseMenuOption = 0;  // 0 = Resume, 1 = Main Menu
    bool firstJump = false;   // Flag to check if the player has jumped at least once
    bool scoreStarted = false; // Flag to check if scoring has started

    // Clock for timing
    sf::Clock clock;
    int score = 0; // Variable to store points
    float elapsedTime = 0.0f; // Variable to store elapsed time

    std::string playerName; // Variable to store player nickname

    // Generate initial platforms
    generateInitialPlatforms(platforms, window.getSize().y);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameState == MENU) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedOption = (selectedOption - 1 + 4) % 4;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        selectedOption = (selectedOption + 1) % 4;
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        if (selectedOption == 0) difficulty = EASY;
                        else if (selectedOption == 1) difficulty = MEDIUM;
                        else if (selectedOption == 2) difficulty = HARD;

                        player.sprite.setPosition(200, player.startingY);  // Reset player position
                        gameState = GAME; // Change to PRESTART state, waiting for any key to begin
                        backgroundY = 0; // Reset background position
                        isMoving = false; // Ensure the game is not moving at the start
                        score = 0; // Reset score at the beginning of the game
                        elapsedTime = 0.0f; // Reset elapsed time
                        scoreStarted = false; // Reset scoring flag
                        isGameActive = true; // Activate game
                    }
                    if (event.key.code == sf::Keyboard::Enter && selectedOption == 3) gameState = SKIN_SELECTION;
                }
            }
            else if (gameState == PRESTART) {
                // Wait for any key to start the game
                if (event.type == sf::Event::KeyPressed) {
                    score = 0; // Reset score at the beginning of the game
                    elapsedTime = 0.0f; // Reset elapsed time
                    scoreStarted = false; // Reset scoring flag
                    isGameActive = true; // Activate game
                    gameState = GAME;  // Switch to the actual GAME state on key press
                }
            }
            else if (gameState == GAME) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    gameState = PAUSE;
                }
            }
            else if (gameState == SKIN_SELECTION) {
                if (event.type == sf::Event::KeyPressed) {
                    // Navigate left or right to choose a skin
                    if (event.key.code == sf::Keyboard::Right) {
                        selectedSkinIndex = (selectedSkinIndex + 1) % skins.size();
                        previewSprite.setTexture(skins[selectedSkinIndex]);
                    }
                    else if (event.key.code == sf::Keyboard::Left) {
                        selectedSkinIndex = (selectedSkinIndex - 1 + skins.size()) % skins.size();
                        previewSprite.setTexture(skins[selectedSkinIndex]);
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        // Apply selected skin and start the game
                        player.sprite.setTexture(skins[selectedSkinIndex]);
                        gameState = GAME;
                    }
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
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        // Save score to file
                        saveScore(playerName, score, difficulty); // Save score with difficulty
                        // Reset variables to initial state
                        playerName.clear();
                        score = 0;
                        firstJump = false;
                        scoreStarted = false;
                        isGameActive = false; // Deactivate game
                        gameState = MENU; // Return to menu
                    }
                }
                else if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode < 128 && playerName.length() < 10) { // Limit to 10 characters
                        if (event.text.unicode == '\b' && !playerName.empty()) {
                            playerName.pop_back(); // Remove last character
                        }
                        else if (event.text.unicode != '\b') {
                            playerName += static_cast<char>(event.text.unicode); // Add character
                        }
                    }
                }
            }
        }

        window.clear();

        // Move the background down only if the game is moving
        if (isMoving) {
            backgroundY += 2; // Adjust this value to change the speed of the background

            // Reset background position if it goes off screen
            if (backgroundY >= backgroundTexture.getSize().y) {
                backgroundY = 0;
            }
        }

        // Draw the background
        window.draw(backgroundSprite);
        backgroundSprite.setPosition(0, backgroundY);
        window.draw(backgroundSprite);
        backgroundSprite.setPosition(0, backgroundY - backgroundTexture.getSize().y);
        window.draw(backgroundSprite);

        if (gameState == MENU) {
            // Draw the menu
            window.draw(mainMenuSprite);
            easyOption.setFillColor(selectedOption == 0 ? sf::Color::Green : sf::Color::White);
            mediumOption.setFillColor(selectedOption == 1 ? sf::Color::Cyan : sf::Color::White);
            hardOption.setFillColor(selectedOption == 2 ? sf::Color::Red : sf::Color::White);
            skinSelection.setFillColor(selectedOption == 3 ? sf::Color::Yellow : sf::Color::White);
            easyOption.setString(selectedOption == 0 ? "- EASY" : "EASY");
            mediumOption.setString(selectedOption == 1 ? "- MEDIUM" : "MEDIUM");
            hardOption.setString(selectedOption == 2 ? "- HARD" : "HARD");
            skinSelection.setString(selectedOption == 3 ? "- CHANGE SKIN" : "CHANGE SKIN");
            easyOption.setCharacterSize(selectedOption == 0 ? 40 : 32);
            mediumOption.setCharacterSize(selectedOption == 1 ? 40 : 32);
            hardOption.setCharacterSize(selectedOption == 2 ? 40 : 32);
            skinSelection.setCharacterSize(selectedOption == 3 ? 30 : 25);

            window.draw(easyOption);
            window.draw(mediumOption);
            window.draw(hardOption);
            window.draw(skinSelection);

            // Display top scores
            std::vector<ScoreEntry> scores = loadScores();
            displayTopScores(window, scores);
        }
        else if (gameState == PRESTART) {
            window.draw(mainMenuSprite);
            window.draw(startMessage);  // Show "Press any key to start" message
        }
        else if (gameState == GAME) {
            // Scoring every second, only if scoring has started
            if (!scoreStarted) {
                window.draw(startMessage);  // Show "Press any key to start" message
            }

            if (scoreStarted) {
                elapsedTime += clock.restart().asSeconds(); // Restart clock and add time to elapsedTime
                if (elapsedTime >= 1.0f) {
                    score += 1; // Add points
                    elapsedTime = 0.0f; // Reset time
                }
            }

            // Display score
            scoreText.setString("Score: " + std::to_string(score));

            // Change score color based on difficulty
            if (score > 0) {
                if (difficulty == EASY && score > 15) {
                    scoreText.setFillColor(sf::Color::Green);
                }
                else if (difficulty == MEDIUM && score > 30) {
                    scoreText.setFillColor(sf::Color::Blue);
                }
                else if (difficulty == HARD && score > 50) {
                    scoreText.setFillColor(sf::Color::Red);
                }
                else {
                    scoreText.setFillColor(sf::Color::White);
                }
            }

            // Handle player input for movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) player.move(-1, difficulty);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) player.move(1, difficulty);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                player.jump(difficulty);
                firstJump = true; // Set the flag to true when the player jumps
                isMoving = true;  // Start moving the background and platforms
                scoreStarted = true; // Start scoring after the first jump
            }

            // Update player and platforms
            player.update(isGameActive);

            // Generate new platforms if needed
            if (platforms.back().sprite.getPosition().y > 100) {
                generatePlatforms(platforms, window.getSize().y, difficulty);
            }

            for (auto& platform : platforms) {
                checkCollision(player, platform, isGameActive);
            }

            // Remove platforms that have moved off the screen
            platforms.erase(std::remove_if(platforms.begin(), platforms.end(),
                [&window](Platform& p) { return p.isOutOfScreen(window.getSize().y); }),
                platforms.end());

            // Move platforms down as the player moves up
            if (player.sprite.getPosition().y < 550) {
                for (auto& platform : platforms) {
                    if (isMoving) { // Only move platforms if the game is moving
                        if (difficulty == EASY) {
                            platform.move(0, 2);
                        }
                        else if (difficulty == MEDIUM) {
                            platform.move(0, 3);
                        }
                        else if (difficulty == HARD) {
                            platform.move(0, 3.5);
                        }
                    }
                }
            }

            // Check if the player fell below the starting position and if the player has jumped at least once
            if (firstJump && player.fellFromPlatform()) {
                gameState = GAME_OVER;
            }

            window.draw(scoreText);
            window.draw(player.sprite);
            for (auto& platform : platforms) {
                window.draw(platform.sprite);
            }
        }
        else if (gameState == PAUSE) {
            window.draw(pauseSprite);
            resumeOption.setFillColor(pauseMenuOption == 0 ? sf::Color::Red : sf::Color::White);
            mainMenuOption.setFillColor(pauseMenuOption == 1 ? sf::Color::Red : sf::Color::White);

            window.draw(resumeOption);
            window.draw(mainMenuOption);
        }
        else if (gameState == SKIN_SELECTION) {
            // Display skin selection menu
            window.draw(skinChooseSprite);
            //window.draw(skinSelectionText);
            window.draw(previewSprite);
            window.draw(instructionText);
        }
        else if (gameState == GAME_OVER) {
            window.draw(gameOverSprite);
            //window.draw(gameOverText);
            window.draw(nickLabel);
            // Display player nickname
            sf::Text nameText;
            nameText.setFont(font);
            nameText.setString(playerName);
            nameText.setCharacterSize(20);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(140, 250);
            window.draw(nameText);
        }

        window.display();
    }

    return 0;
}