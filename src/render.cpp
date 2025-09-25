#include "render.hpp"

#include <cmath>

// ------------------------------ VARIABLES PAR DEFAUT ------------------------------ //
float Render::Cell::cellSize = 40.f; // Définition de la taille de cellule par défaut
float cellSize = Render::Cell::cellSize; // Variable globale pour la taille des cellules (utile dans main.cpp)
// ------------------------------------------------------------------------------------ //



void Render::drawGrid(sf::RenderTarget& window, sf::Color color)
{

    // --- Gestion de la grille pour SFML --- //
    sf::Vector2u size = window.getSize(); // taille de la fenêtre (par défaut 800x600 (voir main.cpp))
    float W = static_cast<float>(size.x); // largeur
    float H = static_cast<float>(size.y); // hauteur

    sf::VertexArray grid(sf::Lines); // dit qu'on va dessiner des lignes entre chaque point

    // Lignes verticales
    for (float x = 0.f; x <= W; x += cellSize) {
        grid.append(sf::Vertex(sf::Vector2f(std::round(x), 0.f), color));
        grid.append(sf::Vertex(sf::Vector2f(std::round(x), H), color));
    }

    // Lignes horizontales
    for (float y = 0.f; y <= H; y += cellSize) {
        grid.append(sf::Vertex(sf::Vector2f(0.f, std::round(y)), color));
        grid.append(sf::Vertex(sf::Vector2f(W, std::round(y)), color));
    }

    window.draw(grid); // une fois qu'on a tout ajouté on dessine

}

std::vector<Render::Cell> Render::buildCells(sf::RenderTarget& window)
{
    // --- Gestion de la grille pour le reste du programme --- //

    sf::Vector2u size = window.getSize(); // taille de la fenêtre (par défaut 800x600 (voir main.cpp))
    const int cols = static_cast<int>(size.x / cellSize); // permet dans le for de dire quel colonne/ligne on est
    const int rows = static_cast<int>(size.y / cellSize);

    std::vector<Cell> cells; 
    cells.reserve(cols * rows); //Epargnement de mémoire pour éviter les reallocations (optimisation)

    int id = 1; // On initialise l'id à 1 pour avoir un tableau qui dit cells 1 à cells n (on pourrais mettre 0)
    
    for (int row = 0; row < rows; ++row) { // On a défini que nos incrémentation (rows, cols) était de la taille d'une cellule plutôt qu'un pixel
        for (int col = 0; col < cols; ++col, ++id) { // On incrémente l'id à chaque cellule 
            const float x = col * cellSize;
            const float y = row * cellSize;

            // On implémente les paramètre de la nouvelle cellule dans notre structure Cell
            Cell c;
            c.id = id;
            c.col = col;
            c.row = row;

            c.bounds = sf::FloatRect(x, y, cellSize, cellSize); // On défini le rectangle de la cellule qui part de (x,y) et fait cellSize en largeur et hauteur
            c.center = { x + (cellSize/2), y + (cellSize/2) }; // On défini le centre de la cellule au milieu du rectangle
            // On ajoute la cellule au tableau
            cells.push_back(c);
        }
    }

    return cells; // On retourne le tableau de cellules
}




// ------------------------------------------------------------------------------//

/*
void Render::drawGrid(sf::RenderTarget& window, float cellSize, sf::Color color)
{
    sf::Vector2u size = window.getSize(); // taille de la fenêtre (par défaut 800x600 (voir main.cpp))
    float W = static_cast<float>(size.x); // largeur
    float H = static_cast<float>(size.y); // hauteur

    sf::VertexArray grid(sf::Lines); // dit qu'on va dessiner des lignes entre chaque point

    // Lignes verticales
    for (float x = 0.f; x <= W; x += cellSize) {
        grid.append(sf::Vertex(sf::Vector2f(std::round(x), 0.f), color));
        grid.append(sf::Vertex(sf::Vector2f(std::round(x), H), color));
    }

    // Lignes horizontales
    for (float y = 0.f; y <= H; y += cellSize) {
        grid.append(sf::Vertex(sf::Vector2f(0.f, std::round(y)), color));
        grid.append(sf::Vertex(sf::Vector2f(W, std::round(y)), color));
    }

    window.draw(grid); // une fois qu'on a tout ajouté on dessine
}

// Construit le tableau de cellules (id, col/row, bounds, centre)
std::vector<Render::Cell> Render::buildCells(sf::RenderTarget& window, float cellSize)
{
    const sf::Vector2u sz = window.getSize();
    const int cols = static_cast<int>(sz.x / cellSize);
    const int rows = static_cast<int>(sz.y / cellSize);

    std::vector<Cell> out;
    out.reserve(cols * rows);

    int id = 1; // 1-based
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col, ++id) {
            const float x = col * cellSize;
            const float y = row * cellSize;
            Cell c;
            c.id = id;
            c.col = col;
            c.row = row;
            c.bounds = sf::FloatRect(x, y, cellSize, cellSize);
            c.center = { x + cellSize * 0.5f, y + cellSize * 0.5f };
            out.push_back(c);
        }
    }
    return out;
}*/

// ------------------------------------------------------------------------------//

/*// Met en surbrillance toutes les cellules dont id ∈ [idStart, idEnd]
void Render::highlightCellRange(sf::RenderTarget& window,
                                const std::vector<Cell>& cells,
                                int idStart, int idEnd,
                                sf::Color fill, sf::Color outline, float outlineThickness)
{
    if (cells.empty()) return;
    if (idStart > idEnd) std::swap(idStart, idEnd);

    // clamp sur la plage valide
    const int minId = cells.front().id;
    const int maxId = cells.back().id;
    idStart = std::max(idStart, minId);
    idEnd   = std::min(idEnd,   maxId);

    sf::RectangleShape rect;
    rect.setFillColor(fill);
    rect.setOutlineColor(outline);
    rect.setOutlineThickness(outlineThickness);

    for (const auto& c : cells) {
        if (c.id < idStart || c.id > idEnd) continue;
        rect.setPosition(c.bounds.left, c.bounds.top);
        rect.setSize({c.bounds.width, c.bounds.height});
        window.draw(rect);
    }
}
*/


  /*  int id = 1;
    for (int row = 0; row < nbRows; ++row) {
        for (int col = 0; col < nbCols; ++col) {
            float cx = col * cellSize + cellSize / 2.f;
            float cy = row * cellSize + cellSize / 2.f;

            txt.setString(std::to_string(id));
            sf::FloatRect bounds = txt.getLocalBounds();
            txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            txt.setPosition(cx, cy);

            window.draw(txt);
            ++id;
        }
    }*/