#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <random>
#include <ctime>
#include <sstream>
#include <iostream>

// Window settings
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Player settings
const float PLAYER_SIZE = 40.0f;
const float PLAYER_SPEED = 300.0f;
const float JUMP_FORCE = -600.0f;
const float GRAVITY = 1000.0f;

// Platform settings
const float PLATFORM_WIDTH = 100.0f;
const float PLATFORM_HEIGHT = 20.0f;
const float PLATFORM_SPEED = 200.0f;
const float SPAWN_INTERVAL = 1.5f;

class ChronoJumper {
private:
    sf::RenderWindow window;
    sf::RectangleShape player;
    std::vector<sf::RectangleShape> platforms;
    sf::Vector2f playerVelocity;
    bool isJumping;
    float spawnTimer;
    int score;
    sf::Font font;
    sf::Text scoreText;
    sf::Sound jumpSound;
    sf::SoundBuffer jumpBuffer;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist;

public:
    ChronoJumper() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chrono Jumper"), 
                    gen(rd()), dist(50.0f, WINDOW_HEIGHT - 150.0f) {
        window.setFramerateLimit(60);

        // Initialize player
        player.setSize(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE));
        player.setFillColor(sf::Color::Cyan);
        player.setPosition(WINDOW_WIDTH / 4.0f, WINDOW_HEIGHT - PLAYER_SIZE);
        playerVelocity = sf::Vector2f(0.0f, 0.0f);
        isJumping = false;

        // Initialize score
        score = 0;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font\n";
        }
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10.0f, 10.0f);

        // Initialize sound
        if (!jumpBuffer.loadFromFile("jump.wav")) {
            std::cerr << "Error loading jump sound\n";
        }
        jumpSound.setBuffer(jumpBuffer);

        spawnTimer = 0.0f;
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isJumping) {
            playerVelocity.y = JUMP_FORCE;
            isJumping = true;
            jumpSound.play();
        }
    }

    void update(float dt) {
        // Update player
        playerVelocity.y += GRAVITY * dt;
        player.move(playerVelocity * dt);

        // Check ground collision
        if (player.getPosition().y > WINDOW_HEIGHT - PLAYER_SIZE) {
            player.setPosition(player.getPosition().x, WINDOW_HEIGHT - PLAYER_SIZE);
            playerVelocity.y = 0.0f;
            isJumping = false;
        }

        // Spawn platforms
        spawnTimer += dt;
        if (spawnTimer >= SPAWN_INTERVAL) {
            sf::RectangleShape platform(sf::Vector2f(PLATFORM_WIDTH, PLATFORM_HEIGHT));
            platform.setFillColor(sf::Color::Green);
            platform.setPosition(WINDOW_WIDTH, dist(gen));
            platforms.push_back(platform);
            spawnTimer = 0.0f;
        }

        // Update platforms
        for (auto it = platforms.begin(); it != platforms.end();) {
            it->move(-PLATFORM_SPEED * dt, 0.0f);
            if (it->getPosition().x < -PLATFORM_WIDTH) {
                it = platforms.erase(it);
                score++;
            } else {
                ++it;
            }
        }

        // Check player-platform collision
        for (const auto& platform : platforms) {
            if (player.getGlobalBounds().intersects(platform.getGlobalBounds()) &&
                playerVelocity.y > 0 &&
                player.getPosition().y + PLAYER_SIZE - 10 < platform.getPosition().y) {
                playerVelocity.y = JUMP_FORCE;
                isJumping = true;
                jumpSound.play();
            }
        }

        // Update score text
        std::stringstream ss;
        ss << "Score: " << score;
        scoreText.setString(ss.str());
    }

    void render() {
        window.clear(sf::Color::Black);
        window.draw(player);
        for (const auto& platform : platforms) {
            window.draw(platform);
        }
        window.draw(scoreText);
        window.display();
    }

    void run() {
        sf::Clock clock;
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            float dt = clock.restart().asSeconds();
            handleInput();
            update(dt);
            render();
        }
    }
};

int main() {
    ChronoJumper game;
    game.run();
    return 0;
}
