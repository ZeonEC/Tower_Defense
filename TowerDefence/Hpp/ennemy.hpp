#pragma once
#include <SFML/Graphics.hpp>
#include <ctime>
#include "Astar.hpp"

class PathFinding_AStar;

class Ennemy {


    public:
        
        //Constructeurs
        Ennemy(std::string name,int numEnnemies,int HitPoint,int Shell,int Defence,int attack_damage,int attack_speed,int attack_range,int move_speed, PathFinding_AStar& pathfinder);
        ~Ennemy(); 


        // Definition d'un ennemi
        struct ennemy{
            sf::RectangleShape shape;
            sf::Vector2f spawn_position;
            sf::Vector2f position;
            std::string name;
            int HitPoint;
            int Shell;
            int Defence;
            int attack_damage;
            int attack_speed;
            int attack_range;
            int move_speed;
            std::vector<sf::Vector2f> pathPoints;
            int currentPathIndex = 0;
        };
        sf::RectangleShape CreateShape();
        sf::Vector2f Spawn_Position();
        
        void draw(sf::RenderWindow& rendertexture);
        void move(PathFinding_AStar& pathfinder);
        struct ennemy **groupe_ennemies;

    private:
        
        int m_numEnnemies;
};