#include "monster.hpp"

template <typename T>
T Vector2length(const sf::Vector2<T>& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Monster::Monster(float xPos, float yPos)
    : Character("content/monster1.png", xPos, yPos)
{
}

void Monster::Move(float dt)
{
    int dir = rand() % 4 + 1;
    if (dir == 1) {
        this->MoveDown(dt);
    } else if (dir == 2) {
        this->MoveLeft(dt);
    } else if (dir == 3) {
        this->MoveUp(dt);
    } else
        this->MoveRight(dt);
}