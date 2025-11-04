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
#include <SFML/Audio.hpp> // DELTATUNE DEMAIN

//--------Inclusion des autres programmes--------//

#include "render.hpp"
#include "game.hpp"
#include "enemy.hpp"
#include "Astar.hpp"

sf::Texture bgTexture;
sf::Sprite  bgSprite;

bool loadBackground(const std::string& path) {
    if (!bgTexture.loadFromFile(path)) return false;
    bgTexture.setSmooth(false); // pixel-art
    bgSprite.setTexture(bgTexture);
    bgSprite.setPosition(0.f, 0.f);
    return true;
}

sf::Music backgroundMusic;


// Pour les paramètres de base, possible de faire des constantes globales dans un fichier de config !!!!!


int main() {

//---------------------------- INITIALISATION ------------------------------//

    // Initialisation de la fenêtre SFML
    sf::RenderWindow game_window(sf::VideoMode(800, 600), " SHIN MEGAMI TENSEI VI");
    game_window.setFramerateLimit(60);

    if (!loadBackground("../src/assets/maps/map_forest_800x600_grid.png")) {
        std::cerr << "Erreur : impossible de charger le fond de carte !" << std::endl;
    }

    //---------------------------- MUSIQUE ------------------------------//
    if (!backgroundMusic.openFromFile("../src/assets/musics/WELCOME_TO_THE_CITY.ogg")) {
    std::cerr << "Erreur chargement musique !" << std::endl;
    }
    // volume et boucle
    backgroundMusic.setVolume(10.f);  // entre 0 et 100
    backgroundMusic.setLoop(true);
    backgroundMusic.play();


    //------------------- ZONES -------------------//

    RenderMap renderMap(game_window.getSize());
    RenderControl renderControl(game_window.getSize());
    RenderInfo renderInfo(game_window.getSize());

    // Initialisation des acteurs
    std::vector<Enemy*> enemies;      // PAS de new ici pas besoin grace à l'instance de Game
    std::vector<Tourelle*> tourelles;      // PAS de new ici pas besoin grace à l'instance de Game
    std::vector<Render::Cell> cells; // Tableau de cellules pour la grille
    //cells = Render::buildCells(mapTexture); // On construit les cellules une fois pour toute au début (Seul les caractéristiques des cellules compte dans cette fonction)
    cells = Render::buildCells(renderMap.getTexture());

    Game game;
    Player player;
    renderInfo.setPlayer(&player);
    
    PathFinding_AStar pathfinder;
    pathfinder.CreateNodes(cells);

    // Initialisation des aléatoires
    std::srand(static_cast<unsigned>(std::time(nullptr))); // seed une fois

    // Initialisation d'une clock
    sf::Clock clock;
    const float spawnoffset = 0.5f; // spawn toutes les 1.0s

    float spawnt =0;
    int currentTowerType = 0; // 0=Basic, 1=Poison, 2=Shotgun, 3=Target, 4=Fly


//---------------------------- LOOP DU JEU ------------------------------//

    while (game_window.isOpen()) {
        sf::Event event{};
        while (game_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) game_window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                game_window.close();

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::A: currentTowerType = 0;
                        std::cout << "Tourelle Basic sélectionnée\n";
                        //controlText.setString("Tourelle : Basic");
                        renderInfo.setPreviewTowerType(0);
                        break;

                    case sf::Keyboard::Z: currentTowerType = 1;
                        std::cout << "Tourelle Poison sélectionnée\n";
                        //controlText.setString("Tourelle : Poison");
                        renderInfo.setPreviewTowerType(1);
                        break;

                    case sf::Keyboard::E: currentTowerType = 2;
                        std::cout << "Tourelle Shotgun sélectionnée\n";
                        //controlText.setString("Tourelle : Shotgun");
                        renderInfo.setPreviewTowerType(2);
                        break;

                    case sf::Keyboard::R: currentTowerType = 3;
                        std::cout << "Tourelle Target sélectionnée\n";
                        //controlText.setString("Tourelle : Target");
                        renderInfo.setPreviewTowerType(3);
                        break;

                    case sf::Keyboard::T: currentTowerType = 4;
                        std::cout << "Tourelle Fly sélectionnée\n";
                        //controlText.setString("Tourelle : Fly");
                        renderInfo.setPreviewTowerType(4);
                        break;

                    default: 
                        break;
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                if (event.mouseButton.x <= renderMap.getSize().x && event.mouseButton.y <= renderMap.getSize().y ){
                    game.generateTourelle(tourelles,cells,pathfinder,enemies,mousePos.x,mousePos.y,currentTowerType, &player);
                }
                else if (renderControl.getSprite().getGlobalBounds().contains(mousePos)) {
                    int x = event.mouseButton.x - renderControl.getSprite().getPosition().x;
                    int y = event.mouseButton.y - renderControl.getSprite().getPosition().y;
                    renderControl.handleClick(x, y);
                }
                

            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                // Si clic dans la zone de la MAP
                if (event.mouseButton.x <= renderMap.getSize().x && event.mouseButton.y <= renderMap.getSize().y) {
                Tourelle* selected = nullptr;

                // Parcours de toutes les tourelles
                for (auto* t : tourelles) {
                    if (!t) continue;
                    if (t->getGlobalBounds().contains(mousePos)) { // méthode qu'on va ajouter juste après
                        selected = t;
                        break;
                    }
                }
                if (!selected)
                renderInfo.setSelectedTower(nullptr); // efface l’aperçu


                // On met à jour le panneau info
                renderInfo.setSelectedTower(selected);
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


        //------- UPDATE DES ENTITES -------// 

        game.update(pathfinder, cells, dt, enemies, tourelles, &player);


        // ------- DESSIN DES ZONES ------- //

        // 1) MAP
        renderMap.clear();
        renderMap.drawBackground(bgSprite);
        renderMap.drawGridLines();
        for (auto& e : enemies) e->draw(renderMap.getTexture());
        for (auto& t : tourelles) t->draw(renderMap.getTexture());
        for (const auto& p : game.getProjectiles()) p.draw(renderMap.getTexture());
        renderMap.display();

        // 2) CONTROL
        renderControl.displayFull(&player);

        // 3) INFO
        renderInfo.clear();
        renderInfo.drawInfo();
        renderInfo.display();


        // ------- DESSIN SUR LA FENÊTRE ------- //
        game_window.clear(sf::Color(30, 30, 35));
        renderMap.drawTo(game_window);
        renderInfo.drawTo(game_window);
        renderControl.drawTo(game_window);
        game_window.display();

    }

   
    // Libérer la mémoire avant de quitter
    game.destroyEnemy(enemies);
    game.destroyTourelles(tourelles);

    return 0;
}

