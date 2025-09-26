#include <SFML/Graphics.hpp>

#pragma once

class TOWER {
    public:
        TOWER(std::string name, int HitPoint, int Shield, int Defence, int attack_damage, int attack_speed, int attack_range);
        ~TOWER();

        struct TOWER_STATS {
            std::string name;
            sf::RectangleShape shape;
            sf::Vector2f position;
            int HitPoint;
            int Shield;
            int Defence;
            int attack_damage;
            int attack_speed;
            int attack_range;
        };

        sf::RectangleShape CreateShape();
        void draw(sf::RenderWindow& rendertexture);


    private:

};