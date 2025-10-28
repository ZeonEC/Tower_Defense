/*#include "render.hpp"

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
}*/

#include "render.hpp"
#include <iostream>
#include <cmath>

float Render::Cell::cellSize = 40.f;

// -------------------- Constructeur -------------------- //
Render::Renderer::Renderer(unsigned int width, unsigned int height)
    : window(sf::VideoMode(width, height), "Enemies + SFML") 
{
    window.setFramerateLimit(60);

    if (!sceneTexture.create(width, height)) {
        std::cerr << "Erreur : impossible de créer la RenderTexture !\n";
    }
    sceneSprite.setTexture(sceneTexture.getTexture());
    resizeGrid(gridCols, gridRows);
}

// -------------------- Méthodes -------------------- //
void Render::Renderer::clear() {
    sceneTexture.clear(sf::Color(30, 30, 35)); // fond de la scène
}

void Render::Renderer::display() {
    // On affiche la RenderTexture dans la fenêtre
    sceneTexture.display();
    window.clear();
    window.draw(sceneSprite);
    window.display();
}

sf::RenderWindow& Render::Renderer::getWindow() {
    return window;
}

sf::RenderTexture& Render::Renderer::getScene() {
    return sceneTexture;
}

// -------------------- Grille -------------------- //
void Render::Renderer::drawGrid(sf::Color color)
{
    sf::Vector2u size = sceneTexture.getSize();
    float W = static_cast<float>(size.x);
    float H = static_cast<float>(size.y);
    float cellSize = Render::Cell::cellSize;

    sf::VertexArray grid(sf::Lines);
    for (float x = 0.f; x <= W; x += cellSize) {
        grid.append(sf::Vertex({x, 0.f}, color));
        grid.append(sf::Vertex({x, H}, color));
    }
    for (float y = 0.f; y <= H; y += cellSize) {
        grid.append(sf::Vertex({0.f, y}, color));
        grid.append(sf::Vertex({W, y}, color));
    }
    sceneTexture.draw(grid);
}


// 🧮 Calcule une taille de cellule proportionnelle à la fenêtre
void Render::Renderer::resizeGrid(unsigned int cols, unsigned int rows)
{
    sf::Vector2u winSize = window.getSize();
    gridCols = cols;
    gridRows = rows;

    float cellW = static_cast<float>(winSize.x) / cols;
    float cellH = static_cast<float>(winSize.y) / rows;
    Render::Cell::cellSize = std::min(cellW, cellH); // cellule carrée
}

// 🔨 Construit automatiquement les cellules pour la grille actuelle
std::vector<Render::Cell> Render::Renderer::buildCells()
{
    sf::Vector2u size = window.getSize();
    const int cols = getgridCols();
    const int rows = getgridRows();
    float cs = Render::Cell::cellSize;

    std::vector<Render::Cell> cells;
    cells.reserve(cols * rows);

    int id = 1;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col, ++id) {
            const float x = col * cs;
            const float y = row * cs;
            Render::Cell c;
            c.id = id;
            c.col = col;
            c.row = row;
            c.bounds = sf::FloatRect(x, y, cs, cs);
            c.center = {x + cs / 2, y + cs / 2};
            cells.push_back(c);
        }
    }
    return cells;
}