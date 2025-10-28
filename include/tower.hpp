    #ifndef TOWER_HPP
    #define TOWER_HPP

    #include <SFML/Graphics.hpp>
    #include <vector>

    #include "enemy.hpp" 

    // Déclarations anticipées (évite les inclusions circulaires dans le .hpp)
    class Enemy;
    class Projectile;
    class FlyEnemy; 

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
        Tourelle(int damage,
                float range,
                float fireRate,
                float projectileSpeed,
                float radius,
                sf::Color color);
        virtual ~Tourelle();

        // Position / rendu
        void setPosition(float x, float y);
        sf::Vector2f getPosition() const;
        float        getRadius()   const { return shape.getRadius(); }
        virtual void draw(sf::RenderWindow& win) const;

        // Paramètres gameplay
        int   getDamage() const { return damage; }
        void  setDamage(int d)  { damage = d;   }

        float getRange()   const { return range; }
        void  setRange(float r)  { range = r;    }

        float getFireRate() const { return fireRate; }
        void  setFireRate(float rps) { fireRate = rps; }

        float getProjectileSpeed() const { return projectileSpeed; }
        void  setProjectileSpeed(float s) { projectileSpeed = s; }

        // Mise à jour (diminue le cooldown)
        virtual void update(float dt);

        /**
         * Essaie de tirer sur une cible dans la portée.
         * @return true si un projectile a été créé.
         */
        virtual bool tryShoot(float dt,
                    const std::vector<Enemy*>& enemies,
                    std::vector<Projectile>& outProjectiles);

    protected:
        // Accès pour les classes dérivées
        sf::CircleShape shape;

        int   damage;
        float range;
        float fireRate;
        float cooldown;
        float projectileSpeed;

        // Sélectionne l'ennemi le plus proche dans la portée (nullptr si aucun)
        virtual Enemy* acquireTarget(const std::vector<Enemy*>& enemies) const;

        using EnemyMask = unsigned int;

        static constexpr EnemyMask kindBit(EnemyKind k) {
            return 1u << static_cast<unsigned int>(k);
        }

        EnemyMask allowedMask = ~0u;  // par défaut : autorise tout

        // Helpers pour configurer le masque
        void allowOnly(EnemyMask m)         { allowedMask = m; }
        void allowAlso(EnemyKind k)         { allowedMask |= kindBit(k); }
        void forbid(EnemyKind k)            { allowedMask &= ~kindBit(k); }

        // Test d'acceptation
        bool accepts(const Enemy* e) const {
            EnemyKind k = e->getKind();
            return (allowedMask & kindBit(k)) != 0;
        }

};



// --------------------- Exemples de variantes ---------------------

    class BasicTourelle : public Tourelle {
    public:
        // portée moyenne, cadence standard, dégâts moyens
        explicit BasicTourelle(float radius = 14.f);
    };


    class PoisonTourelle : public Tourelle {
    public:
        explicit PoisonTourelle(float radius = 12.f);
    };

    class shotgunTourelle : public Tourelle {
    public:
        explicit shotgunTourelle(float radius = 14.f);
        bool tryShoot(float dt,
                    const std::vector<Enemy*>& enemies,
                    std::vector<Projectile>& outProjectiles) override;
    };

    class TargetTourelle : public Tourelle {
    public:
        explicit TargetTourelle(float radius = 14.f);
    };

    class FlyTourelle : public Tourelle {
    public:
        static int counter;
        explicit FlyTourelle(float radius = 12.f);
        ~FlyTourelle();

        Enemy* acquireTarget(const std::vector<Enemy*>& enemies) const override;

    };

    #endif // TOWER_HPP
