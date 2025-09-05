
#include "game.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace {
    constexpr unsigned WIDTH  = 600;
    constexpr unsigned HEIGHT = 600;
}

Game::Game()
: mWindow(sf::VideoMode(WIDTH, HEIGHT), "Tower Defense")
{
    mWindow.setFramerateLimit(60);

    // Base (boule bleue) à protéger
    mBase.setRadius(18.f);
    mBase.setFillColor(sf::Color(100, 180, 255));
    mBase.setOrigin(mBase.getRadius(), mBase.getRadius());
    mBase.setPosition(560.f, 560.f);

    // Chemin: une suite de waypoints (tu peux modifier)
    mPath = {
        {  20.f, 300.f},
        { 280.f, 300.f},
        { 280.f, 100.f},
        { 520.f, 100.f},
        { 520.f, 500.f},
        { 560.f, 560.f} // arrive sur la base
    };

    // Police pour HUD (optionnel) — on tente une police système (pas requis)
    // Si tu as une police, charge-la avec loadFromFile(). Sinon, le texte ne s'affichera pas.
    // mFont.loadFromFile("assets/Roboto-Regular.ttf");
    mText.setFont(mFont); // si non chargée, SFML utilisera un fallback ou n’affichera rien
    mText.setCharacterSize(16);
    mText.setFillColor(sf::Color::White);
    mText.setPosition(10.f, 10.f);
    mText.setString("Click gauche: poser une tour (max 3)\nPV Base: 5");
}

void Game::run() {
    while (mWindow.isOpen()) {
        handleEvents();

        float dt = mFrameClock.restart().asSeconds();
        if (!mGameOver) {
            update(dt);
        }

        render();
    }
}

void Game::handleEvents() {
    sf::Event e;
    while (mWindow.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            mWindow.close();
        }
        if (!mGameOver && e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            if (mTowers.size() < 3) {
                // Ajouter une tour à l'endroit du clic
                Tower t;
                t.base.setRadius(12.f);
                t.base.setOrigin(12.f, 12.f);
                t.base.setFillColor(sf::Color(120, 120, 120));
                t.base.setOutlineColor(sf::Color::White);
                t.base.setOutlineThickness(2.f);
                t.base.setPosition(static_cast<float>(e.mouseButton.x),
                                   static_cast<float>(e.mouseButton.y));
                mTowers.push_back(t);
            }
        }
    }
}

void Game::update(float dt) {
    // 1) Spawner des ennemis régulièrement
    spawnEnemyPeriodically(dt);

    // 2) Déplacer les ennemis (2 px / frame équivalent ici, clampé par dt si > 60 FPS)
    moveEnemies();

    // 3) Les tourelles tirent si un ennemi est dans leur portée
    towersAcquireAndShoot(dt);

    // 4) Déplacer les projectiles et gérer collisions
    updateBullets(dt);

    // 5) Nettoyer les entités mortes
    cleanupDead();

    // 6) Mettre à jour HUD
    mText.setString(
        "Click gauche: poser une tour (max 3)\n"
        "Tours: " + std::to_string(mTowers.size()) + "/3\n"
        "Ennemis: " + std::to_string(mEnemies.size()) + "\n"
        "PV Base: " + std::to_string(mBaseHP) + (mGameOver ? "\nGAME OVER" : "")
    );
}

void Game::render() {
    mWindow.clear(sf::Color(30, 30, 35));

    // Dessiner le chemin (facultatif)
    for (std::size_t i = 1; i < mPath.size(); ++i) {
        sf::Vertex line[] = {
            sf::Vertex(mPath[i-1], sf::Color(80, 80, 80)),
            sf::Vertex(mPath[i],   sf::Color(80, 80, 80))
        };
        mWindow.draw(line, 2, sf::Lines);
    }

    // Base
    mWindow.draw(mBase);

    // Tours
    for (auto& t : mTowers) mWindow.draw(t.base);

    // Ennemis
    for (auto& en : mEnemies) mWindow.draw(en.shape);

    // Balles
    for (auto& b : mBullets) mWindow.draw(b.shape);

    // HUD
    if (mFont.getInfo().family.size() > 0) {
        mWindow.draw(mText);
    }

    mWindow.display();
}

void Game::spawnEnemyPeriodically(float dt) {
    mEnemySpawnTimer += dt;
    if (mEnemySpawnTimer >= mEnemySpawnPeriod) {
        mEnemySpawnTimer = 0.f;
        Enemy en;
        en.shape.setRadius(10.f);
        en.shape.setOrigin(10.f, 10.f);
        en.shape.setFillColor(sf::Color(200, 70, 70));
        en.shape.setPosition(mPath.front());
        en.waypointIndex = 1; // se dirige vers le 2e point
        en.speed = 2.f;       // 2 px / itération (frame). Ici on applique ça par step.
        en.hp = 100.f;
        mEnemies.push_back(en);
    }
}

void Game::moveEnemies() {
    for (auto& en : mEnemies) {
        if (en.waypointIndex >= mPath.size()) continue;

        sf::Vector2f pos = en.shape.getPosition();
        sf::Vector2f target = mPath[en.waypointIndex];
        sf::Vector2f dir = target - pos;
        float d = length(dir);

        if (d < 1.f) {
            en.waypointIndex++;
            // Si l’ennemi atteint la base (dernier waypoint == base)
            if (en.waypointIndex >= mPath.size()) {
                // dégâts à la base
                mBaseHP -= 1;
                if (mBaseHP <= 0) {
                    mBaseHP = 0;
                    mGameOver = true;
                }
                // on "mange" l’orbe → on retirera l’ennemi au cleanup
            }
            continue;
        }
        sf::Vector2f step = normalize(dir) * en.speed; // 2 px par update
        // S’assurer de ne pas dépasser la cible
        if (length(step) > d) step = normalize(dir) * d;
        en.shape.move(step);
    }
}

void Game::towersAcquireAndShoot(float dt) {
    for (auto& t : mTowers) {
        t.timeSinceLastShot += dt;

        // Trouver l'ennemi le plus proche dans la portée
        int bestIndex = -1;
        float bestDist = t.range;

        for (int i = 0; i < static_cast<int>(mEnemies.size()); ++i) {
            const auto& en = mEnemies[i];
            float d = distance(t.base.getPosition(), en.shape.getPosition());
            if (d <= bestDist && en.waypointIndex < mPath.size()) {
                bestDist = d;
                bestIndex = i;
            }
        }

        if (bestIndex != -1 && t.timeSinceLastShot >= t.fireCooldown) {
            // Tirer un projectile
            t.timeSinceLastShot = 0.f;

            const auto& targetEnemy = mEnemies[bestIndex];
            sf::Vector2f from = t.base.getPosition();
            sf::Vector2f to   = targetEnemy.shape.getPosition();
            sf::Vector2f dir  = normalize(to - from);

            Bullet b;
            b.shape.setRadius(4.f);
            b.shape.setOrigin(4.f, 4.f);
            b.shape.setFillColor(sf::Color(240, 220, 50));
            b.shape.setPosition(from);

            float bulletSpeed = 7.f; // px / itération
            b.velocity = dir * bulletSpeed;
            b.damage = 35.f;

            mBullets.push_back(b);
        }
    }
}

void Game::updateBullets(float dt) {
    // Avancer les balles
    for (auto& b : mBullets) {
        b.shape.move(b.velocity); // vitesse fixe par frame (pas dépendant du dt)
    }

    // Collisions balle-ennemi
    for (auto& b : mBullets) {
        for (auto& en : mEnemies) {
            if (en.waypointIndex >= mPath.size()) continue; // déjà à la base
            float d = distance(b.shape.getPosition(), en.shape.getPosition());
            if (d < (b.shape.getRadius() + en.shape.getRadius())) {
                en.hp -= b.damage;
                // on "téléporte" la balle hors champ ; elle sera supprimée au cleanup
                b.shape.setPosition(-1000.f, -1000.f);
                break;
            }
        }
    }
}

void Game::cleanupDead() {
    // Ennemis morts ou arrivés à la base
    mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(),
        [this](const Enemy& en) {
            bool reachedBase = (en.waypointIndex >= mPath.size());
            bool dead = (en.hp <= 0.f);
            return reachedBase || dead;
        }), mEnemies.end());

    // Balle hors écran
    mBullets.erase(std::remove_if(mBullets.begin(), mBullets.end(),
        [](const Bullet& b) {
            auto p = b.shape.getPosition();
            return p.x < -50.f || p.y < -50.f || p.x > WIDTH + 50.f || p.y > HEIGHT + 50.f;
        }), mBullets.end());
}

/* ---------- Utilitaires ---------- */
float Game::length(const sf::Vector2f& v) {
    return std::sqrt(v.x*v.x + v.y*v.y);
}

sf::Vector2f Game::normalize(const sf::Vector2f& v) {
    float len = length(v);
    if (len == 0) return {0.f, 0.f};
    return { v.x / len, v.y / len };
}

float Game::distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return length(b - a);
}


/*#include "game.hpp"
#include <iostream>

Game::Game() // Constructeur de la classe game
: mWindow(sf::VideoMode(800, 600), "Tower Defense") // Initialise la fenêtre
{
    // Chargement de l’image (par ex. "player.png")
    if (!mPlayerTexture.loadFromFile("../TheGame/assets/h1.png")) {
        // Erreur si le fichier est introuvable
        std::cerr << "Erreur: impossible de charger h1.png\n";
    }

    // Associer la texture au sprite
    mPlayer.setTexture(mPlayerTexture);
    mPlayer.setScale(0.01f, 0.01f); // reduction du sprite car trop grand pour la fenêtre

    // Position initiale du joueur
    mPlayer.setPosition(200.f, 200.f);
}


void Game::run() {
    while (mWindow.isOpen()) {
        keybindEvents();
        update();
        render();
    }
}

void Game::keybindEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();
    }
}

void Game::update() {
    // Vitesse de déplacement (pixels par frame)
    float speed = 0.2f;

    // Vérifie si une touche est pressée et déplace le joueur
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        mPlayer.move(-speed, 0.f); // gauche
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        mPlayer.move(speed, 0.f); // droite
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        mPlayer.move(0.f, -speed); // haut
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        mPlayer.move(0.f, speed); // bas
    }
}


void Game::render() {
    mWindow.clear();
    mWindow.draw(mPlayer);
    mWindow.display();
}
*/