#ifndef GAME_EVENT_HPP
#define GAME_EVENT_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>

#include "game.hpp"
#include "render.hpp"
#include "enemy.hpp"
#include "Astar.hpp"

enum class AppState {
    MainMenu,
    Playing,
    Menu,
    GameOver
};

class GameEvent {
public:
    GameEvent(
        sf::RenderWindow& window,
        Game& game,
        Player& player,
        RenderMenu& renderMenu,
        RenderGameOver& renderGameOver,
        RenderControl& renderControl,
        RenderInfo& renderInfo,
        RenderMap& renderMap,
        RenderMainMenu& renderMainMenu,
        PathFinding_AStar& pathfinder,
        sf::Music& mainMenuMusic,
        sf::Music& backgroundMusic,
        std::vector<Enemy*>& enemies,
        std::vector<Tourelle*>& tourelles,
        std::vector<Render::Cell>& cells
    );

    /// Gère tous les événements de la frame et retourne le nouvel état
    AppState processEvents(AppState currentState, int& currentTowerType);

private:
    // Références vers les éléments du jeu
    sf::RenderWindow& window;
    Game& game;
    Player& player;
    RenderMenu& renderMenu;
    RenderGameOver& renderGameOver;
    RenderControl& renderControl;
    RenderInfo& renderInfo;
    RenderMap& renderMap;
    RenderMainMenu& renderMainMenu;
    PathFinding_AStar& pathfinder;
    sf::Music& mainMenuMusic;
    sf::Music& backgroundMusic;

    std::vector<Enemy*>& enemies;
    std::vector<Tourelle*>& tourelles;
    std::vector<Render::Cell>& cells;

    // Méthodes internes pour chaque état
    AppState handleMainMenu(const sf::Event& event);
    AppState handlePlaying(const sf::Event& event, int& currentTowerType);
    AppState handleMenu(const sf::Event& event);
    AppState handleGameOver(const sf::Event& event);
    AppState handleMainMenuEvents(const sf::Event& event, sf::RenderWindow& window);

    // Méthode utilitaire
    void resetGame();
};

#endif
