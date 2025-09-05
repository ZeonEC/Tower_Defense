#include "game.hpp"

Game::Game() // Constructeur de la classe game
: mWindow(sf::VideoMode(800, 600), "Tower Defense"), // Initialise la fenêtre

mPlayer(20.f) // rayon du cercle
{
    mPlayer.setFillColor(sf::Color::Green);
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
