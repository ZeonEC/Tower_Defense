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
    backgroundMusic.setVolume(1.f);  // entre 0 et 100
    backgroundMusic.setLoop(true);
    backgroundMusic.play();


    //------------------- ZONES -------------------//

    /*
    sf::Vector2u winSize = game_window.getSize();
    sf::Vector2u mapSize     = { static_cast<unsigned>(winSize.x * 0.7f), winSize.y };
    sf::Vector2u controlSize = { static_cast<unsigned>(winSize.x - mapSize.x), winSize.y * 0.4f };
    sf::Vector2u infoSize    = { static_cast<unsigned>(winSize.x - mapSize.x), winSize.y * 0.6f};

    sf::RenderTexture mapTexture;
    mapTexture.create(mapSize.x, mapSize.y);
    sf::RenderTexture controlTexture;
    controlTexture.create(controlSize.x, controlSize.y);
    sf::RenderTexture infoTexture;
    infoTexture.create(infoSize.x, infoSize.y);

    sf::Sprite mapSprite(mapTexture.getTexture());
    sf::Sprite controlSprite(controlTexture.getTexture());
    sf::Sprite infoSprite(infoTexture.getTexture());

    // Positionner les sprites pour qu’ils s’alignent
    mapSprite.setPosition(0.f, 0.f);
    infoSprite.setPosition(static_cast<float>(mapSize.x), 0.f);
    controlSprite.setPosition(static_cast<float>(mapSize.x), static_cast<float>(infoSize.y));
    */
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


/*
    sf::Font font;
    if (!font.loadFromFile("../src/assets/police/AGENCYB.TTF")) { // ou une autre police que tu as
        std::cerr << "Erreur : impossible de charger la police !" << std::endl;
    }

    sf::Text controlText;
    controlText.setFont(font);
    controlText.setCharacterSize(18);
    controlText.setFillColor(sf::Color::White);
    controlText.setPosition(20.f, 20.f);
    controlText.setString("Tourelle : Basic");

    // Onglets control
    int currentTab = 0; // 0 = Tourelles, 1 = Ennemis, 2 = Options

    sf::Text tabText;
    tabText.setFont(font);
    tabText.setCharacterSize(14);
    tabText.setFillColor(sf::Color::White);

    // Onglets (nom + zones)
    std::vector<std::string> tabNames = {"Tourelles", "Ennemis", "Menu"};
    std::vector<sf::FloatRect> tabAreas;
    for (int i = 0; i < 3; ++i)
        tabAreas.emplace_back(10.f + i * 80.f, 10.f, 70.f, 30.f); // x,y,w,h
*/

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
                        renderInfo.setTowerInfo("Basic");
                        break;

                    case sf::Keyboard::Z: currentTowerType = 1;
                        std::cout << "Tourelle Poison sélectionnée\n";
                        //controlText.setString("Tourelle : Poison");
                        renderInfo.setTowerInfo("Poison");
                        break;

                    case sf::Keyboard::E: currentTowerType = 2;
                        std::cout << "Tourelle Shotgun sélectionnée\n";
                        //controlText.setString("Tourelle : Shotgun");
                        renderInfo.setTowerInfo("Shotgun");
                        break;

                    case sf::Keyboard::R: currentTowerType = 3;
                        std::cout << "Tourelle Target sélectionnée\n";
                        //controlText.setString("Tourelle : Target");
                        renderInfo.setTowerInfo("Target");
                        break;

                    case sf::Keyboard::T: currentTowerType = 4;
                        std::cout << "Tourelle Fly sélectionnée\n";
                        //controlText.setString("Tourelle : Fly");
                        renderInfo.setTowerInfo("Fly");
                        break;

                    default: 
                        break;
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                /*
                if (event.mouseButton.x <= mapSize.x && event.mouseButton.y <= mapSize.y ){
                    game.generateTourelle(tourelles,cells,pathfinder,mousePos.x,mousePos.y,currentTowerType);
                }
                
                else if (event.mouseButton.x <= controlSize.x && event.mouseButton.y <= controlSize.y ){
                    // Ajuste la position si ta zone Control est à droite de la map


                    for (int i = 0; i < (int)tabAreas.size(); ++i) {
                        if (tabAreas[i].contains(mousePos)) {
                            currentTab = i;
                            std::cout << "Changement d’onglet : " << tabNames[i] << "\n";
                        }
                    }
                };
                */
                if (event.mouseButton.x <= renderMap.getSize().x && event.mouseButton.y <= renderMap.getSize().y ){
                    game.generateTourelle(tourelles,cells,pathfinder,enemies,mousePos.x,mousePos.y,currentTowerType, &player);
                }
                else if (renderControl.getSprite().getGlobalBounds().contains(mousePos)) {
                    int x = event.mouseButton.x - renderControl.getSprite().getPosition().x;
                    int y = event.mouseButton.y - renderControl.getSprite().getPosition().y;
                    renderControl.handleClick(x, y);
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

        // Si l'ennemie atteint le bout de la fenêtre il reviens au début
        /*for (auto& e : enemies) {
            if (e->getPosition().x > 820.f) e->setPosition(-20.f, e->getPosition().y);
        }*/

        //On nettoie la fenêtre de la frame précédente
        //game_window.clear(sf::Color(30, 30, 35));


        //------- UPDATE DES ENTITES -------// 

        //for (auto& e : enemies) e->update(dt);
        //for (auto& t : tourelles) t->update(dt);

        game.update(pathfinder, cells, dt, enemies, tourelles, &player);

        //------- DESSIN DANS LA FENETRE SFML -------// 

        //--A FAIRE AVANT LES AUTRES ENTITES--// 
        //On redessine les grilles

        // ----------------- DESSIN DES ZONES ----------------- //
        // 1) Map
        /*
        mapTexture.clear(sf::Color::Black);
        mapTexture.draw(bgSprite); // fond de carte
        Render::drawGrid(mapTexture, sf::Color(30, 30, 35));
        for (auto& e : enemies) e->draw(mapTexture);
        for (auto& t : tourelles) t->draw(mapTexture);
        for (const auto& p : game.getProjectiles()) p.draw(mapTexture);
        mapTexture.display();

        // 2) Control (ex: boutons)
        controlTexture.clear(sf::Color(50, 50, 50));
                // --- Barre d'onglets ---
        for (int i = 0; i < (int)tabAreas.size(); ++i) {
            sf::RectangleShape tabBg;
            tabBg.setPosition(tabAreas[i].left, tabAreas[i].top);
            tabBg.setSize({tabAreas[i].width, tabAreas[i].height});
            tabBg.setFillColor(i == currentTab ? sf::Color(100, 100, 180) : sf::Color(80, 80, 80));
            controlTexture.draw(tabBg);
        
            tabText.setString(tabNames[i]);
            tabText.setPosition(tabAreas[i].left + 10.f, tabAreas[i].top + 5.f);
            controlTexture.draw(tabText);
        }

        // --- Contenu selon l’onglet sélectionné ---
        sf::Text content("", font,18);
        content.setFillColor(sf::Color::White);
        content.setPosition(20.f, 60.f);

        switch (currentTab) {
            case 0:
                content.setString("Gestion des Tourelles\n[A] Basic\n[Z] Poison\n[E] Shotgun\n[R] Target");
                break;
            case 1:
                content.setString("Ennemis :\n- Basic\n- Fast\n- Tank\n- Fly\n- Target");
                break;
            case 2:
                content.setString("Options :\n- Volume musique\n- Vitesse de jeu\n- Mode debug");
                break;
        }
        controlTexture.draw(content);

        controlTexture.display();

        // 3) Info (ex: score, stats)
        infoTexture.clear(sf::Color(30, 30, 35));
        infoTexture.draw(controlText);
        infoTexture.display();

        // ----------------- DESSIN SUR LA FENÊTRE PRINCIPALE ----------------- //
        game_window.clear(sf::Color(30, 30, 35));
        game_window.draw(mapSprite);
        game_window.draw(controlSprite);
        game_window.draw(infoSprite);
        game_window.display();*/

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
        renderControl.displayFull();

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

