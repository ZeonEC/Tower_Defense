#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/Graphics.hpp>

#include "enemy.hpp"

// ====================================== ROLE DE LA CLASSE =========================================== //

// La classe Render s’occupe de tout ce qui concerne la grille visuelle du jeu : elle définit la structure de la carte et fournit les outils pour l’afficher à l’écran.

// ==================================================================================================== //

class Render {

    public:

    // ---------- représentation d’une cellule ---------- //
    struct Cell {
        int id;               // 1-based (1,2,3,...) gauche->droite, haut->bas
        int col, row;         // indices 0-based
        sf::FloatRect bounds; // rectangle en pixels (x,y,w,h)
        sf::Vector2f center;  // centre en pixels
        bool turreted = false; // indique si une tourelle est placée dans cette cellule
        static float cellSize;
    };

    // dessine une grille par-dessus la fenêtre, cellSize : taille d'une cellule en pixels
    static void drawGrid(sf::RenderTarget& window, sf::Color color = sf::Color(60, 60, 60));

    

    // Construit toutes les cellules visibles pour la fenêtre courante
    static std::vector<Cell> buildCells(sf::RenderTarget& window);
    // ------------------------------------------------------------------------------//

};

#endif

// ------------------------------------------------------------------------------//
//                  SURBRILLANCE D'UNE CELLULE
// ------------------------------------------------------------------------------//

/* // Met en surbrillance un intervalle d’ids (inclusif) 

    static void highlightCellRange(sf::RenderTarget& window,
                                   const std::vector<Cell>& cells,
                                   int idStart, int idEnd,
                                   sf::Color fill = sf::Color(255, 215, 0, 60),   // or
                                   sf::Color outline = sf::Color(255, 215, 0),    // gold
                                   float outlineThickness = 1.f);*/