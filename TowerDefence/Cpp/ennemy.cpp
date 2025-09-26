#include "ennemy.hpp"
#include "Astar.hpp"




Ennemy::Ennemy( std::string name,int numEnnemies,int HitPoint,int Shell,int Defence,int attack_damage,int attack_speed,int attack_range,int move_speed, PathFinding_AStar& pathfinder){
    m_numEnnemies = numEnnemies;
    groupe_ennemies = new ennemy*[numEnnemies];
    for(int i = 0; i < numEnnemies; i++){
        groupe_ennemies[i] = new ennemy;
        groupe_ennemies[i]->name = name;
        groupe_ennemies[i]->HitPoint = HitPoint;
        groupe_ennemies[i]->Shell = Shell;
        groupe_ennemies[i]->Defence = Defence;
        groupe_ennemies[i]->attack_damage = attack_damage;
        groupe_ennemies[i]->attack_speed = attack_speed;
        groupe_ennemies[i]->attack_range = attack_range;
        groupe_ennemies[i]->move_speed = move_speed;
        groupe_ennemies[i]->shape = CreateShape();
        groupe_ennemies[i]->position = Spawn_Position();
        groupe_ennemies[i]->shape.setPosition(groupe_ennemies[i]->position);
        //groupe_ennemies[i]->pathPoints = pathfinder.SolveAStar(groupe_ennemies[i]->position);
    }

}

sf::RectangleShape Ennemy::CreateShape(){
    sf::RectangleShape shape;
    shape.setSize(sf::Vector2f(40.f,40.f));
    shape.setFillColor(sf::Color(150,50,250));
    shape.setOrigin(sf::Vector2f(20.f,20.f));
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color::Black);
    return shape;
}

sf::Vector2f Ennemy::Spawn_Position(){
    sf::Vector2f position;
    position = sf::Vector2f(rand() % 100, rand() % 100);

    return position;
}

void Ennemy::move(PathFinding_AStar& pathfinder) {
    for (int i = 0; i < m_numEnnemies; i++) {
        auto& enemy = groupe_ennemies[i];

        // Calcul du chemin uniquement si nécessaire
        if (enemy->pathPoints.empty() || enemy->currentPathIndex >= enemy->pathPoints.size()) {
            enemy->pathPoints = pathfinder.SolveAStar(enemy->position);
            enemy->currentPathIndex = 0;
        }

        // Suivre le chemin
        if (enemy->currentPathIndex < enemy->pathPoints.size()) {
            sf::Vector2f target = enemy->pathPoints[enemy->currentPathIndex];
            sf::Vector2f dir = target - enemy->position;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist <= enemy->move_speed) {
                // Si proche, snap à la position exacte du centre du nœud
                enemy->position = target;
                enemy->shape.setPosition(enemy->position);
                enemy->currentPathIndex++;
            } else {
                // Sinon, avancer vers la cible
                dir /= dist;
                enemy->position += dir * (float)enemy->move_speed;
                enemy->shape.setPosition(enemy->position);
            }
        }
    }
}



void Ennemy::draw(sf::RenderWindow& rendertexture){
    for(int i = 0; i < m_numEnnemies; i++){
        rendertexture.draw(groupe_ennemies[i]->shape);
    }
    
}

Ennemy::~Ennemy() {
    for (int i = 0; i< m_numEnnemies; i++) {
        delete groupe_ennemies[i];
    }
    delete[] groupe_ennemies;
}

