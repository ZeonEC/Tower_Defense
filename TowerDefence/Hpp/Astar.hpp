#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "ennemy.hpp"

class PathFinding_AStar
{
public:
    PathFinding_AStar();
    ~PathFinding_AStar() { delete[] nodes; }

    struct sNode
    {
        bool bObstacle = false;
        bool bVisited = false;
        float fGlobalGoal;
        float fLocalGoal;
        int x, y;
        std::vector<sNode*> vecNeighbours;
        sNode* parent;
    };

    std::vector<sf::Vector2f> SolveAStar(sf::Vector2f& position);

    //void Update(sf::RenderWindow& window, sf::Vector2f& position);

    void Draw(sf::RenderWindow& window);
    sNode* EnnemyNode(sf::Vector2f& position);

    std::vector<sf::Vector2f> getShortestpath() const;

    sNode *nodes = nullptr;
	sNode *nodeStart = nullptr;
	sNode *nodeEnd = nullptr;

    int nNodeSize = 40;
	int nMapWidth = 20;
	int nMapHeight = 20;

    private:



    void CreateNodes();
    std::vector<sf::Vector2f> path;
};




