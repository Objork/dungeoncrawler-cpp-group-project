#pragma once

#ifndef _PLAYER_CLASS_
#define _PLAYER_CLASS_

#include "Combat/Weapons/Weapon.hpp"
#include "character.hpp"
class Weapon;
class Player : public Character {
public:
    Player();
    ~Player() {};
    int GetHitPoints() const;

    void Attack(sf::Vector2f dir, std::list<Projectile*>& worldProjectiles);
    void Equip(Weapon* weapon);

private:
    Weapon* weapon_;
};

#endif