#include "Astar.hpp"
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include "enemy.hpp"


void PathFinding_AStar::CreateNodes(const std::vector<Render::Cell>& cells)
{
    if (cells.empty()) return;

    NodeSize = cells[0].cellSize;

    // Déterminer la taille de la grille
    int maxRow = 0;
    int maxCol = 0;
    for (const auto& c : cells) {
        if (c.row > maxRow) maxRow = c.row;
        if (c.col > maxCol) maxCol = c.col;
    }
    gridRows = maxRow + 1;
    gridCols = maxCol + 1;

    Nodes.clear();
    Nodes.reserve(cells.size());

    // Création des nodes
    for (const auto& c : cells) {
        Node node;
        node.x = c.center.x;
        node.y = c.center.y;
        node.row = c.row;
        node.col = c.col;
        node.turreted = c.turreted;
        node.bVisited = false;
        node.EnemyPresence = false;
        node.fGlobalGoal = std::numeric_limits<float>::infinity();
        node.fLocalGoal = std::numeric_limits<float>::infinity();
        node.parent = nullptr;
        node.Neighbours.clear();
        Nodes.push_back(node);
    }

    // Détermination des voisins (8 directions)
    for (size_t i = 0; i < Nodes.size(); ++i) {
        for (size_t j = 0; j < Nodes.size(); ++j) {
            if (i == j) continue;
            if (std::abs(Nodes[j].row - Nodes[i].row) <= 1 &&
                std::abs(Nodes[j].col - Nodes[i].col) <= 1) {
                Nodes[i].Neighbours.push_back(&Nodes[j]);
            }
        }
    }

    // Détermination des noeuds de fin (bord droit)
    nodeEnd.clear();
    for (auto& n : Nodes) {
        if (n.col >= gridCols - 1)
            nodeEnd.push_back(&n);
    }
}


PathFinding_AStar::Node* PathFinding_AStar::findClosestNode(const sf::Vector2f& position)
{
    if (Nodes.empty()) return nullptr;

    Node* closest = nullptr;
    float bestDist = std::numeric_limits<float>::infinity();

    for (auto& n : Nodes) {
        float dx = n.x - position.x;
        float dy = n.y - position.y;
        float d = dx * dx + dy * dy;
        if (d < bestDist) {
            bestDist = d;
            closest = &n;
        }
    }
    return closest;
}

std::vector<sf::Vector2f> PathFinding_AStar::SolveAStar(const sf::Vector2f& startPos)
{
    std::vector<sf::Vector2f> emptyPath;
    paths.clear();
    path.clear();

    if (Nodes.empty() || nodeEnd.empty() || NodeSize <= 0.f)
        return emptyPath;

    // Trouve le nœud de départ le plus proche
    Node* startNode = findClosestNode(startPos);
    if (!startNode) {
        std::cerr << "[AStar] startNode introuvable pour (" << startPos.x << "," << startPos.y << ")\n";
        return emptyPath;
    }

    auto heuristic_local = [&](Node* a, Node* b) -> float {
        if (!a || !b) return std::numeric_limits<float>::infinity();
        float dx = a->x - b->x;
        float dy = a->y - b->y;
        return std::sqrt(dx * dx + dy * dy);
    };

    // Pour chaque nodeEnd possible, on calcule un chemin
    for (Node* targetEnd : nodeEnd) {
        if (!targetEnd) continue;

        // Reset des nodes
        for (auto& n : Nodes) {
            n.bVisited = false;
            n.fGlobalGoal = std::numeric_limits<float>::infinity();
            n.fLocalGoal = std::numeric_limits<float>::infinity();
            n.parent = nullptr;
        }

        Node* nodeCurrent = startNode;
        startNode->fLocalGoal = 0.0f;
        startNode->fGlobalGoal = heuristic_local(startNode, targetEnd);

        std::vector<Node*> listNotTestedNodes;
        listNotTestedNodes.push_back(startNode);

        while (!listNotTestedNodes.empty() && nodeCurrent != targetEnd) {
            std::sort(listNotTestedNodes.begin(), listNotTestedNodes.end(),
                [](Node* lhs, Node* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });

            while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
                listNotTestedNodes.erase(listNotTestedNodes.begin());
            if (listNotTestedNodes.empty()) break;

            nodeCurrent = listNotTestedNodes.front();
            nodeCurrent->bVisited = true;

            for (Node* neighbour : nodeCurrent->Neighbours) {
                if (!neighbour || neighbour->bVisited || neighbour->turreted)
                    continue;

                
                float penalty = neighbour->EnemyPresence ? 5.0f : 1.0f;
                float stepCost = ((neighbour->row != nodeCurrent->row && neighbour->col != nodeCurrent->col)
                                  ? 1.414f : 1.0f) * penalty;

                float possibleLowerGoal = nodeCurrent->fLocalGoal + stepCost;

                if (possibleLowerGoal < neighbour->fLocalGoal) {
                    neighbour->parent = nodeCurrent;
                    neighbour->fLocalGoal = possibleLowerGoal;
                    neighbour->fGlobalGoal = neighbour->fLocalGoal + heuristic_local(neighbour, targetEnd);
                    listNotTestedNodes.push_back(neighbour);
                }
            }
        }

        if (targetEnd->parent == nullptr && targetEnd != startNode)
            continue;

        // Reconstruction du chemin
        path.clear();
        Node* p = targetEnd;
        while (p) {
            path.push_back(sf::Vector2f(p->x, p->y));
            p = p->parent;
        }
        std::reverse(path.begin(), path.end());

        if (!path.empty())
            paths.push_back(path);
    }

    if (paths.empty())
        return emptyPath;

    // Choisir le chemin le plus court
    auto pathCost = [](const std::vector<sf::Vector2f>& p) -> float {
        if (p.size() < 2) return 0.f;
        float cost = 0.f;
        for (size_t i = 1; i < p.size(); ++i) {
            float dx = p[i].x - p[i - 1].x;
            float dy = p[i].y - p[i - 1].y;
            cost += std::sqrt(dx * dx + dy * dy);
        }
        return cost;
    };

    size_t bestIndex = 0;
    float bestCost = pathCost(paths[0]);
    for (size_t i = 1; i < paths.size(); ++i) {
        float c = pathCost(paths[i]);
        if (c < bestCost) {
            bestCost = c;
            bestIndex = i;
        }
    }

    std::vector<sf::Vector2f> finalPath = paths[bestIndex];
    // Insère la position réelle du départ (pas celle du node)
    finalPath.insert(finalPath.begin(), startPos);

    return finalPath;
}


bool PathFinding_AStar::IsPathStillPossible(const sf::Vector2f& startPos)
{
    if (Nodes.empty() || nodeEnd.empty()) return false;

    Node* startNode = findClosestNode(startPos);
    if (!startNode) return false;

    for (auto& n : Nodes) {
        n.bVisited = false;
        n.parent = nullptr;
    }

    std::vector<Node*> open;
    open.push_back(startNode);

    while (!open.empty()) {
        Node* current = open.back();
        open.pop_back();
        current->bVisited = true;

        if (std::find(nodeEnd.begin(), nodeEnd.end(), current) != nodeEnd.end())
            return true;

        for (auto* n : current->Neighbours) {
            if (!n || n->bVisited || n->turreted) continue;
            open.push_back(n);
        }
    }

    return false;
}


void PathFinding_AStar::updateEnemyPresence(const std::vector<Enemy*>& enemies)
{
    for (auto& node : Nodes)
        node.EnemyPresence = false;

    // Influence continue : chaque ennemi affecte les nodes autour de lui
    for (auto* e : enemies) {
        if (!e) continue;
        sf::Vector2f pos = e->getPosition();

        for (auto& n : Nodes) {
            float dx = n.x - pos.x;
            float dy = n.y - pos.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < NodeSize * 0.8f) {
                n.EnemyPresence = true;
            }
        }
    }
}

