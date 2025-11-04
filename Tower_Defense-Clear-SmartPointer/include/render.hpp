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
// CLASSE DERIVÉE : RenderMap
// ============================================================================
class RenderMap : public Render {
public:
    explicit RenderMap(const sf::Vector2u& windowSize);
    void clear() override;
    void drawBackground(const sf::Sprite& bg);
    void drawGridLines();
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
    void handleClick(int mouseX, int mouseY);
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
    void setTowerInfo(const std::string& name);
    void setPlayer(Player* p) { player = p; }
    void setSelectedTower(Tourelle* t);
    void setPreviewTowerType(int towerType);
    std::unique_ptr<sf::Shape> cloneShape(const sf::Shape* shape);

    void updateTowerPreview(Tourelle* t, int towerType = -1);
    void drawInfo();
    void clear() override;
};

#endif // RENDER_HPP
