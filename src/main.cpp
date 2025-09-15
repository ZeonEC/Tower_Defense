////////////////////////////////////////////////////////
//          Programmes de Enzo CHADEVILLE             //
//                                                    //
//                                                    //
//                                                    //
////////////////////////////////////////////////////////

//--------Inclusion des librairies--------//
#include <iostream>
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

//--------Inclusion des autres programmes--------//
#include "enemy.hpp"

//using namespace std;



int main()
{
sf::RenderWindow window(sf::VideoMode(800, 600), "Enemies + SFML");
    window.setFramerateLimit(60);

    // On crée quelques ennemis
    std::vector<std::unique_ptr<Enemy>> enemies;
    enemies.emplace_back(std::make_unique<BasicEnemy>());
    enemies.emplace_back(std::make_unique<FastEnemy>());
    enemies.emplace_back(std::make_unique<TankEnemy>());
    enemies.emplace_back(std::make_unique<TargetEnemy>());
    enemies.emplace_back(std::make_unique<FlyEnemy>());

    // On positionne chaque ennemi en colonne à gauche
    float startX = 60.f;
    float y = 100.f;
    for (auto& e : enemies) {
        e->setPosition(startX, y);
        y += 80.f;
    }

    sf::Clock clock;
    float damageTimer = 0.f;

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Espace = infliger des dégâts à tous pour tester la mort
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                for (auto& e : enemies) e->hit(25);
            }
        }

        float dt = clock.restart().asSeconds();

        // Exemple: dégâts automatiques toutes les 0.6s sur le premier ennemi
        damageTimer += dt;
        if (damageTimer >= 0.6f) {
            damageTimer = 0.f;
            if (!enemies.empty()) {
                enemies.front()->hit(10);
                std::cout << "BasicEnemy HP = " << enemies.front()->getHp()
                          << (enemies.front()->isDead() ? " (dead)\n" : "\n");
            }
        }

        for (auto& e : enemies) e->update(dt);

        // Reset si trop à droite (boucle visuelle)
        for (auto& e : enemies) {
            if (e->getPosition().x > 820.f) e->setPosition(-20.f, e->getPosition().y);
        }

        window.clear(sf::Color(30, 30, 35));
        for (auto& e : enemies) e->draw(window);
        window.display();
    }

    return 0;
}



    //Ici les classes des ennemies ont besoin de savoir les hp/ spd ou seulement un des paramètres,
    //  BasicEnemy goblin(20, 1.5f);
    //  TankEnemy orc(0.5f); 
    //  FastEnemy bat(15);

    //Si je crée un ennemie avec une classe qui définie déja chaque statistique (ici hp et spd) alors je ne met pas de ()
    //  TestEnemy neuille; 
