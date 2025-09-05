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
    sf::CircleShape mPlayer; // élément test (un cercle)
};
