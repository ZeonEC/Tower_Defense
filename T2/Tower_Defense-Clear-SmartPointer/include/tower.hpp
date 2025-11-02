#ifndef TOWER_HPP
#define TOWER_HPP

//------------------------ INCLUDE libs ----------------------//

#include <SFML/Graphics.hpp> // pour SFML (sf::Vector2f, sf::RenderWindow, sf::CircleShape, sf::Color)
#include <vector> // pour des vecteurs d'ennemis et de projectiles

//------------------------ INCLUDE prog ----------------------//

#include "enemy.hpp" // pour Enemy, FlyEnemy... et EnemyKind
#include "projectile.hpp" // Instance de Projectile dans tryShoot



// ====================================== ROLE DE LA CLASSE =========================================== //

// La classe Tourelle représente une tourelle de défense capable de détecter des ennemis, de tirer des projectiles et de gérer ses propres caractéristiques de combat.
// Les classes dérivées (BasicTourelle, PoisonTourelle, etc.) spécialisent la tourelle en modifiant ses statistiques et ses comportements particuliers.

// ==================================================================================================== //


// Déclarations anticipées (évite les inclusions circulaires dans les .hpp)
// Permet de dire au compilateur que ces classes existent quelque part, permet de le définir en type valide avant leur définition complète.
class Enemy;
class Projectile;
class FlyEnemy; 



//------------------------ MAIN CLASS DECLARATION ----------------------//

// Structure de base d'une tourelle :

    /**
    * Classe de base d'une tourelle.
    * - damage        : dégâts par projectile
    * - range         : portée en pixels
    * - fireRate      : tirs par seconde
    * - cooldown      : temps restant avant prochain tir (s)
    * - projectileSpeed : vitesse (px/s) des projectiles tirés
    * - shape         : représentation dans SFML (des rectangles pour l'instant)
    */

class Tourelle {
    
    public:

    // Constructeur
        Tourelle(int damage,
                float range,
                float fireRate,
                float projectileSpeed,
                float cellsize,
                sf::Color color,
                int cost);
    
    // Destructeur
        virtual ~Tourelle();

    // Gestion de la position des tourelles (dans la fenêtre SFML) Setter -- Getter
        void setPosition(float x, float y);
        sf::Vector2f getPosition() const;

    // Rendu fenêtre ( Récupération de la taille et dessin dans la fenêtre )
        float getRadius() const { return shape.getRadius(); }
        virtual void draw(sf::RenderTarget& win) const;

        // ------------- GAMEPLAY ------------- //
        
        // Les setters sont accesoires, un type d'ennemies a une range bien précise, on ne fait pas d'exception ( Piste d'amelioration si on veut upgrade des tourelles )

    // Getter -- Setter des dégats ( toutes les tourelles ont le même comportements vis à vis des dégats)
        int   getDamage() const { return damage; }
        void  setDamage(int d)  { damage = d;   }

    // Getter -- Setter de la range (En fonction du type de tourelle, on récupéreras des valeurs différentes )
        float getRange()   const { return range; }
        void  setRange(float r)  { range = r;    }

    // Getter -- Setter de la cadence de tir
        float getFireRate() const { return fireRate; }
        void  setFireRate(float rps) { fireRate = rps; }

    // Getter -- Setter de la vitesse de projectile  
        float getProjectileSpeed() const { return projectileSpeed; }
        void  setProjectileSpeed(float s) { projectileSpeed = s; }

    // Permet de gérer la 'recharge' des tourelles (ex : fireRate = 2.0f; // 2 tirs par seconde = un tir toutes les 0.5s) (pour pas que ça mitraille a chaque tic) 
        virtual void update(float dt);

    // Tentative de tir sur un ennemie en fonction du cooldown ( selectionne un ennemie, genère un projectile, rénitialise le temps de recharge )
    // Il est en virtual car chaque tourelle peut tiré différement ( typiquement le shotgun qui tire en spray ) return TRUE si un projectile a été créé.
        virtual bool tryShoot(
            float dt, // Important pour le cooldown
            const std::vector<Enemy*>& enemies, // Evidement besoin des tableaux enemy et projectil pour stocker
            std::vector<Projectile>& outProjectiles);

        int getCost() const { return cost; }

    
    protected:

        // ------------- PARTAGE AUX CLASSES HERITIERES ------------- //

    // Accès pour les classes dérivées
        sf::CircleShape shape;
        int   damage;
        float range;
        float fireRate;
        float cooldown;
        float projectileSpeed;
        int cost;

        // ------------- FONCTIONS ------------- //

    // Sélectionne l'ennemi le plus proche dans la portée (nullptr si aucun)
    // Effectue un 'scan' sur toute sa circonférence 
        virtual Enemy* acquireTarget(const std::vector<Enemy*>& enemies) const;

// ================================================================================= //

//                   LE MASQUE DE CIBLAGE (compliqué/beaucoup de gamberge)           //

// ================================================================================= //

    // On donne un nouveau nom de type de variable ( En gros écrire EnemyMask c'est comme écrire unsigned int, au moins on sait de quoi on parle)
    // L'idée derrière c'est que ça permet direct de capté pourquoi on vas se servir d'une nouvelle variable (au club des EnemyMask ici lol)
        using EnemyMask = unsigned int;

    // Ici on utilise le masque (on se base sur ce qu'on a mis dans le header de enemy.hpp ( enum class EnemyKind { Basic = 0, Fast, Tank, Target, Fly }; ))
    // On a donc Basic = 0, Fast = 2, Tank = 3 ....
    // Sa s'appele un bitMask et ça permet de stocker plusieurs choix booléens dans un seul entier, grâce aux bits.
    // Cette fonction convertit un type d’ennemi en bit correspondant.

/*  ===========================================================
    SYSTÈME DE MASQUE D'ENNEMIS (EnemyMask)
    -----------------------------------------------------------
    Chaque type d'ennemi correspond à un bit unique dans un entier
    non signé (unsigned int). Cela permet de gérer les autorisations
    des tourelles sous forme de "masque binaire".

    Association des bits :
    -----------------------------------------------------------------------------
    |  EnemyKind | Valeur entière | 1u << k  | Valeur binaire | Valeur décimale |
    |------------|----------------|----------|----------------|-----------------|
    | Basic      | 0              | 1u << 0  | 00001          | 1               |
    | Fast       | 1              | 1u << 1  | 00010          | 2               |
    | Tank       | 2              | 1u << 2  | 00100          | 4               |
    | Target     | 3              | 1u << 3  | 01000          | 8               |
    | Fly        | 4              | 1u << 4  | 10000          | 16              |
    -----------------------------------------------------------------------------

    Exemple :
    ----------
    Un masque "allowedMask" de valeur binaire 01101 signifie :
    → J'autorise Basic, Tank et Fly
    → mais pas Fast ni Target.

    Ainsi :
    - kindBit(EnemyKind::Basic)  → 1
    - kindBit(EnemyKind::Tank)   → 4
    - kindBit(EnemyKind::Fly)    → 16

    Formules utiles :
    -----------------
    kindBit(k)  : renvoie le bit associé à un type d'ennemi
    allowAlso(k): active un bit (ajoute un type autorisé)
    forbid(k)   : désactive un bit (retire un type autorisé)
    accepts(e)  : renvoie true si le type de l'ennemi est autorisé
    =========================================================== */


    // On vient récupéré la valeur de l'enum ( Basic - 0, Fast - 1..)
    // static_cast<unsigned int>(k) vient récupérer notre valeur pour la remettre en valeur entier (role de static_cast en gros hein)
    // Ensuite on fait un décalage du bit a gauche 

    // Parce que ça permet de stocker plusieurs autorisations dans un seul entier.
        static constexpr EnemyMask kindBit(EnemyKind k) {
            return 1u << static_cast<unsigned int>(k);
        }

    // Ici on met tous les bit du allowed mask à 1 (~0u = tous les bits à 1) docn ça autorise toutes les cibles a se faire tirer dessu par la tourelle désigné
        EnemyMask allowedMask = ~0u;  // par défaut : autorise tout

            // Helpers pour configurer le masque

        //allowOnly est un petit raccourci pour dire que tu autorise uniquement un type d'ennemie (mieux que de devoir tt forbid a chaque fois)
            void allowOnly(EnemyMask m)         { allowedMask = m; }

        // On autorise aussi un enemy kind (mask en ou pour faire comme si juste on allumais, mask en & = pas bon) ON se servira rarement de cette fonction 
            void allowAlso(EnemyKind k)         { allowedMask |= kindBit(k); }

        // Retire une autorisation de tir (et la mask en et parce que si on met un 0 alors ça marche plus)
            void forbid(EnemyKind k)            { allowedMask &= ~kindBit(k); }

            // Test d'acceptation
        // La tourelle trouve un ennemie, il faut maintenant vérifié si elle peut tirer sur cette ennemie, elle récupère donc le bit de l'ennemie ciblé
        // Vérifie si le type d'ennemi passé en paramètre (e) fait partie des types autorisés par la tourelle.
        // 1. On récupère le type d'ennemi : e->getKind() 2. On le convertit en bit unique via kindBit(k) 3. On fait un "ET binaire" entre allowedMask et ce bit
        // Si le résultat ≠ 0 → l'ennemi est autorisé 
        // Si le résultat == 0 → l'ennemi est interdit

            bool accepts(const Enemy* e) const {
                EnemyKind k = e->getKind();
                return (allowedMask & kindBit(k)) != 0;
            }

// ================================================================================= //
// ================================================================================= //
};



    // --------------------- Variantes ---------------------

// Par rapport aux explications plus haut rien ne change, ici on définie simplement des tourelles, faudra faire gaffe de bien changer le mask(enemy.hpp), le spawn(gamo.hpp) 

    // ================================ BASIC TOURELLLE ================================ //           

    // portée moyenne, cadence standard, dégâts moyens
        class BasicTourelle : public Tourelle {
        public:
            static int counter;
            explicit BasicTourelle(float cellsize) : Tourelle(
            /*damage*/          6,
            /*range*/           220.f,
            /*fireRate*/        2.f,   // 1 tir / s
            /*projectileSpeed*/ 450.f,
            /*radius*/          cellsize,
            /*color*/           sf::Color(60, 180, 255),
            /*cost*/            10
        ) 
            {
            // Autoriser tout SAUF les volants :
            forbid(EnemyKind::Fly);
            }

            ~BasicTourelle() { --counter; }
            
        };

    // ================================ POISON TOURELLLE ================================ //

    // Poison : dégâts faibles + cadence correcte + portée moyenne
        class PoisonTourelle : public Tourelle {
        public:
        static int counter;
            explicit PoisonTourelle(float cellsize) : Tourelle(
            /*damage*/          2,
            /*range*/           200.f,
            /*fireRate*/        1.2f,
            /*projectileSpeed*/ 420.f,
            /*radius*/          cellsize,
            /*color*/           sf::Color(150, 255, 150),
            /*cost*/            30
        ) 
            {
                // Autoriser tout SAUF les volants :
                forbid(EnemyKind::Fly);
            }

            ~PoisonTourelle() { --counter; }
        };

    // ================================ SHOTGUN TOURELLLE ================================ //

    // Shotgun : cadence élevée, dégâts faibles, faible portée
        class shotgunTourelle : public Tourelle {
        public:
        static int counter;
            explicit shotgunTourelle(float cellsize) : Tourelle(
            /*damage*/          3,
            /*range*/           160.f,
            /*fireRate*/        1.f,
            /*projectileSpeed*/ 680.f,
            /*radius*/          cellsize,
            /*color*/           sf::Color(255, 255, 255),
            /*cost*/            40
        ) 
            {
                // Autoriser tout SAUF les volants :
                forbid(EnemyKind::Fly);
            }
            bool tryShoot(float dt, const std::vector<Enemy*>& enemies, std::vector<Projectile>& outProjectiles) override;

            ~shotgunTourelle() { --counter; }
        };

    // ================================ FLY TOURELLLE ================================ //

        class FlyTourelle : public Tourelle {
        public:
            static int counter;
            explicit FlyTourelle(float cellsize) : Tourelle(
            /*damage*/          100,
            /*range*/           220.f,
            /*fireRate*/        1.5f,
            /*projectileSpeed*/ 520.f,
            /*radius*/          cellsize,
            /*color*/           sf::Color(200, 0, 0),
            /*cost*/            40
        ) 
            { 
                forbid(EnemyKind::Basic);
                forbid(EnemyKind::Fast);
                forbid(EnemyKind::Tank);
                forbid(EnemyKind::Target);
                ++counter; 
            }
            ~FlyTourelle() { --counter; }

            //Enemy* acquireTarget(const std::vector<Enemy*>& enemies) const override;

        };


    // ================================ TARGET TOURELLLE ================================ //

        class TargetTourelle : public Tourelle {
        public:
        static int counter;
            explicit TargetTourelle(float cellsize) : Tourelle(
            /*damage*/          100,
            /*range*/           240.f,
            /*fireRate*/        5.0f,
            /*projectileSpeed*/ 450.f,
            /*radius*/          cellsize,
            /*color*/           sf::Color(255, 200, 140),
            /*cost*/            50

        ) 
            {
                // Autoriser tout SAUF les volants :
                forbid(EnemyKind::Fly);
            }
            ~TargetTourelle() { --counter; }
        };

#endif // TOWER_HPP