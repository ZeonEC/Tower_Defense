#include "Astar.hpp"
#include <cmath>


PathFinding_AStar::PathFinding_AStar()
{
    CreateNodes();
}


void PathFinding_AStar::CreateNodes()
{
    nodes = new sNode[nMapWidth * nMapHeight]; // Create a array of nodes

    // Initialize nodes
    for (int x = 0; x < nMapWidth; x++)
        for (int y = 0; y < nMapHeight; y++)
        {   
            sNode& node = nodes[y * nMapWidth + x];
            node.x = x * nNodeSize +nNodeSize/2;
            node.y = y * nNodeSize +nNodeSize/2;
            node.bObstacle = false;
            node.parent = nullptr;
            node.bVisited = false;
        }
    // Add for each node a vectors of its neighbors
    for (int x = 0; x < nMapWidth; x++)
        for (int y = 0; y < nMapHeight; y++)
        {
            if (y > 0)
                nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x)]);
            if (y < nMapHeight - 1)
                nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x)]);
            if (x > 0)
                nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[y * nMapWidth + (x - 1)]);
            if (x < nMapWidth - 1)
                nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[y * nMapWidth + (x + 1)]);
            
            // diagonal Neighbors
            if (y>0 && x>0)
            	nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x - 1)]);
            if (y<nMapHeight-1 && x>0)
            	nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x - 1)]);
            if (y>0 && x<nMapWidth-1)
            	nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x + 1)]);
            if (y<nMapHeight - 1 && x<nMapWidth-1)
            	nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x + 1)]);
        }

            nodeEnd = &nodes[nMapWidth * nMapHeight - 1];
}



// Get the node corresponding to the ennemy position
PathFinding_AStar::sNode* PathFinding_AStar::EnnemyNode(sf::Vector2f& position)
{
    int x = position.x / nNodeSize;
    int y = position.y / nNodeSize;

    if (x >= 0 && x < nMapWidth && y >= 0 && y < nMapHeight)
        return &nodes[y * nMapWidth + x];
    else    return nullptr;
}

std::vector<sf::Vector2f> PathFinding_AStar::SolveAStar(sf::Vector2f& position)
{
    path.clear();
    nodeStart = EnnemyNode(position);
    for (int i = 0; i < nMapWidth * nMapHeight; i++)
    {
        nodes[i].bVisited = false;
        nodes[i].fGlobalGoal = INFINITY;
        nodes[i].fLocalGoal = INFINITY;
        nodes[i].parent = nullptr;
    }
    //calcul the path distance
    auto distance = [](sNode* a, sNode* b)
    {
        return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
    };

    auto heuristic = distance;

    sNode* nodeCurrent = nodeStart;

    nodeStart->fLocalGoal = 0.0f;
    nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

    std::vector<sNode*> listNotTestedNodes;
    listNotTestedNodes.push_back(nodeStart);
    while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)
    {
        std::sort(listNotTestedNodes.begin(), listNotTestedNodes.end(),
            [](const sNode* lhs, const sNode* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });

        while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
            listNotTestedNodes.erase(listNotTestedNodes.begin());
        if (listNotTestedNodes.empty())
            break;
        nodeCurrent = listNotTestedNodes.front();
        nodeCurrent->bVisited = true;

        for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
        {
            if (!nodeNeighbour->bVisited && !nodeNeighbour->bObstacle)
                listNotTestedNodes.push_back(nodeNeighbour);
            float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);
            if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
            {
                nodeNeighbour->parent = nodeCurrent;
                nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;
                nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
            }
        }
    }

    if (nodeEnd == nullptr)
    return path;

    sNode* p = nodeEnd;
    while (p != nullptr)
    {
        path.push_back(sf::Vector2f(p->x, p->y));
        p = p->parent; // remonte les parents
    }

    // Actuellement le chemin est de End -> Start, on inverse :
    std::reverse(path.begin(), path.end());
    return path;
}


/*
void PathFinding_AStar::Update(sf::RenderWindow& window, sf::Vector2f& position)
{
    auto mousePos = sf::Mouse::getPosition(window);
    int x = mousePos.x / nNodeSize;
    int y = mousePos.y / nNodeSize;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {

        if (x >= 0 && x < nMapWidth && y >= 0 && y < nMapHeight)
        {
                nodes[y * nMapWidth + x].bObstacle = !nodes[y * nMapWidth + x].bObstacle;



        }
    }
}
*/

void PathFinding_AStar::Draw(sf::RenderWindow& window)
{
    sf::RectangleShape rect(sf::Vector2f(nNodeSize-2, nNodeSize-2));


    for (int x = 0; x < nMapWidth; x++)
    {
        for (int y = 0; y < nMapHeight; y++)
        {
            sNode& node = nodes[y * nMapWidth + x];

            // Couleurs selon l'état du noeud
            if (&node == nodeStart)
                rect.setFillColor(sf::Color::Green); // départ
            else if (&node == nodeEnd)
                rect.setFillColor(sf::Color::Red);   // arrivée
            else if (node.bObstacle)
                rect.setFillColor(sf::Color(100, 100, 100)); // obstacle
            else if (node.bVisited)
                rect.setFillColor(sf::Color::Blue); // noeud visité
            else
                rect.setFillColor(sf::Color(50, 50, 150)); // noeud normal

            rect.setPosition(node.x - (nNodeSize - 2) / 2, node.y - (nNodeSize - 2) / 2);;
            window.draw(rect);
        }
    }

    // Dessiner le chemin du noeud de fin au départ
    if (nodeEnd != nullptr)
    {
        sNode* p = nodeEnd;
        while (p->parent != nullptr)
        {
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(p->x, p->y), sf::Color::Yellow),
                sf::Vertex(sf::Vector2f(p->parent->x, p->parent->y), sf::Color::Yellow)
            };
            window.draw(line, 2, sf::Lines);
            p = p->parent;
        }
    }
}


