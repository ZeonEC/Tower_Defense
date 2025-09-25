#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <SFML/System/Vector2.hpp> // pour sf::Vector2f

#include "render.hpp"
#include "enemy.hpp"
#include "tower.hpp"

//cette fonction permet de gérer l'initialisatoin et le comportement des différents acteurs du jeux (Tourelles, ennemies, projectiles, etc...)

class Game {
public:
    Game() = default;
    ~Game() = default;

    // Génère un ennemi aléatoire et l'ajoute au vecteur
    //void generateEnemy(std::vector<Enemy*>& enemies, std::vector<Render::Cell> cells);
    void generateEnemy(std::vector<Enemy*>& enemies,std::vector<Render::Cell> cells);

    // Détruit tous les ennemis et vide le vecteur
    void destroyEnemy(std::vector<Enemy*>& enemies);


    void generateTourelle(std::vector<Tourelle*>& tourelles,std::vector<Render::Cell>& cells,float x,float y);
    // Détruit tous les tourelles et vide le vecteur
    void destroyTourelles(std::vector<Tourelle*>& tourelles);
};

#endif
