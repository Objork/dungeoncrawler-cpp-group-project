#pragma once

#ifndef _PLAYER_CLASS_
#define _PLAYER_CLASS_

#include "Actors/character.hpp"
#include "Combat/Health/Potion.hpp"

class Player : public Character {
public:
    Player();
    ~Player() {};

    virtual void Update(float dt);

    void Dash();
    std::list<ProjectileUP> Attack(sf::Vector2f aimPos);

    void initVariables();

    void ResetDashCooldown();

    float GetDashCooldownLeft() const { return dashCooldownLeft_; };
    float GetDashCooldownLength() const { return dashCooldownLength_; };

    bool CanDash;
    bool IsDashing;

    void AddPotion(Potion* potion);
    void UsePotion(const std::string& colour);

    std::vector<Potion*> GetInventory() const;

private:
    int attacksBoosted_;
    int dashesBoosted_;

    float dashSpeed_;
    float dashLengthBoostModifier = 2.0f;
    float dashDefaultDurationLength_;
    float dashCurrentDurationLength_;
    float dashDurationLeft_;
    bool deadAnimationPlayed = false;
    float dt_time = 0;
    float dashCooldownLength_;
    float dashCooldownLeft_;
    void updateDashCooldown(float dt);

    std::vector<Potion*> inventory_;
};
typedef std::shared_ptr<Player> PlayerPS;

#endif