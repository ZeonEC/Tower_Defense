#include "render.hpp"
#include "tower.hpp"

#include <cmath>
#include <ostream>
#include <sstream>
#include <iostream>

// ============================================================================
// Render (base class)
// ============================================================================
float Render::Cell::cellSize = 40.f;

Render::Render() = default;
Render::~Render() = default;

sf::RenderTexture& Render::getTexture() { return texture; }
const sf::Sprite& Render::getSprite() const { return sprite; }
sf::Vector2u Render::getSize() const { return size; }

void Render::display() { texture.display(); }
void Render::update(float) {}
void Render::drawTo(sf::RenderWindow& target) { target.draw(sprite); }

void Render::drawGrid(sf::RenderTarget& window, sf::Color color) {
    sf::Vector2u size = window.getSize();
    for (float x = 0; x < size.x; x += Cell::cellSize) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(x, 0), color),
            sf::Vertex(sf::Vector2f(x, size.y), color)
        };
        window.draw(line, 2, sf::Lines);
    }
    for (float y = 0; y < size.y; y += Cell::cellSize) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(0, y), color),
            sf::Vertex(sf::Vector2f(size.x, y), color)
        };
        window.draw(line, 2, sf::Lines);
    }
}

std::vector<Render::Cell> Render::buildCells(sf::RenderTarget& window) {
    std::vector<Cell> cells;
    sf::Vector2u size = window.getSize();
    int cols = static_cast<int>(size.x / Cell::cellSize);
    int rows = static_cast<int>(size.y / Cell::cellSize);

    int id = 1;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Cell cell;
            cell.id = id++;
            cell.col = c;
            cell.row = r;
            cell.bounds = sf::FloatRect(c * Cell::cellSize, r * Cell::cellSize, Cell::cellSize, Cell::cellSize);
            cell.center = {cell.bounds.left + Cell::cellSize / 2.f, cell.bounds.top + Cell::cellSize / 2.f};
            cells.push_back(cell);
        }
    }
    return cells;
}

// ============================================================================
// RenderMap
// ============================================================================
RenderMap::RenderMap(const sf::Vector2u& windowSize) {
    size = { static_cast<unsigned>(windowSize.x * 0.7f), windowSize.y };
    texture.create(size.x, size.y);
    sprite.setTexture(texture.getTexture());
    sprite.setPosition(0.f, 0.f);
}

void RenderMap::clear() {
    texture.clear(sf::Color::Black);
}

void RenderMap::drawBackground(const sf::Sprite& bg) {
    texture.draw(bg);
}

void RenderMap::drawGridLines() {
    Render::drawGrid(texture, sf::Color(80, 80, 80));
}

// ============================================================================
// RenderControl
// ============================================================================
RenderControl::RenderControl(const sf::Vector2u& windowSize)
    : activeTab(0) {
    size = { static_cast<unsigned>(windowSize.x * 0.3f), static_cast<unsigned>(windowSize.y * 0.4f) };
    texture.create(size.x, size.y);
    sprite.setTexture(texture.getTexture());
    sprite.setPosition(static_cast<float>(windowSize.x * 0.7f), static_cast<float>(windowSize.y * 0.6f));

    tabs = {"Tourelles", "Ennemis", "Options"};

    if (!font.loadFromFile("../src/assets/police/AGENCYB.TTF")) {
        std::cerr << "Erreur : impossible de charger la police AGENCYB.TTF !" << std::endl;
    }

    tabText.setFont(font);
    tabText.setCharacterSize(18);
    tabText.setFillColor(sf::Color::White);

    contentText.setFont(font);
    contentText.setCharacterSize(16);
    contentText.setFillColor(sf::Color::White);
    contentText.setPosition(20.f, 60.f);

    towerCostTexts.resize(5);
    float baseY = 100.f;
    for (auto& t : towerCostTexts) {
    t.setFont(font);
    t.setCharacterSize(16);
    t.setFillColor(sf::Color::White);
    t.setPosition(40.f, baseY);
    baseY += 20.f; // espacement vertical
}
}

void RenderControl::clear() {
    texture.clear(sf::Color(50, 50, 50));
}

void RenderControl::drawTabs() {
    float x = 10.f;
    for (std::size_t i = 0; i < tabs.size(); ++i) {
        sf::RectangleShape tabRect({70.f, 30.f});
        tabRect.setPosition(x, 10.f);
        tabRect.setFillColor(i == activeTab ? sf::Color(100, 100, 255) : sf::Color(80, 80, 80));
        texture.draw(tabRect);

        tabText.setString(tabs[i]);
        tabText.setPosition(x + 8.f, 15.f);
        texture.draw(tabText);

        x += 80.f;
    }
}

void RenderControl::drawContent(Player* player) {
    switch (activeTab) {
        case 0: { // --- Onglet Tourelles ---
            contentText.setString("Gestion des Tourelles :\n");

            // Liste des tourelles et coûts
            struct TourelleInfo {
                std::string name;
                int cost;
            };

            std::vector<TourelleInfo> tourelles = {
                {"[A] Basic",   BasicTourelle(0.f).getCost()},
                {"[Z] Poison",  PoisonTourelle(0.f).getCost()},
                {"[E] Shotgun", shotgunTourelle(0.f).getCost()},
                {"[R] Target",  TargetTourelle(0.f).getCost()},
                {"[T] Fly",     FlyTourelle(0.f).getCost()}
            };

            // Met à jour chaque ligne de coût
            for (std::size_t i = 0; i < tourelles.size(); ++i) {
                std::ostringstream oss;
                oss << tourelles[i].name << " : " << tourelles[i].cost << " or";
                towerCostTexts[i].setString(oss.str());

                // 🔸 Couleur rouge si le joueur n’a pas assez de ressources pour CE coût
                if (player->getRessources() < tourelles[i].cost)
                    towerCostTexts[i].setFillColor(sf::Color::Red);
                else
                    towerCostTexts[i].setFillColor(sf::Color::White);
            }

            // Dessin
            texture.draw(contentText);
            for (auto& t : towerCostTexts)
                texture.draw(t);
            break;
        }

        case 1:
            contentText.setString("Ennemis :\n- Basic\n- Fast\n- Tank\n- Fly\n- Target");
            texture.draw(contentText);
            break;

        case 2:
            contentText.setString("Options :\n- Volume musique\n- Vitesse de jeu\n- Mode debug");
            texture.draw(contentText);
            break;
    }
}



void RenderControl::handleClick(int mouseX, int mouseY) {
    float x = 10.f;
    for (std::size_t i = 0; i < tabs.size(); ++i) {
        if (mouseX >= x && mouseX <= x + 70.f && mouseY >= 10.f && mouseY <= 40.f) {
            activeTab = static_cast<int>(i);
            std::cout << "Changement d'onglet : " << tabs[i] << std::endl;
        }
        x += 80.f;
    }
}

void RenderControl::displayFull(Player* player) {
    clear();
    drawTabs();
    drawContent(player);
    display();
}

// ============================================================================
// RenderInfo
// ============================================================================
RenderInfo::RenderInfo(const sf::Vector2u& windowSize) {
    size = { static_cast<unsigned>(windowSize.x * 0.3f), static_cast<unsigned>(windowSize.y * 0.6f) };
    texture.create(size.x, size.y);
    sprite.setTexture(texture.getTexture());
    sprite.setPosition(static_cast<float>(windowSize.x * 0.7f), 0.f);

    if (!font.loadFromFile("../src/assets/police/AGENCYB.TTF"))
    std::cerr << "Erreur : impossible de charger la police AGENCYB.TTF !" << std::endl;

    //Player info
    healthBarBg.setSize({200.f, 20.f});
    healthBarBg.setFillColor(sf::Color(50,50,50));
    healthBarBg.setPosition(20.f, 30.f);

    healthBar.setSize({200.f, 20.f});
    healthBar.setFillColor(sf::Color::Green);
    healthBar.setPosition(healthBarBg.getPosition().x, healthBarBg.getPosition().y);

    healthText.setFont(font);
    healthText.setCharacterSize(14);
    healthText.setFillColor(sf::Color::White);
    healthText.setPosition(healthBarBg.getPosition().x + 5.f, healthBarBg.getPosition().y - 2.f); // légèrement au-dessus de la barre

    PlayerText.setFont(font);
    PlayerText.setCharacterSize(14);
    PlayerText.setFillColor(sf::Color::White);
    PlayerText.setPosition(0.f, 0.f);
    PlayerText.setString("Player life");

    pRessourceText.setFont(font);
    pRessourceText.setCharacterSize(14);
    pRessourceText.setFillColor(sf::Color::White);
    pRessourceText.setPosition(0.f, 60.f);

    //Tower info

    TowerText.setFont(font);
    TowerText.setCharacterSize(14);
    TowerText.setFillColor(sf::Color::White);
    TowerText.setPosition(20.f, 100.f);
    TowerText.setString("Tourelle : Basic");

    towerDamageText.setFont(font);
    towerDamageText.setCharacterSize(14);
    towerDamageText.setFillColor(sf::Color::White);
    towerDamageText.setPosition(TowerText.getPosition().x, TowerText.getPosition().y + 20.f );

    // preview shape
    towerShapePreview = nullptr;
    
}

void RenderInfo::setTowerInfo(const std::string& name) {
    TowerText.setString("Tourelle : " + name);
}

void RenderInfo::setSelectedTower(Tourelle* t)
{
    selectedTower = t;
    if (!t) {
        TowerText.setString("Tourelle : -");
        towerDamageText.setString("Damage : -");
        towerShapePreview.reset();
        return;
    }

    TowerText.setString("Tourelle : " + t->getName());
    towerDamageText.setString("Damage : " + std::to_string(t->getDamage()));

    towerShapePreview.reset();

    towerShapePreview = cloneShape(t->getShapePtr());
    if (towerShapePreview) {
        towerShapePreview->setPosition(80.f, 220.f);
        towerShapePreview->setScale(0.5f, 0.5f);
    }
}






void RenderInfo::drawInfo() {
    // Texte info tourelle
    texture.draw(TowerText);
    texture.draw(towerDamageText);

    if (player) {
        int hp = player->getHealth();
        float healthPercent = static_cast<float>(hp) / 100.f;
        healthPercent = std::max(0.f, std::min(1.f, healthPercent)); // clamp 0..1
        healthBar.setSize({200.f * healthPercent, 20.f});

        // couleur de la barre (verte -> rouge)
        if (healthPercent > 0.5f)
            healthBar.setFillColor(sf::Color(0, 165, 0));
        else if (healthPercent > 0.25f)
            healthBar.setFillColor(sf::Color(255, 165, 0)); // orange
        else
            healthBar.setFillColor(sf::Color::Red);
        
        // On dessine d’abord le fond et la barre
        texture.draw(healthBarBg);
        texture.draw(healthBar);

        // Puis on dessine le texte **après** pour qu’il soit au-dessus
        healthText.setString(std::to_string(hp) + " / 100");

        // Optionnel : centrer le texte sur la barre
        sf::FloatRect textRect = healthText.getLocalBounds();
        healthText.setOrigin(textRect.left + textRect.width / 2.f,
                             textRect.top + textRect.height / 2.f);
        healthText.setPosition(healthBarBg.getPosition().x + healthBarBg.getSize().x / 2.f,
                               healthBarBg.getPosition().y + healthBarBg.getSize().y / 2.f);

        pRessourceText.setString("Ressources : " + std::to_string(player->getRessources()));

        texture.draw(healthText);
        texture.draw(PlayerText);
        texture.draw(pRessourceText);
        if (towerShapePreview)
        texture.draw(*towerShapePreview);


    }
}

void RenderInfo::setPreviewTowerType(int towerType) {
    towerShapePreview.reset();
    std::string name;
    int damage = 0;

    switch (towerType) {
        case 0: {
            BasicTourelle t(0.f);
            name = t.getName();
            damage = t.getDamage();
towerShapePreview = cloneShape(t.getShapePtr());
if (towerShapePreview) {
    towerShapePreview->setPosition(80.f, 220.f);
    towerShapePreview->setScale(0.5f, 0.5f);
}

            break;
        }
        case 1: {
            PoisonTourelle t(0.f);
            name = t.getName();
            damage = t.getDamage();
towerShapePreview = cloneShape(t.getShapePtr());
if (towerShapePreview) {
    towerShapePreview->setPosition(80.f, 220.f);
    towerShapePreview->setScale(0.5f, 0.5f);
}

            break;
        }
        case 2: {
            shotgunTourelle t(0.f);
            name = t.getName();
            damage = t.getDamage();
towerShapePreview = cloneShape(t.getShapePtr());
if (towerShapePreview) {
    towerShapePreview->setPosition(80.f, 220.f);
    towerShapePreview->setScale(0.5f, 0.5f);
}

            break;
        }
        case 3: {
            TargetTourelle t(0.f);
            name = t.getName();
            damage = t.getDamage();
towerShapePreview = cloneShape(t.getShapePtr());
if (towerShapePreview) {
    towerShapePreview->setPosition(80.f, 220.f);
    towerShapePreview->setScale(0.5f, 0.5f);
}

            break;
        }
        case 4: {
            FlyTourelle t(0.f);
            name = t.getName();
            damage = t.getDamage();
towerShapePreview = cloneShape(t.getShapePtr());
if (towerShapePreview) {
    towerShapePreview->setPosition(80.f, 220.f);
    towerShapePreview->setScale(0.5f, 0.5f);
}
            break;
        }
        default:
            return;
    }

    // Ajuste position et échelle
    if (towerShapePreview) {
        towerShapePreview->setPosition(80.f, 220.f);
        towerShapePreview->setScale(0.5f, 0.5f);
    }

    TowerText.setString("Tourelle : " + name);
    towerDamageText.setString("Damage : " + std::to_string(damage));
    selectedTower = nullptr;
}

// Fonction utilitaire pour copier n'importe quel sf::Shape
std::unique_ptr<sf::Shape> RenderInfo::cloneShape(const sf::Shape* shape) {
    if (!shape) return nullptr;

    // Cercle
    if (const auto* circle = dynamic_cast<const sf::CircleShape*>(shape)) {
        auto copy = std::make_unique<sf::CircleShape>(circle->getRadius(), circle->getPointCount());
        copy->setPosition(circle->getPosition());
        copy->setScale(circle->getScale());
        copy->setFillColor(circle->getFillColor());
        copy->setOutlineColor(circle->getOutlineColor());
        copy->setOutlineThickness(circle->getOutlineThickness());
        copy->setOrigin(circle->getOrigin());
        return copy;
    }

    // Rectangle
    if (const auto* rect = dynamic_cast<const sf::RectangleShape*>(shape)) {
        auto copy = std::make_unique<sf::RectangleShape>(rect->getSize());
        copy->setPosition(rect->getPosition());
        copy->setScale(rect->getScale());
        copy->setFillColor(rect->getFillColor());
        copy->setOutlineColor(rect->getOutlineColor());
        copy->setOutlineThickness(rect->getOutlineThickness());
        copy->setOrigin(rect->getOrigin());
        return copy;
    }

    // ConvexShape
    if (const auto* convex = dynamic_cast<const sf::ConvexShape*>(shape)) {
        auto copy = std::make_unique<sf::ConvexShape>();
        copy->setPointCount(convex->getPointCount());
        for (size_t i = 0; i < convex->getPointCount(); ++i)
            copy->setPoint(i, convex->getPoint(i));
        copy->setPosition(convex->getPosition());
        copy->setScale(convex->getScale());
        copy->setFillColor(convex->getFillColor());
        copy->setOutlineColor(convex->getOutlineColor());
        copy->setOutlineThickness(convex->getOutlineThickness());
        copy->setOrigin(convex->getOrigin());
        return copy;
    }

    // Autres shapes inconnus
    return nullptr;
}




void RenderInfo::clear() {
    texture.clear(sf::Color(30, 30, 35));
}

