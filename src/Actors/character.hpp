#pragma once

#ifndef _CHARACTER_CLASS_
#define _CHARACTER_CLASS_

#include "Combat/Projectile.hpp"
//#include "Interfaces/ICollidable.hpp"
#include "Animation/Animationhandler.hpp"
#include "Combat/Weapons/Weapon.hpp"
#include "Utility/LevelUpSystem.hpp"
#include "Utility/RandomHelper.hpp"
#include "entity.hpp"

class Character : public Entity /*, public ICollidable*/ {
public:
    Character(const std::string& filename, sf::Vector2f pos, bool animated = false);

    virtual ~Character();

    virtual void Update(float dt) = 0;

    void Equip(Weapon* weapon);

    void initVariables();
    /**
     * @brief Get the lower half of bounding box if the character was at the position given as argument
     *
     * @param    pos                  the wanted position
     * @return sf::FloatRect
     */

    void TakeDamage(int value);
    int GetHitPoints() const;

    bool IsAlive();
    bool HasWeapon();

    bool Idle();
    bool Dead();
    bool MoveLeft(float dt);
    bool MoveRight(float dt);
    bool MoveDown(float dt);
    bool MoveUp(float dt);
    // For subclasses, should be = 0
    virtual bool Move(float)
    {
        return false;
    }
    void RevertMove();

    void ResetAttackCooldown();
    float GetAttackCooldownLeft() const { return attackCooldownLeft; };
    float GetAttackCooldownLength() const { return attackCooldownLength_; };
    bool CanAttack;
    int GetMaxHP();

    void SetNormalSpeed(float value);
    void ResetCharacterToBeAlive();
    /*
    // for ICollidable
    virtual sf::FloatRect GetBoundingBox() { return sprite_.getGlobalBounds(); }
    virtual void ProcessCollision(ICollidable* object);
    virtual ICollidable::EntityType GetEntityType();
    */

protected:
    /*void GetHitBy(Projectile& projectile);*/

    Weapon* weapon_;
    Projectile::Type characterProjectileType_;

    sf::Vector2f startPos;

    int hitpoints_;
    int currentMaxHitpoints_;
    int defaultMaxHitpoints_;

    bool hasAnimation_;
    AnimationHandler animationHandler_;
    float currentSpeed_;
    float defaultSpeed_;

    void generalUpdate(float dt);

    float attackCooldownLength_;
    float attackCooldownLeft;
    void updateAttackCooldown(float dt);
    std::list<ProjectileUP> emptyList();
    std::list<ProjectileUP> shotProjectileList(sf::Vector2f aimPos);
};
#endif
