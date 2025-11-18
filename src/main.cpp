
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
#include "gameEvent.hpp"

// Musique du menu
sf::Music MainMenuMusic;

// Musique de fond InGame
sf::Music backgroundMusic;


// Pour les paramètres de base, possible de faire des constantes globales dans un fichier de config !!!!!




int main() {

//---------------------------- INITIALISATION ------------------------------//

    // Initialisation de la fenêtre SFML
    sf::RenderWindow game_window(sf::VideoMode(800, 600), " SHIN MEGAMI TENSEI VI");
    game_window.setFramerateLimit(60);

    //---------------------------- MUSIQUE ------------------------------//
    if (!backgroundMusic.openFromFile("../src/assets/musics/WELCOME_TO_THE_CITY.ogg")) {
    std::cerr << "Erreur chargement musique !" << std::endl;
    }
    if (!MainMenuMusic.openFromFile("../src/assets/musics/MainMenu.ogg")) {
    std::cerr << "Erreur chargement musique !" << std::endl;
    }

    MainMenuMusic.setVolume(5.f);  // entre 0 et 100
    MainMenuMusic.setLoop(true);
    MainMenuMusic.play();

    backgroundMusic.setVolume(5.f);  // entre 0 et 100
    backgroundMusic.setLoop(true);
    

    //------------------- ZONES -------------------//

    RenderMap renderMap(game_window.getSize());
    RenderControl renderControl(game_window.getSize());
    RenderInfo renderInfo(game_window.getSize());
    RenderGameOver renderGameOver(game_window.getSize());
    RenderMenu renderMenu(game_window.getSize());
    RenderMainMenu renderMainMenu(game_window.getSize());


    // Initialisation des acteurs
    std::vector<Enemy*> enemies;      // PAS de new ici pas besoin grace à l'instance de Game
    std::vector<Tourelle*> tourelles;      // PAS de new ici pas besoin grace à l'instance de Game
    std::vector<Render::Cell> cells; // Tableau de cellules pour la grille
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

    AppState appState = AppState::MainMenu;

    GameEvent gameEvent(
    game_window, game, player, renderMenu, renderGameOver,
    renderControl, renderInfo, renderMap, renderMainMenu, pathfinder,
    MainMenuMusic, backgroundMusic,
    enemies, tourelles, cells
    );
//---------------------------- LOOP DU JEU ------------------------------//

    while (game_window.isOpen()) {

    appState = gameEvent.processEvents(appState, currentTowerType);
    

    if (!(appState == AppState::MainMenu)){

    // --- timing global (on calcule toujours dt) ---
    float dt = clock.restart().asSeconds();

    if (game.isGameOver()) {
        renderGameOver.show(); // nouvelle fonction
        if (backgroundMusic.getStatus() == sf::Music::Playing) {
            backgroundMusic.stop();
        }
        appState = AppState::GameOver;
    }

    // --- Rendu/Update selon l'état ---

    if (!game.isGameOver() && !renderMenu.Selection()){
    game.updateWaves(dt, enemies, cells);
    game.update(pathfinder, cells, dt, enemies, tourelles, &player);
    }

    // --- mise à jour du texte de vague ---
    if (!game.isWavesFinished()) {
        renderMap.setWaveText(
            "Vague : " + std::to_string(game.getWaveIndex()) +
            " / " + std::to_string(game.getWaveTotal())
        );
    } else {
        renderMap.setWaveText("Toutes les vagues terminees !");
    }
    

    // Map
    renderMap.clear();
    renderMap.drawBackground();
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

    game_window.draw(renderMap.getWaveText());

    if (game.isGameOver()) {
        if (renderGameOver.isVisible()) {
            renderGameOver.drawOverlay();
            renderGameOver.drawTo(game_window);
        }
    }

    if (renderMenu.Selection()){
        appState = AppState::Menu;
        renderMenu.show();
    if (renderMenu.isVisible()) {
    renderMenu.drawOverlay();
    game_window.draw(renderMenu.getSprite());
    }
    }
    }

    if (appState == AppState::MainMenu) {
    // Dessine le menu principal
    renderMainMenu.drawMenu();       // remplit le render texture
    game_window.clear(sf::Color(0, 0, 0)); // ou autre couleur de fond
    renderMainMenu.drawTo(game_window); // copie sur la fenêtre
    }
    game_window.display();
}
   
    // Libérer la mémoire avant de quitter
    game.destroyEnemy(enemies);
    game.destroyTourelles(tourelles);

    return 0;
}

