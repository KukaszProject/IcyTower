#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>

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
    sf::Texture textureLeft;
    sf::Texture textureRight;
    sf::Texture textureJump;
    float velocityY = 0.0f;
    bool isJumping = false;
    float gravity = 0.5f;
    float jumpStrength = -12.0f;
    float speed = 5.0f;
    float startingY = 500.0f; // Starting Y position of the player

    int platformCount = 0;

    Player() {
        if (!textureLeft.loadFromFile("hero_l.png") ||
            !textureRight.loadFromFile("hero_r.png") ||
            !textureJump.loadFromFile("hero_jump.png")) {
            std::cerr << "Failed to load player textures!" << std::endl;
        }
        sprite.setTexture(textureRight); // Domyœlnie ustaw teksturê w prawo
        sprite.setPosition(200, startingY);  // Start position
    }

    void jump(Difficulty dif) {
        if (dif == EASY) {
            if (!isJumping) {
                velocityY = jumpStrength;
                isJumping = true;
                sprite.setTexture(textureJump); // Ustaw teksturê skoku
            }
        }
        else if (dif == MEDIUM) {
            if (!isJumping) {
                velocityY = jumpStrength * 0.8f; // U¿ycie 0.8f zamiast 0.8
                isJumping = true;
                sprite.setTexture(textureJump); // Ustaw teksturê skoku
            }
        }
        else if (dif == HARD) {
            if (!isJumping) {
                velocityY = jumpStrength * 0.5f; // U¿ycie 0.5f zamiast 0.5
                isJumping = true;
                sprite.setTexture(textureJump); // Ustaw teksturê skoku
            }
        }
    }

    void move(float dirX, Difficulty dif) {
        if (dirX < 0) {
            sprite.setTexture(textureLeft); // Ustaw teksturê w lewo
        }
        else if (dirX > 0) {
            sprite.setTexture(textureRight); // Ustaw teksturê w prawo
        }

        if (dif == EASY) {
            sprite.move(dirX * speed, 0);
        }
        else if (dif == MEDIUM) {
            sprite.move(dirX * speed * 0.8f, 0); // U¿ycie 0.8f zamiast 0.8
        }
        else if (dif == HARD) {
            sprite.move(dirX * speed * 0.5f, 0); // U¿ycie 0.5f zamiast 0.5
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
    if (!isGameActive) return false; // Nie sprawdzaj kolizji, jeœli gra nie jest aktywna

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

void generatePlatforms(std::vector<Platform>& platforms, float windowHeight, Difficulty difficulty, Player player) {
    float x = static_cast<float>(rand() % 300); // Random x position
    float y = -20; // Just above the screen

    if (difficulty == EASY) {
        platforms.emplace_back(x, y);
        player.platformCount++;
        player.speed = 5.0f;
        player.jumpStrength = 15.0f;
        player.gravity = 0.5f; // U¿ycie 0.5f zamiast 0.5
    }
    else if (difficulty == MEDIUM) {
        platforms.emplace_back(x, y);
        player.platformCount++;
        player.speed = 3.0f;
        player.jumpStrength = 12.0f;
        player.gravity = 0.6f; // U¿ycie 0.6f zamiast 0.6
    }
    else if (difficulty == HARD) {
        platforms.emplace_back(x, y);
        player.platformCount++;
        player.speed = 3.0f;
        player.jumpStrength = 11.0f;
        player.gravity = 0.6f; // U¿ycie 0.7f zamiast 0.7
    }
}

struct ScoreEntry {
    std::string name;
    int score;
    Difficulty difficulty; // Dodajemy informacjê o trybie

    bool operator<(const ScoreEntry& other) const {
        return score < other.score; // Sort in ascending order
    }

    // Dodaj operator porównania dla sortowania
    bool operator>(const ScoreEntry& other) const {
        return score > other.score; // Sort in descending order
    }
};

void saveScore(const std::string& name, int score, Difficulty difficulty) {
    std::ofstream file("score.txt", std::ios::app);
    if (file.is_open()) {
        file << name << " " << score << " " << static_cast<int>(difficulty) << std::endl; // Zapisz wynik z trybem
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
            entry.difficulty = static_cast<Difficulty>(difficulty); // Przypisz tryb
            scores.push_back(entry);
        }
    }

    // Sort scores in descending order
    std::sort(scores.begin(), scores.end(), std::greater<ScoreEntry>());
    return scores;
}

void displayTopScores(sf::RenderWindow& window, const std::vector<ScoreEntry>& scores) {
    sf::Font font;
    if (!font.loadFromFile("consolab.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setPosition(260, 440); // Position in the lower left corner

    std::ostringstream oss;
    oss << "Top Scores:\n";

    void displayTopScores(sf::RenderWindow & window, const std::vector<ScoreEntry>&scores); {
        sf::Font font;
        if (!font.loadFromFile("consolab.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
            return;
        }

        // Zmienne do przechowywania najlepszych wyników
        std::string easyName, mediumName, hardName;
        int easyScore = -1, mediumScore = -1, hardScore = -1;

        // Zbieranie najlepszych wyników
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

        // Tworzenie i rysowanie tekstów dla wyników
        if (easyScore != -1) {
            sf::Text easyText;
            sf::Text easyLabel;

            easyLabel.setFont(font);
            easyLabel.setString("Easy:");
            easyLabel.setFillColor(sf::Color::Red);
            easyLabel.setCharacterSize(20);
            easyLabel.setPosition(20, 450); // Ustaw pozycjê

            easyText.setFont(font);
            easyText.setString(easyName + " : " + std::to_string(easyScore));
            easyText.setFillColor(sf::Color::White);
            easyText.setCharacterSize(18);
            easyText.setPosition(20, 470); // Ustaw pozycjê
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
            mediumLabel.setPosition(20, 495); // Ustaw pozycjê

            mediumText.setFont(font);
            mediumText.setString(mediumName + ": " + std::to_string(mediumScore));
            mediumText.setFillColor(sf::Color::White);
            mediumText.setCharacterSize(18);
            mediumText.setPosition(20, 515); // Ustaw pozycjê
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
            hardLabel.setPosition(20, 540); // Ustaw pozycjê

            hardText.setFont(font);
            hardText.setString(hardName + ": " + std::to_string(hardScore));
            hardText.setFillColor(sf::Color::White);
            hardText.setCharacterSize(18);
            hardText.setPosition(20, 560); // Ustaw pozycjê
            window.draw(hardText);
            window.draw(hardLabel);
        }
    }

    

    scoreText.setString(oss.str());
    //window.draw(scoreText);
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    sf::RenderWindow window(sf::VideoMode(400, 600), "Icy Tower");
    window.setFramerateLimit(60);

    if (!Platform::loadTexture("platform.png")) {
        return -1;
    }

    Player player;
    std::vector<Platform> platforms;

    // Load background texture
    sf::Texture backgroundTexture;
    sf::Texture mainMenuTexture;
    sf::Texture scoreLabelTexture;
    if (!backgroundTexture.loadFromFile("bg.png")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return -1;
    }

    mainMenuTexture.loadFromFile("mainMenu.png");
    scoreLabelTexture.loadFromFile("scoreLabel.png");
    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Sprite mainMenuSprite(mainMenuTexture);
    sf::Sprite scoreLabel(scoreLabelTexture);
    float backgroundY = 0; // Y position of the background

    GameState gameState = MENU;  // Start with the menu
    Difficulty difficulty = EASY;

    // Flag to check if the game is moving
    bool isMoving = false;
    bool isGameActive = false; // Flaga do kontrolowania aktywnoœci gry

    // Font for menu and game text
    sf::Font font;
    if (!font.loadFromFile("consolab.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    // Menu items
    sf::Text menuTitle, easyOption, mediumOption, hardOption, gameOverText, startMessage, nickLabel;
    menuTitle.setFont(font);
    menuTitle.setString("Select Difficulty");
    menuTitle.setCharacterSize(30);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition(70, 100);

    easyOption.setFont(font);
    easyOption.setString("Easy");
    easyOption.setCharacterSize(25);
    easyOption.setPosition(50, 200);

    mediumOption.setFont(font);
    mediumOption.setString("Medium");
    mediumOption.setCharacterSize(25);
    mediumOption.setPosition(50, 245);

    hardOption.setFont(font);
    hardOption.setString("Hard");
    hardOption.setCharacterSize(25);
    hardOption.setPosition(50, 290);

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
    nickLabel.setCharacterSize(20);
    nickLabel.setFillColor(sf::Color::White);
    nickLabel.setPosition(50, 320);

    int selectedOption = 0;

    // Pause menu items
    sf::Text resumeOption, mainMenuOption, scoreText;
    resumeOption.setFont(font);
    resumeOption.setString("Resume");
    resumeOption.setCharacterSize(25);
    resumeOption.setPosition(150, 200);

    mainMenuOption.setFont(font);
    mainMenuOption.setString("Main Menu");
    mainMenuOption.setCharacterSize(25);
    mainMenuOption.setPosition(150, 250);

    scoreText.setFont(font);
    scoreText.setString("Score: 0");
    scoreText.setCharacterSize(25);
    scoreText.setPosition(10, 20);

    int pauseMenuOption = 0;  // 0 = Resume, 1 = Main Menu
    bool firstJump = false;   // Flag to check if the player has jumped at least once
    bool scoreStarted = false; // Flag to check if scoring has started

    // Zegar do naliczania czasu
    sf::Clock clock;
    int score = 0; // Zmienna do przechowywania punktów
    float elapsedTime = 0.0f; // Zmienna do przechowywania up³ywu czasu

    std::string playerName; // Zmienna do przechowywania nicku gracza

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
                    else if (event.key.code == sf::Keyboard::Down) { // Umo¿liwiono nawigacjê w dó³
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
                        gameState = GAME; // Change to PRESTART state, waiting for any key to begin
                        backgroundY = 0; // Reset background position
                        isMoving = false; // Ensure the game is not moving at the start\
                        score = 0; // Zerowanie wyniku na pocz¹tku rozgrywki
                        elapsedTime = 0.0f; // Resetowanie czasu
                        scoreStarted = false; // Reset scoring flag
                        isGameActive = true; // Aktywuj grê
                    }
                }
            }
            else if (gameState == PRESTART) {
                // Wait for any key to start the game
                if (event.type == sf::Event::KeyPressed) {
                    score = 0; // Zerowanie wyniku na pocz¹tku rozgrywki
                    elapsedTime = 0.0f; // Resetowanie czasu
                    scoreStarted = false; // Reset scoring flag
                    isGameActive = true; // Aktywuj grê
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
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        // Zapisz wynik do pliku
                        saveScore(playerName, score, difficulty); // Zapisz wynik z trybem
                        // Resetuj zmienne do stanu pocz¹tkowego
                        playerName.clear();
                        score = 0;
                        firstJump = false;
                        scoreStarted = false;
                        isGameActive = false; // Dezaktywuj grê
                        gameState = MENU; // Powrót do menu
                    }
                }
                else if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode < 128 && playerName.length() < 10) { // Ograniczenie do 10 znaków
                        if (event.text.unicode == '\b' && !playerName.empty()) {
                            playerName.pop_back(); // Usuwanie ostatniego znaku
                        }
                        else if (event.text.unicode != '\b') {
                            playerName += static_cast<char>(event.text.unicode); // Dodawanie znaku
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
            mediumOption.setFillColor(selectedOption == 1 ? sf::Color::Blue : sf::Color::White);
            hardOption.setFillColor(selectedOption == 2 ? sf::Color::Red : sf::Color::White);
            easyOption.setCharacterSize(selectedOption == 0 ? 32 : 25);
            mediumOption.setCharacterSize(selectedOption == 1 ? 32 : 25);
            hardOption.setCharacterSize(selectedOption == 2 ? 32 : 25);

            //window.draw(menuTitle);
            window.draw(easyOption);
            window.draw(mediumOption);
            window.draw(hardOption);

            // Wyœwietl najlepsze wyniki
            std::vector<ScoreEntry> scores = loadScores();
            displayTopScores(window, scores);
        }
        else if (gameState == PRESTART) {
            window.draw(mainMenuSprite);
            window.draw(startMessage);  // Show "Press any key to start" message
        }
        else if (gameState == GAME) {
            // Naliczanie punktów co sekundê, tylko jeœli scoring siê rozpocz¹³
            if (!scoreStarted) {
                window.draw(startMessage);  // Show "Press any key to start" message
            }

            if (scoreStarted) {
                elapsedTime += clock.restart().asSeconds(); // Zrestartuj zegar i dodaj czas do elapsedTime
                if (elapsedTime >= 1.0f) {
                    score += 1; // Dodaj punkty
                    elapsedTime = 0.0f; // Zresetuj czas
                }
            }

            // Wyœwietl wynik
            //window.draw(scoreLabel);
            scoreText.setString("Score: " + std::to_string(score));

            // Zmiana koloru wyniku w zale¿noœci od trybu
            if (score > 0) {
                if (difficulty == EASY && score > 15) { // Przyk³adowy próg
                    scoreText.setFillColor(sf::Color::Green);
                }
                else if (difficulty == MEDIUM && score > 30) { // Przyk³adowy próg
                    scoreText.setFillColor(sf::Color::Blue);
                }
                else if (difficulty == HARD && score > 50) { // Przyk³adowy próg
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

            for (auto& platform : platforms) {
                checkCollision(player, platform, isGameActive);
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
            window.draw(mainMenuSprite);
            resumeOption.setFillColor(pauseMenuOption == 0 ? sf::Color::Red : sf::Color::White);
            mainMenuOption.setFillColor(pauseMenuOption == 1 ? sf::Color::Red : sf::Color::White);

            window.draw(resumeOption);
            window.draw(mainMenuOption);
        }
        else if (gameState == GAME_OVER) {
            window.draw(mainMenuSprite);
            window.draw(gameOverText);
            window.draw(nickLabel);
            // Wyœwietl nick gracza
            sf::Text nameText;
            nameText.setFont(font);
            nameText.setString(playerName);
            nameText.setCharacterSize(20);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(50, 350);
            window.draw(nameText);
        }

        window.display();
    }

    return 0;
}