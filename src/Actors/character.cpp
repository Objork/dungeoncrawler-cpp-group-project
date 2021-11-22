#include "character.hpp"

Character::Character(const std::string& filename, float xPos, float yPos)
    : Entity(filename, xPos, yPos, sf::Vector2f(3, 3))
{
    initVariables();
}

Character::~Character() { }

void Character::Update()
{
    sprite_.setPosition(xPos_, yPos_);
    if (hitpoints_ <= 0) {
        alive_ = false;
    }
    xPos_ = clamp(xPos_, 50, 1050);
    yPos_ = clamp(yPos_, 0, 550);
}

float Character::clamp(float value, float low, float high)
{
    if (value < low) {
        value = low;
    }
    if (value > high) {
        value = high;
    }
    return value;
}

void Character::initVariables()
{
    alive_ = true;
    hitpoints_ = 50;
}

bool Character::MoveLeft(float dt)
{
    xPos_ -= speed_ * dt;
    return true;
}

bool Character::MoveRight(float dt)
{
    xPos_ += speed_ * dt;
    return true;
}

bool Character::MoveDown(float dt)
{
    yPos_ += speed_ * dt;
    return true;
}

bool Character::MoveUp(float dt)
{
    yPos_ -= speed_ * dt;
    return true;
}

sf::Vector2f Character::GetSpriteCenter()
{
    auto spriteOrigin = GetPos();
    auto localSpriteBounds = sprite_.getLocalBounds();
    auto spriteScale = sprite_.getScale();
    spriteOrigin.x += 1.0f / 2 * localSpriteBounds.width * spriteScale.x;
    spriteOrigin.y += 1.0f / 2 * localSpriteBounds.height * spriteScale.y;
    return spriteOrigin;
}

void Character::TakeDamage(int value)
{
    hitpoints_ -= value;
}

bool Character::IsAlive() { return alive_; }