/*
#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include <vector>

//#include "enemy.hpp"

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
*/

// ------------------------------------------------------------------------------//
//                  SURBRILLANCE D'UNE CELLULE
// ------------------------------------------------------------------------------//

/* // Met en surbrillance un intervalle d’ids (inclusif) si tu veux visualiser
    static void highlightCellRange(sf::RenderTarget& window,
                                   const std::vector<Cell>& cells,
                                   int idStart, int idEnd,
                                   sf::Color fill = sf::Color(255, 215, 0, 60),   // or
                                   sf::Color outline = sf::Color(255, 215, 0),    // gold
                                   float outlineThickness = 1.f);*/

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace Render {

struct Cell {
    static float cellSize;
    int id;
    int col, row;
    sf::FloatRect bounds;
    sf::Vector2f center;
    bool turreted = false;
};

class Renderer {
public:
    Renderer(unsigned int width = 800, unsigned int height = 600);
    ~Renderer() = default;
    void resizeGrid(unsigned int cols, unsigned int rows);
    void clear();
    void display();

    sf::RenderWindow& getWindow();
    sf::RenderTexture& getScene();

    void drawGrid(sf::Color color);
    //static std::vector<Cell> buildCells(sf::RenderTarget& target);
    static std::vector<Cell> buildCells();
    inline unsigned int getgridCols()const {return gridCols;};
    inline unsigned int getgridRows()const {return gridRows;};

private:
    sf::RenderWindow window;
    sf::RenderTexture sceneTexture;
    sf::Sprite sceneSprite;

    unsigned int gridCols = 20; // nombre de colonnes par défaut
    unsigned int gridRows = 15; // nombre de lignes par défaut

};

} // namespace Render
