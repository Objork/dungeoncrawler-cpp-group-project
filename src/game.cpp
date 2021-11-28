#include "game.hpp"
#include "Collision.hpp"

namespace {
const sf::Vector2f PLACEHOLDER_PROJ_SIZE = sf::Vector2f(1.0, 1.0);
const int PLACEHOLDER_PROJ_SPEED = 1000;
const int PLACEHOLDER_PROJ_DIST = 20;
const int PLACEHOLDER_PROJ_DMG = 20;
const float PLACEHOLDER_PROJ_LS = 0.5;

}

Game::Game()
{
    player_ = new Player();
    SwordWeapon* sword = new SwordWeapon(5, 10, sf::Vector2f(50, 100), "content/sprites/projectiles.png");
    player_->Equip(sword);

    Monster* m = new RandomMonster(player_, 300, 300); // placeholder
    Monster* m2 = new SearchingMonster(player_, 200, 200);
    monsters_.push_back(m);
    monsters_.push_back(m2);

    gamebar_ = Gamebar(player_);
    initVariables();
    initWindow();
}

Game::~Game()
{
    delete window_;
    delete player_;
    for (auto monster : monsters_) {
        delete monster;
    }
}

void Game::UpdateGame()
{
    Events();

    updateDt();
    manageInput();

    // Update projectiles
    updateProjectiles();
    for (auto monster : monsters_) {
        monster->Update(dt);
    }
    // checkCollisions(player_, Projectile::Type::EnemyProjectile);
    checkCollisions(monsters_, Projectile::Type::PlayerProjectile);
    checkWallCollisions();
    player_->Update(dt);
    gamebar_.Update();
}
// render game frames
void Game::RenderGame()
{
    window_->clear();
    room.Render(window_);
    player_->Render(window_);
    gamebar_.Render(window_);
    for (auto projectile : projectiles_) {
        projectile->Render(window_);
    }
    for (auto monster : monsters_) {
        monster->Render(window_);
    }
    window_->display();
}
// Keeps the game running when window is open
bool Game::Running() const { return window_->isOpen(); }

void Game::Events()
{
    while (window_->pollEvent(event_)) {
        switch (event_.type) {
        case sf::Event::Closed:
            window_->close();
            break;
        case sf::Event::LostFocus:
            paused = true;
            break;
        case sf::Event::GainedFocus:
            paused = false;
            break;
        case sf::Event::KeyPressed:
            if (event_.key.code == sf::Keyboard::Space) {
                Monster* m = new RandomMonster(player_, player_->GetPos().x, player_->GetPos().y);
                monsters_.push_back(m);
                /* sf::Vector2f direction = sf::Vector2f(1, 0);
                Projectile* p = new Projectile(50, 50);
                p->SetDirection(direction);
                p->SetType(Projectile::Type::EnemyProjectile);
                projectiles_.push_back(p); */
            }
            break;
        case sf::Event::MouseButtonPressed:
            if (event_.mouseButton.button == sf::Mouse::Button::Left) {
                auto mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window_));
                player_->Attack(mousePos, projectiles_);
            }
            break;
        default:
            break;
        }
    }
}

void Game::initVariables() { gameEnder_ = false; }
// initalize window
void Game::initWindow()
{
    videomode_ = sf::VideoMode(1280, 768);
    window_ = new sf::RenderWindow(videomode_, "Dungeon Crawler");
}

void Game::updateDt() { dt = dtClock.restart().asSeconds(); }

void Game::manageInput()
{
    bool W = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool A = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool S = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    bool D = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

    bool twoKeys = ((W || S) && (A || D));

    if (twoKeys) {
        if (A) {
            player_->MoveLeft(dt / sqrt(2));
        } else if (D) {
            player_->MoveRight(dt / sqrt(2));
        }

        if (W) {
            player_->MoveUp(dt / sqrt(2));
        } else if (S) {
            player_->MoveDown(dt / sqrt(2));
        }
    } else {
        if (A) {
            player_->MoveLeft(dt);
        } else if (D) {
            player_->MoveRight(dt);
        }

        if (W) {
            player_->MoveUp(dt);
        } else if (S) {
            player_->MoveDown(dt);
        }
    }
}

void Game::checkCollisions(std::list<Character*> characters, Projectile::Type projectileType)
{

    if (projectiles_.empty()) {
        return;
    }

    std::vector<Character*> monsterListToDelete;

    for (auto character : characters) {
        for (auto projectile : projectiles_) {
            if (projectile->GetType() == projectileType) {
                if (Collision::PixelPerfectTest(projectile->GetSprite(), character->GetSprite())) {
                    character->TakeDamage(projectile->GetDamage());
                    if (character->IsAlive() == false) {
                        monsterListToDelete.push_back(character);
                    }
                    if (!projectile->Penetrates()) {
                        projectile->Kill();
                    }
                }
            }
        }
    }
    // Delete dead Monsters
    for (auto monster : monsterListToDelete) {
        deleteMonster(monster);
    }
}

void Game::checkWallCollisions()
{
    if (projectiles_.empty()) {
        return;
    }

    std::vector<Projectile*> projectileListToDelete;

    for (auto row : room.tileVector_) {
        for (auto tile : row) {
            if (tile->isWalkable == false) {
                for (auto projectile : projectiles_) {
                    if (projectile->GetSprite().getGlobalBounds().intersects(tile->tileSprite.getGlobalBounds())) {
                        projectile->Kill();
                    }
                }
            }
        }
    }
}
// redundant atm
void Game::deleteProjectile(Projectile* p)
{
    if (projectiles_.empty())
        return;

    for (auto it = projectiles_.begin(); it != projectiles_.end(); ++it) {
        if (*it == p) {
            projectiles_.erase(it);
            return;
        }
    }
}

void Game::deleteMonster(Character* m)
{
    if (monsters_.empty())
        return;

    for (auto it = monsters_.begin(); it != monsters_.end(); ++it) {
        if (*it == m) {
            monsters_.erase(it);
            return;
        }
    }
}

void Game::updateProjectiles()
{
    if (projectiles_.empty())
        return;

    for (auto it = projectiles_.begin(); it != projectiles_.end(); ++it) {
        auto p = *it;
        if (!p->IsAlive()) {
            it = projectiles_.erase(it);
        } else {
            p->Update(dt);
        }
    }
}
