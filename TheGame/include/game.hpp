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

};
