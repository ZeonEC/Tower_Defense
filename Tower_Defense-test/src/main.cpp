////////////////////////////////////////////////////////
//          Programmes de Enzo CHADEVILLE             //
//                                                    //
//                                                    //
//                                                    //
////////////////////////////////////////////////////////

//--------Inclusion des librairies--------//
#include <iostream>
//#include <memory> //Librairie utile pour les unique_ptr (pointeur intelligent, plus avancé que les pointeur de base)
#include <vector>
#include <SFML/Graphics.hpp>

//--------Inclusion des autres programmes--------//

#include "render.hpp"
#include "game.hpp"
#include "enemy.hpp"
#include "Astar.hpp"


//using namespace std;

// Pour les paramètres de base, possible de faire des constantes globales dans un fichier de config !!!!!

/*
int main() {

//---------------------------- INITIALISATION ------------------------------//

    // Initialisation de la fenêtre SFML
    sf::RenderWindow game_window(sf::VideoMode(800, 600), "Enemies + SFML");
    game_window.setFramerateLimit(60);
    //float cellSize = 20.f;

    // Initialisation des acteurs
    std::vector<Enemy*> enemies;      // PAS de new ici pas besoin grace à l'instance de Game
    std::vector<Tourelle*> tourelles;      // PAS de new ici pas besoin grace à l'instance de Game
    std::vector<Render::Cell> cells; // Tableau de cellules pour la grille
    cells = Render::buildCells(game_window); // On construit les cellules une fois pour toute au début (Seul les caractéristiques des cellules compte dans cette fonction)
    Game game;
    PathFinding_AStar pathfinder;
    pathfinder.CreateNodes(cells);                       

    // Initialisation des aléatoires
    std::srand(static_cast<unsigned>(std::time(nullptr))); // seed une fois

    // Initialisation d'une clock
    sf::Clock clock;
    const float spawnoffset = 0.5f; // spawn toutes les 1.0s

    float spawnt = 0.f;

//---------------------------- LOOP DU JEU ------------------------------//

    while (game_window.isOpen()) {
        sf::Event event{};
        while (game_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) game_window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                game_window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                //game.generateEnemy(enemies);  // exemple d’utilisation
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;
                game.generateTourelle(tourelles,cells,pathfinder,x,y);
                for (auto* e : enemies){
                    e->needRepath = true;
                }

            }     
        }
       
        // --- timing ---
        float dt = clock.restart().asSeconds(); // dt mis à jour A CHAQUE FRAME
        spawnt += dt;

        // Spawn tant qu'on a dépassé l'intervalle (robuste si un frame lag)
        while (spawnt >= spawnoffset) {
            game.generateEnemy(enemies,cells);
            spawnt -= spawnoffset;
        }


        //On nettoie la fenêtre de la frame précédente
        game_window.clear(sf::Color(30, 30, 35));


        //------- UPDATE DES ENTITES -------// 

        //for (auto& e : enemies) e->update(dt);
        //for (auto& t : tourelles) t->update(dt);
        pathfinder.updateEnemyPresence(enemies); // ✅ met à jour les nodes occupés par des ennemis
        for (auto& e : enemies) e->update(pathfinder,cells);
        for (auto& t : tourelles) t->update(dt);

        //------- DESSIN DANS LA FENETRE SFML -------// 

        //--A FAIRE AVANT LES AUTRES ENTITES--// 
        //On redessine les grilles
        Render::drawGrid(game_window, sf::Color(80, 80, 80));

        // Mise à jour et dessin des ennemis
        for (auto& e : enemies) e->draw(game_window);
        for (auto& t : tourelles) t->draw(game_window);
        game_window.display();
    }

   
    // Libérer la mémoire avant de quitter
    game.destroyEnemy(enemies);
    game.destroyTourelles(tourelles);

    return 0;
}
*/

int main() {
    Render::Renderer render;  // notre moteur de rendu
    auto& window = render.getWindow();
    auto& scene  = render.getScene();

    std::vector<Render::Cell> cells = Render::Renderer::buildCells();

    Game game;
    PathFinding_AStar pathfinder;
    pathfinder.CreateNodes(cells);

    std::vector<Enemy*> enemies;
    std::vector<Tourelle*> tourelles;

    sf::Clock clock;
    float spawnTimer = 0.f;
    const float spawnRate = 0.5f;

    while (window.isOpen()) {
        sf::Event event{};

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;
                game.generateTourelle(tourelles, cells, pathfinder, x, y);
                for (auto* e : enemies) e->needRepath = true;
            }
            if (event.type == sf::Event::Resized) {
                render.resizeGrid(20, 15); // recalcule cellSize
                cells = render.buildCells(); // reconstruit la grille
                pathfinder.CreateNodes(cells); // recalcule les nœuds
            }
        }

        float dt = clock.restart().asSeconds();
        spawnTimer += dt;
        while (spawnTimer >= spawnRate) {
            game.generateEnemy(enemies, cells);
            spawnTimer -= spawnRate;
        }

        // --- Rendu ---
        render.clear();
        pathfinder.updateEnemyPresence(enemies);
        for (auto& e : enemies) e->update(pathfinder, cells);
        for (auto& t : tourelles) t->update(dt);

        render.drawGrid(sf::Color(80, 80, 80));
        for (auto& e : enemies) e->draw(scene);
        for (auto& t : tourelles) t->draw(scene);
        render.display();
    }

    game.destroyEnemy(enemies);
    game.destroyTourelles(tourelles);
}

