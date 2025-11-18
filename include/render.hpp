#ifndef RENDER_HPP
#define RENDER_HPP


#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

#include "Player.hpp"

class Tourelle;

// ============================================================================
// CLASSE DE BASE : Render
// ============================================================================
class Render {
public:
    //Définition d'une cellule
    struct Cell {
        int id;
        int col, row;
        sf::FloatRect bounds;
        sf::Vector2f center;
        bool turreted = false;
        static float cellSize;
    };

    Render();
    virtual ~Render();

    sf::RenderTexture& getTexture();
    const sf::Sprite& getSprite() const;
    sf::Vector2u getSize() const;

    virtual void clear() = 0;
    virtual void display();
    virtual void update(float dt);
    virtual void drawTo(sf::RenderWindow& target);

    static void drawGrid(sf::RenderTarget& window, sf::Color color = sf::Color(60, 60, 60));
    static std::vector<Cell> buildCells(sf::RenderTarget& window);

protected:
    sf::RenderTexture texture;
    sf::Sprite sprite;
    sf::Vector2u size;
};

// ============================================================================
// CLASSE DERIVÉE : RenderMainMenu
// ============================================================================
class RenderMainMenu : public Render {

private:

    sf::Sprite background;
    sf::Texture backgroundTexture;

    sf::Font font;
    sf::Text label;

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

            auto r = label.getLocalBounds();
            label.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
            label.setPosition(pos);
        }

        bool contains(sf::Vector2f p) const { return box.getGlobalBounds().contains(p); }   //Retourne si colision
        void draw(sf::RenderTarget& rt) const { rt.draw(box); rt.draw(label); }             //Dessine le bouton
    };

    Button startButton; //Bouton Start
    Button quitButton;  //Bouton Quit

public:
    explicit RenderMainMenu(const sf::Vector2u& windowSize); //Constructeur du RenderMainMenu / explicit empêche les conversions implicite | ne change pas le type de windowSize

    void setupButtons();                                            //Création des boutons
    void drawMenu();                                                //Dessine le MainMenu
    void clear();                                                   //Vide le RenderMainMenu
    const Button& getStartButton() const { return startButton; };   //Getter du bouton Start
    const Button& getQuitButton() const { return quitButton; };     //Getter du bouton Quit

    
};

// ============================================================================
// CLASSE DERIVÉE : RenderMap
// ============================================================================
class RenderMap : public Render {
private :
    sf::Text waveText;
    sf::Font font;
    sf::Sprite background;
    sf::Texture backgroundTexture; 
public:
    explicit RenderMap(const sf::Vector2u& windowSize); //Constructeur du RenderMainMenu / explicit empêche les conversions implicite | ne change pas le type de windowSize
    void clear() override;                                          //Nouvelle définition du clear
    void drawBackground();                                          //Dessine le fond
    void drawGridLines();                                           //Dessine la grille
    void setWaveText(std::string text) {waveText.setString(text);}; //Ecrit le texte des vagues d'ennemi
    sf::Text getWaveText() const {return waveText;};                //Getter pour le texte des vagues d'ennemi
};

// ============================================================================
// CLASSE DERIVÉE : RenderMenu
// ============================================================================

class RenderMenu : public Render {
private:
    sf::Font font;
    sf::Text titleText;

    sf::RectangleShape playButton;
    sf::Text playText;

    sf::RectangleShape restartButton;
    sf::Text restartText;

    sf::RectangleShape quitButton;
    sf::Text quitText;

    bool visible = false;
    bool MenuSelected = false;


public:
    explicit RenderMenu(const sf::Vector2u& windowSize);

    void show() { visible = true; }
    void hide() { visible = false; }
    bool isVisible() const { return visible; }
    void isSelected() {MenuSelected = true;}
    void isNotSelected() {MenuSelected = false;}
    bool Selection() const {return MenuSelected;}
    void clear() override;
    void drawMenu();    // dessiner le menu sur la texture
    void drawOverlay(); // afficher si visible

    const sf::RectangleShape& getPlayButton() const { return playButton; }
    const sf::RectangleShape& getRestartButton() const { return restartButton; }
    const sf::RectangleShape& getQuitButton() const { return quitButton; }
};

// ============================================================================
// CLASSE DERIVÉE : RenderControl
// ============================================================================
class RenderControl : public Render {
private:
    int activeTab;
    std::vector<std::string> tabs;
    sf::Font font;
    sf::Text tabText;
    sf::Text contentText;
    sf::Text TowercostText;
    std::vector<sf::Text> towerCostTexts;

public:
    explicit RenderControl(const sf::Vector2u& windowSize);
    void clear() override;
    void drawTabs();
    void drawContent(Player* player);
    void handleClick(int mouseX, int mouseY, RenderMenu& menu);
    void displayFull(Player* player);
};

// ============================================================================
// CLASSE DERIVÉE : RenderInfo
// ============================================================================
class RenderInfo : public Render {

    private:
    sf::Font font;
    sf::Text TowerText;
    sf::Text healthText;  // texte affichant la valeur de vie
    sf::Text PlayerText;  // texte affichant la valeur de vie
    sf::Text pRessourceText; // texte affichant la valeur des ressources du joueur
    Player* player = nullptr;       // pointeur vers le joueur
    sf::RectangleShape healthBarBg; // fond gris
    sf::RectangleShape healthBar;   // barre verte/rouge
    sf::Text towerDamageText;
    std::unique_ptr<sf::Shape> towerShapePreview;

    Tourelle* selectedTower = nullptr;
public:

    explicit RenderInfo(const sf::Vector2u& windowSize);
    void clearPreview();
    void setPlayer(Player* p) { player = p; }
    void setSelectedTower(Tourelle* t);
    void setPreviewTowerType(int towerType);
    std::unique_ptr<sf::Shape> cloneShape(const sf::Shape* shape);  //Pointe la forme de la tourelle

    void updateTowerPreview(Tourelle* t, int towerType = -1);       //Met à jour le texte en fontion de la tourelle sélectionnée. towerType E [0,5] => =-1 => vide
    void drawInfo();
    void clear() override;
};

// ============================================================================
// CLASSE DERIVÉE : RenderGameOver
// ============================================================================
class RenderGameOver : public Render {
private:
    sf::Font font;
    sf::Text gameOverText;
    sf::RectangleShape restartButton;
    sf::Text restartText;
    sf::RectangleShape quitButton;
    sf::Text quitText;

    bool visible = false;

public:
    explicit RenderGameOver(const sf::Vector2u& windowSize);
    void show();
    void hide();
    bool isVisible() const { return visible; }

    const sf::RectangleShape& getRestartButton() const { return restartButton; }
    const sf::RectangleShape& getQuitButton() const { return quitButton; }

    void clear() override;
    void drawOverlay();
};


#endif // RENDER_HPP

