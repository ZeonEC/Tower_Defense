//------------------------ INCLUDE libs ----------------------//
#include <iostream>

//------------------------ INCLUDE prog ----------------------//
#include "enemy.hpp"
#include "tower.hpp" 
#include "Astar.hpp" 

// On crée un petit compteur de chaque ennemies comme on a vu en cours, au cas ou. (Ça sert a rien pour l'instant)
int Enemy::counter      = 0;
int BasicEnemy::counter = 0;
int FastEnemy::counter  = 0;
int TankEnemy::counter  = 0;
int TargetEnemy::counter= 0;
int FlyEnemy::counter   = 0;


// ============================================================================
//                           CLASSE DE BASE : Enemy
// ============================================================================
//
// Représente un ennemi générique pour le TD :
//
//  - hp            : points de vie
//  - speed         : vitesse de déplacement (utilisée comme "step" par frame)
//  - radius        : rayon visuel/collision (taille sur la grille)
//  - texturePath   : chemin vers la texture SFML
//  - ressourceValue: nombre de ressources données au joueur à sa mort
//
// Les classes dérivées (BasicEnemy, FastEnemy, etc.) ne font que fixer ces
// valeurs de base et éventuellement override le comportement (tir, pathfinding…)
// ============================================================================ //

Enemy::Enemy(int hp, float spd, float radius, const std::string& texturePath, int ressourceValue)
: health(hp), speed(spd), dead(false), eat(false), ressourceValue(ressourceValue)
{
    // ------------------- Chargement de la texture ------------------- //
    if (!texture.loadFromFile(texturePath)) {
        std::cerr << "Erreur : impossible de charger la texture : " << texturePath << "\n";

        // Fallback minimal : texture 1x1 blanche pour éviter un sprite vide
        sf::Image img;
        img.create(1, 1, sf::Color::White);
        texture.loadFromImage(img);
    }
    sprite.setTexture(texture);

    // ------------------- Centrage de l'origine ------------------- //
    // On place l'origine du sprite au centre, pour simplifier :
    //   - les rotations
    //   - le positionnement sur la grille
    const auto local = sprite.getLocalBounds(); // taille en pixels de la texture
    sprite.setOrigin(local.width * 0.5f, local.height * 0.5f);

    // ------------------- Mise à l'échelle du sprite ------------------- //
    // On veut que le sprite ait un rayon visuel = radius (donc diamètre = 2*radius).
    const float targetDiameter = radius * 2.f;

    // On évite une division par 0 si jamais la texture est très petite ou foireuse.
    float sx = targetDiameter / std::max(1.f, local.width);
    float sy = targetDiameter / std::max(1.f, local.height);
    sprite.setScale(sx, sy);

    // Incrément du compteur global
    ++counter;
}

// ----------------------------------------------------------
// Gestion des points de vie / affichage
// ----------------------------------------------------------

// Applique des dégâts à l’ennemi
void Enemy::hit(int amount) {
    health -= amount;
    if (health < 0) health = 0;
    if (health == 0) dead = true;
}

// Dessin de l’ennemi (si mort on ne dessine plus)
void Enemy::draw(sf::RenderTarget& win) const {
    if (!dead) win.draw(sprite);
}


// ----------------------------------------------------------
// Update  avec Pathfinding A* (ennemis “classiques”)
// ----------------------------------------------------------
//
// - On utilise le pathfinder A* pour récupérer une liste de points (pathPoints)
//   que l’ennemi doit suivre jusqu’à goal
// - currentPathIndex pointe sur le prochain point du chemin
// - needRepath permet, côté gameplay, de forcer un recalcul (tour ajoutée, chemin bloqué, etc.)
// - reachedGoal passe à true quand l’ennemi atteint la fin du chemin (On utilise ça pou le faire disparaitre et enlever des points de vie)

void Enemy::update(PathFinding_AStar& pathfinder, const std::vector<Render::Cell>& cells) {

    // ------------------- Gestion de la mort ------------------- //

    // Si l’ennemi est déjà mort, inutile de faire le reste.
    // (on évite des calculs inutiles et des déplacements fantômes)
    if (dead) {return;}

    // ------------------- Recalcul du chemin ------------------- //

    // On redemande un chemin au pathfinder si :
    //  - on n’en a pas encore,
    //  - on est arrivé au bout du précédent,
    //  - ou qu’un "repath" a été demandé (tour placée, obstacle, etc.)
    if (pathPoints.empty() ||
        currentPathIndex >= static_cast<int>(pathPoints.size()) ||
        needRepath)
    {
        // Demande à A* de trouver un chemin depuis la position actuelle
        pathPoints = pathfinder.SolveAStar(getPosition());

        // On repart du début du nouveau chemin
        currentPathIndex = 0;

        // On réinitialise le flag (sinon boucle infinie)
        needRepath = false;
    }

    // debug au cas ou :
    // (affiche le chemin calculé dans la console)
    // for (size_t j = 0; j < pathPoints.size(); ++j)
    //     std::cout << "path[" << j << "] = (" << pathPoints[j].x << ", " << pathPoints[j].y << ")\n";


    // ------------------- Déplacement le long du chemin ------------------- //

    // Si on a encore des points à parcourir, on se déplace vers le suivant
    if (currentPathIndex < static_cast<int>(pathPoints.size())) {

        // Position actuelle de l’ennemi
        sf::Vector2f pos = getPosition();

        // Prochain point à atteindre (centre de la cellule suivante)
        sf::Vector2f target = pathPoints[currentPathIndex];

        // Direction brute entre la position actuelle et le point visé
        // DIR a (x,y) on s'en sert après pour calculer la distance entre la position de base et la target (Th PYTHAGORE) 
        sf::Vector2f dir = target - pos;
        // Distance totale jusqu’au point  (√(dx² + dy²))
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        // "step" = distance que l’ennemi peut parcourir par update()
        float step = getSpeed();

        // On libère la case précédente dans la grille du pathfinding
        if (pathfinder.EnemyNode) {
            pathfinder.EnemyNode->EnemyPresence = false;
        }

        // ------------------- Cas 1 : arrivé au point courant ------------------- //
        // Si l’ennemi est suffisamment proche du point cible (ou pile dessus),
        // on le place exactement dessus et on passe au point suivant.
        if (dist <= step || dist == 0.f) {

            // On “snap” l’ennemi pile au centre du nœud
            setPosition(target.x, target.y);

            // Passage au point suivant
            ++currentPathIndex;

        // ------------------- Cas 2 : encore en route vers le point ------------------- //
        } else {

            // Normalisation du vecteur de direction
            dir.x /= dist;
            dir.y /= dist;

            // On avance progressivement vers le point suivant
            pos.x += dir.x * step;
            pos.y += dir.y * step;

            // Mise à jour de la position réelle dans le monde
            setPosition(pos.x, pos.y);
        }
    }

    // ------------------- ÉTAPE 4 : arrivée à la fin du chemin ------------------- //
    // Si on a dépassé ou atteint le dernier point du chemin,
    // ça veut dire que l’ennemi est arrivé à goal (Bien joué patron !).
    if (currentPathIndex >= static_cast<int>(pathPoints.size())) {
        reachedGoal = true;
    }
}

// Getters simples
bool Enemy::isDead() const {
    return dead;
}

int Enemy::getHp() const {
    return health;
}

float Enemy::getSpeed() const {
    return speed;
}

// Rayon logique lié à la taille d’une cellule (ça permet de gérer en gros la hitbox)
float Enemy::getRadius() const {
    const float r = Render::Cell::cellSize * 0.5f;
    return r;
}

// Variante de perte de PV (avec rendu "mort" en gris)
void Enemy::takeDamage(int amount) {
    if (dead) return;
    health -= amount;
    if (health <= 0) {
        health = 0;
        dead = true;
        sprite.setColor(sf::Color(60, 60, 60, 180));
    }
}


// ============================================================================
//                         CLASSES DERIVEES D'ENNEMI
// ============================================================================
//
// Chacune fixe :
//   - ses PV
//   - sa vitesse
//   - son rayon (taille visuelle sur la grille)
//   - sa texture
//   - la valeur en ressources donnée au joueur à sa mort
//
// Le reste du comportement peut rester celui d’Enemy, sauf cas particuliers
// (FlyEnemy / TargetEnemy plus bas).
// ============================================================================ //


//----------------------------------------------------------
// Ennemi de base : BasicEnemy
//----------------------------------------------------------
//
// - PV moyens
// - vitesse standard
// - taille = 1 cellule
//
BasicEnemy::BasicEnemy()
    : Enemy(
        /*hp*/          100,
        /*speed*/       1,
        /*radius*/      Render::Cell::cellSize * 0.5f, // taille = cellule
        /*texturePath*/ "../src/assets/enemies/basic.png",
        /*ressources*/  1
    )
{
    ++counter;
}

BasicEnemy::~BasicEnemy() {
    --BasicEnemy::counter;
}


//----------------------------------------------------------
// Ennemi rapide : FastEnemy
//----------------------------------------------------------
//
// - moins de PV que le Basic
// - se déplace plus vite
// - plus petit visuellement (radius = cellSize / 2)
//
FastEnemy::FastEnemy()
    : Enemy(
        /*hp*/          75,
        /*speed*/       2,
        /*radius*/      Render::Cell::cellSize * 0.35f, // taille = cellule/2
        /*texturePath*/ "../src/assets/enemies/Fast.png",
        /*ressources*/  2
    )
{
    ++counter;
}

FastEnemy::~FastEnemy() {
    --FastEnemy::counter;
}


//----------------------------------------------------------
// Ennemi tank : TankEnemy
//----------------------------------------------------------
//
// - beaucoup de PV
// - très lent
// - un peu plus grand que la cellule (0.6 * cellSize)
//
TankEnemy::TankEnemy()
    : Enemy(
        /*hp*/          300,
        /*speed*/       0.5f,
        /*radius*/      Render::Cell::cellSize * 0.6f, // taille = cellule * 0.6
        /*texturePath*/ "../src/assets/enemies/Tank.png",
        /*ressources*/  5
    )
{
    ++counter;
}

TankEnemy::~TankEnemy() {
    --TankEnemy::counter;
}


//----------------------------------------------------------
// Ennemi Target : TargetEnemy
//----------------------------------------------------------
//
// Ennemis spécialisés dans la destruction des tourelles.
//
// Idée de base :
//  - ils cherchent la tourelle la plus proche,
//  - se dirigent vers elle,
//  - peuvent lui tirer dessus (projectiles vers les tours).
//
TargetEnemy::TargetEnemy()
    : Enemy(
        /*hp*/          150,
        /*speed*/       0.75f,
        /*radius*/      Render::Cell::cellSize * 0.5f,
        /*texturePath*/ "../src/assets/enemies/Target.png",
        /*ressources*/  3
    )
{
    ++counter;
}

TargetEnemy::~TargetEnemy() {
    --TargetEnemy::counter;
}


//----------------------------------------------------------
// Ennemi volant : FlyEnemy
//----------------------------------------------------------
//
// Particularité :
//  - ignore le pathfinding A* (vole au-dessus des blocs),
//  - se déplace en ligne droite vers les tourelles,
//  - peut tirer sur les tourelles (projectiles “vers tours”).
//
FlyEnemy::FlyEnemy()
    : Enemy(
        /*hp*/          100,
        /*speed*/       2,
        /*radius*/      Render::Cell::cellSize * 0.4f,
        /*texturePath*/ "../src/assets/enemies/Fly.png",
        /*ressources*/  2
    )
{
    ++counter;
}

FlyEnemy::~FlyEnemy() {
    --FlyEnemy::counter;
}


// ============================================================================
//                      FlyEnemy : ciblage / tir sur tourelles
// ============================================================================
//
// - towers : pointeur sur la liste des tourelles (fourni par le Game)
// - target : tourelle actuellement suivie
// - shootRange : distance à partir de laquelle on peut tirer
// - fireCooldown / fireRate : gestion cadence de tir
//
// update :
//   1) choisit la tourelle la plus proche (acquireTarget)
//   2) se déplace vers elle (moveTowards)
//   3) si à portée -> tir possible (tryShoot)
// ============================================================================ //

// Fonction de ciblage
void FlyEnemy::acquireTarget()
{
    // A chaque appelle on remet la cible en nul pour éviter qu'il retourne sur une autre que la plus proche 
    target = nullptr;
    // On skip la fonction si jamais on a pas de tourelle
    if (!towers) return;

    // On définie une distance de comparaison infinie au début 
    float bestD2 = std::numeric_limits<float>::max();
    const sf::Vector2f pos = getPosition();

    // On parcourt toutes les tourelles pour trouver la plus proche (en distance²)
    for (auto* t : *towers) {
        if (!t || t->isDestroyed()) continue; // Si l'état de la tourelle est morte/pas la alors pas besoin de faire le reste.
        sf::Vector2f d = t->getPosition() - pos;
        float d2 = d.x*d.x + d.y*d.y;
        if (d2 < bestD2) {
            bestD2 = d2;
            target = t; // Si l'autre qu'il a trouvé est meilleur que la précédente alors go celle ci 
        }
    }
}

// Déplacement simple : on va tout droit vers "dest" avec un pas "step"
void FlyEnemy::moveTowards(const sf::Vector2f& dest, float step)
{
    sf::Vector2f p = getPosition();
    sf::Vector2f v = dest - p;
    const float L = std::sqrt(v.x*v.x + v.y*v.y);
    if (L < 1e-4f) return;     // évite une division par 0

    v.x /= L;
    v.y /= L;
    p += v * step;             // step = "distance" par frame (speed)
    setPosition(p.x, p.y);
}

void FlyEnemy::update(PathFinding_AStar&, const std::vector<Render::Cell>& cells)
{

    if (dead || reachedGoal) return;

    const float step = getSpeed();

    // On redemande constamment si une tourelle est plus proche que la cible qu'il a
    acquireTarget();
    // Si pas de cible ou cible détruite, on essaie d’en trouver une nouvelle
    if (!target || target->isDestroyed()) acquireTarget();

    // Déplacement : vers la tourelle si on en a une, sinon vers la droite
    if (target) {
        sf::Vector2f pos  = getPosition();
        sf::Vector2f tpos = target->getPosition();
        sf::Vector2f d    = tpos - pos;
        float d2 = d.x*d.x + d.y*d.y;

        if (d2 > shootRange * shootRange) {
            // Pas encore à portée de tir -> on se rapproche
            moveTowards(tpos, step);
        } else {
            // À portée de tir -> on peut rester sur place (tir géré dans tryShoot)
        }
    } else {
        // Pas de tourelle disponible : on avance vers la droite
        moveTowards(getPosition() + sf::Vector2f(1.f, 0.f), step);
    }

    // Vérifier si on a atteint le bord droit de la map (goal)
    if (!cells.empty()) {
        int   maxCol = -1;
        float goalX  = 0.f;

        for (const auto& c : cells) {
            if (c.col > maxCol) {
                maxCol = c.col;
                goalX  = c.center.x;
            }
        }

        // Petite marge en plus du centre de la dernière cellule
        goalX += Render::Cell::cellSize * 0.5f;

        if (getPosition().x >= goalX) {
            reachedGoal = true;
        }
    }
}

// Gestion du tir de FlyEnemy vers une tourelle
bool FlyEnemy::tryShoot(float dt, std::vector<Projectile>& out)
{
    // pas de cible valide  alors on tir pas
    if (!target || target->isDestroyed()) return false;

    const sf::Vector2f pos  = getPosition(); // position actuelle de l’ennemi
    const sf::Vector2f tpos = target->getPosition(); // position de la tourelle ciblée
    // petite comparaisons de la distance 
    sf::Vector2f d = tpos - pos;
    float d2 = d.x*d.x + d.y*d.y;

    // Trop loin pour tirer
    if (d2 > shootRange*shootRange) return false;

    // Cooldown de tir
    fireCooldown -= dt;
    if (fireCooldown > 0.f) return false;

    // Création d’un projectile qui vise une TOURELLE
    out.emplace_back(
        pos, tpos,
        projSpeed,
        projDamage,
        /*radius*/      4.f, // rayon visuel du projectile
        /*allowedMask*/ 0u,  // inutile pour les tours
        Projectile::TargetType::Towers            
    );

    fireCooldown = 1.f / fireRate;
    return true;
}


// ============================================================================
//                      TargetEnemy : ciblage / tir sur tourelles
// ============================================================================
//
// Très proche du comportement de FlyEnemy, mais avec des stats différentes
// et une vitesse de déplacement plus faible.
//
// Il pourrait, plus tard, utiliser le A* pour contourner des obstacles
// tout en cherchant à détruire les tourelles les plus gênantes.
// ============================================================================ //

void TargetEnemy::acquireTarget()
{
    target = nullptr;
    if (!towers) return;

    float bestD2 = std::numeric_limits<float>::max();
    const sf::Vector2f pos = getPosition();

    for (auto* t : *towers) {
        if (!t || t->isDestroyed()) continue; 
        sf::Vector2f d = t->getPosition() - pos;
        float d2 = d.x*d.x + d.y*d.y;
        if (d2 < bestD2) {
            bestD2 = d2;
            target = t;
        }
    }
}

// Même principe que pour FlyEnemy : déplacement en ligne droite vers dest
void TargetEnemy::moveTowards(const sf::Vector2f& dest, float step)
{
    sf::Vector2f p = getPosition();
    sf::Vector2f v = dest - p;
    const float L = std::sqrt(v.x*v.x + v.y*v.y);
    if (L < 1e-4f) return;

    v.x /= L;
    v.y /= L;
    p += v * step;
    setPosition(p.x, p.y);
}

void TargetEnemy::update(PathFinding_AStar&, const std::vector<Render::Cell>& cells)
{
    acquireTarget();
    
    if (dead || reachedGoal) return;

    const float step = getSpeed();

    // 1) Cibler une tourelle
    if (!target || target->isDestroyed()) acquireTarget();

    // 2) Déplacement vers la tourelle, sinon avance vers la droite
    if (target) {
        sf::Vector2f pos  = getPosition();
        sf::Vector2f tpos = target->getPosition();
        sf::Vector2f d    = tpos - pos;
        float d2 = d.x*d.x + d.y*d.y;

        if (d2 > shootRange * shootRange) {
            moveTowards(tpos, step);
        } else {
            // À portée de tir : on peut s’arrêter pour tirer
        }
    } else {
        // Pas de tourelle -> comportement basique (avance vers la droite)
        moveTowards(getPosition() + sf::Vector2f(1.f, 0.f), step);
    }

    // 3) Vérifier si on a atteint le bord droit de la map
    if (!cells.empty()) {
        int   maxCol = -1;
        float goalX  = 0.f;

        for (const auto& c : cells) {
            if (c.col > maxCol) {
                maxCol = c.col;
                goalX  = c.center.x;
            }
        }

        goalX += Render::Cell::cellSize * 0.5f;

        if (getPosition().x >= goalX) {
            reachedGoal = true;
        }
    }
}

// Gestion du tir de TargetEnemy vers une tourelle
bool TargetEnemy::tryShoot(float dt, std::vector<Projectile>& out)
{
    if (!target || target->isDestroyed()) return false;

    const sf::Vector2f pos  = getPosition();
    const sf::Vector2f tpos = target->getPosition();
    sf::Vector2f d = tpos - pos;
    float d2 = d.x*d.x + d.y*d.y;

    if (d2 > shootRange*shootRange) return false;   // trop loin

    fireCooldown -= dt;
    if (fireCooldown > 0.f) return false;

    // Projectile qui cible une TOURELLE
    out.emplace_back(
        pos, tpos,
        projSpeed,
        projDamage,
        /*radius*/      4.f,
        /*allowedMask*/ 0u,
        Projectile::TargetType::Towers
    );

    fireCooldown = 1.f / fireRate;
    return true;
}

