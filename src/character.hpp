#pragma once

#ifndef _CHARACTER_CLASS_
#define _CHARACTER_CLASS_

class Character {
public:
    Character(const std::string& filename, float xPos, float yPos);

    ~Character();

    void Update();

    virtual void Render(sf::RenderTarget* target);

    void initSprite(const std::string& filename);

    bool MoveLeft();
    bool MoveRight();
    bool MoveDown();
    bool MoveUp();

    void initVariables();
    sf::Vector2f GetPosition();

    int GetHitpoints() const;
    void ReduceHitpoints();

private:
    sf::Sprite sprite_;
    sf::Texture texture_;
    float xPos_;
    float yPos_;
    float speed_ = 0.2;

    int hitpoints_;
};

#endif