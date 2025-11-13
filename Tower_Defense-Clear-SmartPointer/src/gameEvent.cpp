#include "gameEvent.hpp"

GameEvent::GameEvent(
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
)
: window(window),
  game(game),
  player(player),
  renderMenu(renderMenu),
  renderGameOver(renderGameOver),
  renderControl(renderControl),
  renderInfo(renderInfo),
  renderMap(renderMap),
  renderMainMenu(renderMainMenu),
  pathfinder(pathfinder),
  mainMenuMusic(mainMenuMusic),
  backgroundMusic(backgroundMusic),
  enemies(enemies),
  tourelles(tourelles),
  cells(cells)
{}


// ---------------------------------------------------------------------------
// Gestion principale des événements
// ---------------------------------------------------------------------------
AppState GameEvent::processEvents(AppState currentState, int& currentTowerType) {
    sf::Event event{};
    while (window.pollEvent(event)) {

        // Fermeture de la fenêtre
        if (event.type == sf::Event::Closed)
            window.close();

        // Échap : quitte le jeu depuis n'importe où
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            window.close();

        //Etat du jeu
        switch (currentState) {
            case AppState::MainMenu:
                currentState = handleMainMenuEvents(event,window);
                break;

            case AppState::Playing:
                currentState = handlePlaying(event, currentTowerType);
                break;

            case AppState::Menu:
                currentState = handleMenu(event);
                break;

            case AppState::GameOver:
                currentState = handleGameOver(event);
                break;

            default:
                break;
        }
    }
    return currentState;
}


// ---------------------------------------------------------------------------
// Gestion de l'état PLAYING (jeu actif)
// ---------------------------------------------------------------------------
AppState GameEvent::handlePlaying(const sf::Event& event, int& currentTowerType) {

    // Changement du type de tourelle
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::A: currentTowerType = 0; renderInfo.setPreviewTowerType(0); break;   //Si A, affiche les infos dans renderInfo
            case sf::Keyboard::Z: currentTowerType = 1; renderInfo.setPreviewTowerType(1); break;
            case sf::Keyboard::E: currentTowerType = 2; renderInfo.setPreviewTowerType(2); break;
            case sf::Keyboard::R: currentTowerType = 3; renderInfo.setPreviewTowerType(3); break;
            case sf::Keyboard::T: currentTowerType = 4; renderInfo.setPreviewTowerType(4); break;

            default: break;
        }
    }

    // Clic gauche : placer une tourelle ou interagir
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

        if (event.mouseButton.x <= renderMap.getSize().x && event.mouseButton.y <= renderMap.getSize().y) {
            game.generateTourelle(tourelles, cells, pathfinder, enemies, mousePos.x, mousePos.y, currentTowerType, &player);
        } else if (renderControl.getSprite().getGlobalBounds().contains(mousePos)) {
            int x = event.mouseButton.x - renderControl.getSprite().getPosition().x;
            int y = event.mouseButton.y - renderControl.getSprite().getPosition().y;
            renderControl.handleClick(x, y, renderMenu);
        }
    }

    // Clic droit : sélection d’une tourelle
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        if (mousePos.x <= renderMap.getSize().x && mousePos.y <= renderMap.getSize().y) {
            Tourelle* selected = nullptr;
            for (auto* t : tourelles) {
                if (t && t->getGlobalBounds().contains(mousePos)) {
                    selected = t;
                    break;
                }
            }
            renderInfo.setSelectedTower(selected);
        }
    }

    // Clic molette : upgrade
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        if (mousePos.x <= renderMap.getSize().x && mousePos.y <= renderMap.getSize().y) {
            game.upgradeTourelle(tourelles, &player, mousePos.x, mousePos.y);
        }
    }

    return AppState::Playing;
}

// ---------------------------------------------------------------------------
// Gestion du MENU pause (RenderMenu)
// ---------------------------------------------------------------------------
AppState GameEvent::handleMenu(const sf::Event& event) {
    if (renderMenu.isVisible() &&
        event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        //Si PLAY => retour au jeu
        if (renderMenu.getPlayButton().getGlobalBounds().contains(mp)) {
            renderMenu.hide();
            renderMenu.isNotSelected();
            return AppState::Playing;
        }
        //SI RESTART => relance le jeu (avec réinitialisation)
        else if (renderMenu.getRestartButton().getGlobalBounds().contains(mp)) {
            resetGame();
            backgroundMusic.stop();
            
            renderMenu.hide();
            renderMenu.isNotSelected();
            backgroundMusic.play();
            return AppState::Playing;
        }
        //Si QUIT => quitte le jeu et retourne au menu principal
        else if (renderMenu.getQuitButton().getGlobalBounds().contains(mp)) {
            resetGame();
            backgroundMusic.stop();
            renderMenu.hide();
            renderMenu.isNotSelected();
            mainMenuMusic.play();
            return AppState::MainMenu;
        }
    }
    return AppState::Menu;
}

// ---------------------------------------------------------------------------
// Gestion du GAME OVER
// ---------------------------------------------------------------------------
AppState GameEvent::handleGameOver(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

        if (backgroundMusic.getStatus() == sf::Music::Playing)
            backgroundMusic.stop();

        if (renderGameOver.getRestartButton().getGlobalBounds().contains(mousePos)) {
            resetGame();
            if (mainMenuMusic.getStatus() == sf::Music::Playing) mainMenuMusic.stop();
            if (backgroundMusic.getStatus() != sf::Music::Playing) backgroundMusic.play();
            renderGameOver.hide();
            return AppState::Playing;
        }

        if (renderGameOver.getQuitButton().getGlobalBounds().contains(mousePos)) {
            resetGame();
            renderGameOver.hide();
            if (mainMenuMusic.getStatus() != sf::Music::Playing)
                mainMenuMusic.play();
            return AppState::MainMenu;
        }
    }
    return AppState::GameOver;
}


// ---------------------------------------------------------------------------
// Gestion du MENU PRINCIPAL
// ---------------------------------------------------------------------------
AppState GameEvent::handleMainMenuEvents(const sf::Event& event,sf::RenderWindow& window) {
    // Touche Entrée : démarre le jeu
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        game.startWaves();
        if (mainMenuMusic.getStatus() == sf::Music::Playing) mainMenuMusic.stop();
        if (backgroundMusic.getStatus() != sf::Music::Playing) backgroundMusic.play();
        return AppState::Playing;
    }

    // Clic souris sur les boutons du menu
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        if (renderMainMenu.getStartButton().box.getGlobalBounds().contains(mp)) {
            game.startWaves();
            if (mainMenuMusic.getStatus() == sf::Music::Playing) mainMenuMusic.stop();
            if (backgroundMusic.getStatus() != sf::Music::Playing) backgroundMusic.play();
            return AppState::Playing;
        }
        if (renderMainMenu.getQuitButton().box.getGlobalBounds().contains(mp)) {
            window.close();
        }
    }

    return AppState::MainMenu;
}


// ---------------------------------------------------------------------------
// Fonction utilitaire : réinitialiser la partie
// ---------------------------------------------------------------------------
void GameEvent::resetGame() {
    std::cout << "Réinitialisation du jeu...\n";

    //Détruire tous les ennemis, tourelles et cellules existants
    game.destroyEnemy(enemies);
    game.destroyTourelles(tourelles);
    enemies.clear();
    tourelles.clear();
    cells.clear();


    //Réinitialiser le joueur
    player = Player();

    //Réinitialiser le jeu
    game = Game();
    cells = Render::buildCells(renderMap.getTexture());     //Reconstruction des cellules
    pathfinder = PathFinding_AStar();
    pathfinder.CreateNodes(cells);                          //Reconstruction des noeuds

    //Reconnecter le joueur à renderInfo
    renderInfo.setPlayer(&player);
    renderInfo.clearPreview();
    renderInfo.clear();

    //Recréer les vagues
    game.startWaves();
}
