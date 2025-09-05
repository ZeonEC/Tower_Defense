#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Enemy {
    sf::CircleShape shape;
    std::size_t waypointIndex = 0; // prochain point à atteindre
    float speed = 2.f;             // 2 px / itération
    float hp = 100.f;              // points de vie
};

struct Tower {
    sf::CircleShape base; // visuel de la tourelle
    float range = 140.f;  // portée
    float fireCooldown = 0.4f; // en secondes
    float timeSinceLastShot = 0.f;
};

struct Bullet {
    sf::CircleShape shape;
    sf::Vector2f velocity; // direction * vitesse
    float damage = 35.f;
};

class Game {
public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void render();

    // logique
    void spawnEnemyPeriodically(float dt);
    void moveEnemies();
    void towersAcquireAndShoot(float dt);
    void updateBullets(float dt);
    void cleanupDead();

    // utilitaires
    static float length(const sf::Vector2f& v);
    static sf::Vector2f normalize(const sf::Vector2f& v);
    static float distance(const sf::Vector2f& a, const sf::Vector2f& b);

private:
    sf::RenderWindow mWindow;

    // base à défendre (boule bleue)
    sf::CircleShape mBase;
    int mBaseHP = 5;

    // chemins / waypoints
    std::vector<sf::Vector2f> mPath;

    // entités
    std::vector<Enemy>  mEnemies;
    std::vector<Tower>  mTowers;
    std::vector<Bullet> mBullets;

    // timers
    sf::Clock mFrameClock; // pour dt
    float mEnemySpawnTimer = 0.f;  // secondes depuis le dernier spawn
    float mEnemySpawnPeriod = 1.5f; // spawn toutes les 1.5 s

    // HUD minimal
    sf::Font mFont;
    sf::Text mText;
    bool mGameOver = false;
};


/*
#pragma once
#include <SFML/Graphics.hpp>

class Game {
public:
    Game();
    void run();  // boucle principale du jeu

private:
    void keybindEvents();   // gestion des entrées clavier/souris
    void update();          // mise à jour logique
    void render();          // affichage

    sf::RenderWindow mWindow;

    //Défini le joueur avec un sprite plutôt qu'une forme
    sf::Texture mPlayerTexture;
    sf::Sprite mPlayer;

};*/
