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
// Musique du menu
sf::Music MainMenuMusic;

// Musique de fond InGame
sf::Music backgroundMusic;


// Pour les paramètres de base, possible de faire des constantes globales dans un fichier de config !!!!!

// -------------------- MENU --------------------

enum class AppState {
    Menu,
    Playing,
    Paused,
    GameOver
};

struct Button {
    sf::RectangleShape box;
    sf::Text label;

    void set(const sf::Font& font, const std::string& txt, sf::Vector2f size, sf::Vector2f pos) {
        box.setSize(size);
        box.setFillColor(sf::Color(60, 60, 120));
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(2.f);
        box.setOrigin(size.x * 0.5f, size.y * 0.5f);
        box.setPosition(pos);

        label.setFont(font);
        label.setCharacterSize(28);
        label.setString(txt);
        label.setFillColor(sf::Color::White);

        // centrer le texte dans le bouton
        auto r = label.getLocalBounds();
        label.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
        label.setPosition(pos);

        
    }

    bool contains(sf::Vector2f p) const {
        return box.getGlobalBounds().contains(p);
    }

    void draw(sf::RenderTarget& rt) const {
        rt.draw(box);
        rt.draw(label);
    }
};



int main() {

//---------------------------- INITIALISATION ------------------------------//

    // Initialisation de la fenêtre SFML
    sf::RenderWindow game_window(sf::VideoMode(800, 600), " SHIN MEGAMI TENSEI VI");
    game_window.setFramerateLimit(60);

    if (!loadBackground("../src/assets/maps/map_forest_800x600_grid.png")) {
        std::cerr << "Erreur : impossible de charger le fond de carte !" << std::endl;
    }

//---------------------------- MENU ------------------------------//
    AppState appState = AppState::Menu;

    // --- assets du MENU ---
    sf::Texture menuBgTex;
    sf::Sprite  menuBg;
    sf::Font    uiFont;
    Button      startBtn;

    if (!menuBgTex.loadFromFile("../src/assets/maps/menu_bg.png")) {
        std::cerr << "Erreur : menu_bg.png introuvable.\n";
    }
    menuBgTex.setSmooth(false);
    menuBg.setTexture(menuBgTex);
    menuBg.setPosition(0.f, 0.f);

    // Recharge la même police (ou une autre)
    if (!uiFont.loadFromFile("../src/assets/police/AGENCYB.TTF")) {
        std::cerr << "Erreur : police du menu introuvable.\n";
    }

    // bouton centré
    startBtn.set(uiFont, "DEMARRER", {260.f, 64.f}, {400.f, 420.f});

    // TEXTE DE VAGUE D'ENEMIES
    sf::Text waveText;
    waveText.setFont(uiFont);
    waveText.setCharacterSize(22);
    waveText.setFillColor(sf::Color::White);
    waveText.setPosition(10.f, 5.f); // en haut à gauche


    //---------------------------- MUSIQUE ------------------------------//
    if (!backgroundMusic.openFromFile("../src/assets/musics/WELCOME_TO_THE_CITY.ogg")) {
    std::cerr << "Erreur chargement musique !" << std::endl;
    }
    if (!MainMenuMusic.openFromFile("../src/assets/musics/MainMenu.ogg")) {
    std::cerr << "Erreur chargement musique !" << std::endl;
    }

    MainMenuMusic.setVolume(10.f);  // entre 0 et 100
    MainMenuMusic.setLoop(true);
    MainMenuMusic.play();

    backgroundMusic.setVolume(10.f);  // entre 0 et 100
    backgroundMusic.setLoop(true);
    

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

        // ESC: quitter depuis n'importe quel état
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            game_window.close();
        }

        //---------------------------- ETAT : MENU ------------------------------//
        if (appState == AppState::Menu) {
            // Clic sur le bouton "DEMARRER"
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                                static_cast<float>(event.mouseButton.y));
                if (startBtn.contains(mp)) {
                    appState = AppState::Playing;
                    game.startWaves();
                    if (MainMenuMusic.getStatus() == sf::Music::Playing) MainMenuMusic.stop();
                    if (backgroundMusic.getStatus() != sf::Music::Playing) backgroundMusic.play();
                }
            }

            // Touche Entrée = démarrer
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                appState = AppState::Playing;
                game.startWaves();
                if (MainMenuMusic.getStatus() == sf::Music::Playing) MainMenuMusic.stop();
                if (backgroundMusic.getStatus() != sf::Music::Playing) backgroundMusic.play();
                // backgroundMusic.play();
            }
        }
        //---------------------------- ETAT : INGAME ------------------------------//
        else if (appState == AppState::Playing) {
           
            
            // ---- tes contrôles actuels du JEU (inchangés) ----
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::A: currentTowerType = 0; renderInfo.setPreviewTowerType(0); break;
                    case sf::Keyboard::Z: currentTowerType = 1; renderInfo.setPreviewTowerType(1); break;
                    case sf::Keyboard::E: currentTowerType = 2; renderInfo.setPreviewTowerType(2); break;
                    case sf::Keyboard::R: currentTowerType = 3; renderInfo.setPreviewTowerType(3); break;
                    case sf::Keyboard::T: currentTowerType = 4; renderInfo.setPreviewTowerType(4); break;
                    
                    
                    case sf::Keyboard::Q :
                        { 
                            appState = AppState::Menu; 
                            break;
                        }; 
                    default: break;
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
                if (event.mouseButton.x <= renderMap.getSize().x && event.mouseButton.y <= renderMap.getSize().y) {
                    Tourelle* selected = nullptr;
                    for (auto* t : tourelles) {
                        if (!t) continue;
                        if (t->getGlobalBounds().contains(mousePos)) {
                            selected = t; break;
                        }
                    }
                    if (!selected) renderInfo.setSelectedTower(nullptr);
                    renderInfo.setSelectedTower(selected);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (mousePos.x <= renderMap.getSize().x && mousePos.y <= renderMap.getSize().y) {
                    game.upgradeTourelle(tourelles, &player, mousePos.x, mousePos.y);
                }
            }
        }
    } // fin pollEvent

    // --- timing global (on calcule toujours dt) ---
    float dt = clock.restart().asSeconds();

    // --- Rendu/Update selon l'état ---
    if (appState == AppState::Menu) {
        // juste l’écran de menu
        game_window.clear(sf::Color(20, 20, 30));
        game_window.draw(menuBg);
        startBtn.draw(game_window);
        game_window.display();
        continue; // ne fait pas tourner le jeu
    }

    // ----- Etat PLAYING : ton jeu d'origine -----
    
    // bOUCLE DE SPAWN RANDOM
    /*spawnt += dt;
    while (spawnt >= spawnoffset) {
        game.generateEnemy(enemies,cells);
        spawnt -= spawnoffset;
    }*/
    //game.updateWaves(dt, enemies, cells);

    game.updateWaves(dt, enemies, cells);
    game.update(pathfinder, cells, dt, enemies, tourelles, &player);
    

    // --- mise à jour du texte de vague ---
    if (!game.isWavesFinished()) {
        waveText.setString(
            "Vague : " + std::to_string(game.getWaveIndex()) +
            " / " + std::to_string(game.getWaveTotal())
        );
    } else {
        waveText.setString("Toutes les vagues terminées !");
    }


    // Map
    renderMap.clear();
    renderMap.drawBackground(bgSprite);
    renderMap.drawGridLines();
    for (auto& e : enemies) e->draw(renderMap.getTexture());
    for (auto& t : tourelles) t->draw(renderMap.getTexture());
    for (const auto& p : game.getProjectiles()) p.draw(renderMap.getTexture());
    renderMap.display();

    // Control
    renderControl.displayFull(&player);

    // Info
    renderInfo.clear();
    renderInfo.drawInfo();
    renderInfo.display();

    // Fenêtre
    game_window.clear(sf::Color(30, 30, 35));
    renderMap.drawTo(game_window);
    renderInfo.drawTo(game_window);
    renderControl.drawTo(game_window);

    game_window.draw(waveText);
    game_window.display();

    game_window.display();
}


   
    // Libérer la mémoire avant de quitter
    game.destroyEnemy(enemies);
    game.destroyTourelles(tourelles);

    return 0;
}

