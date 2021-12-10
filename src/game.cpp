#include "game.hpp"
#include "Collision.hpp"
#define C_PIXELS 64

namespace {
const sf::Vector2u VIDEOMODE_DIMS = sf::Vector2u(1280, 768);
const std::string DEATHTEXT = "content/sprites/DEATHTEXT.png";
}

Game::Game()
    : player_(new Player())
    , dungeonMap_(Map(VIDEOMODE_DIMS, 10, *player_))
    , gamebar_(Gamebar(player_))
    , deathtext_(ScreenText(DEATHTEXT, { 0, 0 }, { 1400, 600 }))
{
    SwordWeapon* sword = new SwordWeapon(20, 10, sf::Vector2f(50, 100), 120, "content/sprites/projectiles.png");
    player_->Equip(sword);
    initVariables();
    initWindow();
    dtClock.restart(); // to not have giant first dt
}

Game::~Game()
{
    delete window_;
    delete player_;
}

void Game::UpdateGame()
{
    Events();

    updateDt();
    manageInput();

    // Update projectiles
    for (auto monster : dungeonMap_.GetCurrentRoom()->GetMonsters()) {
        // if moved, check collision with walls
        bool monsterMoved = monster->Move(dt);
        if (monsterMoved && collidesWithWall(monster)) {
            monster->RevertMove();
        }

        std::list<Projectile*> projectileListToAdd = monster->Attack();
        addProjectiles(projectileListToAdd);

        monster->Update(dt);
    }
    updateProjectiles();
    // checkCollisions(player_, Projectile::Type::EnemyProjectile);
    // handleMonsterProjectileCollisions(monsters_, Projectile::Type::PlayerProjectile);
    checkMonsterCollisions();
    checkPlayerCollisions();
    checkAndHandleProjectileWallCollisions();

    player_->Update(dt);
    gamebar_.Update();
}
// render game frames
void Game::RenderGame()
{
    window_->clear();
    dungeonMap_.RenderCurrentRoom(window_);
    player_->Render(window_);
    gamebar_.Render(window_);
    deathtext_.Render(window_);
    if (gameLost() == true) {
        deathtext_.Render(window_);
    }
    for (auto projectile : projectiles_) {
        projectile->Render(window_);
    }
    for (auto monster : dungeonMap_.GetCurrentRoom()->GetMonsters()) {
        if (monster == nullptr) {
            std::cout << "nullptr" << std::endl;
        }
        monster->Render(window_);
    }
    window_->display();
}
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
        default:
            break;
        }
    }
}

void Game::initVariables()
{
    gameEnder_ = false;
}
// initalize window
void Game::initWindow()
{
    // gets resolution and etc.
    auto desktopMode = sf::VideoMode::getDesktopMode();
    videomode_ = sf::VideoMode(std::min(VIDEOMODE_DIMS.x, desktopMode.width), std::min(VIDEOMODE_DIMS.y, desktopMode.height));
    window_ = new sf::RenderWindow(videomode_, "Dungeon Crawler");
}

void Game::updateDt() { dt = dtClock.restart().asSeconds(); }

void Game::manageInput()
{
    bool W = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool A = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool S = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    bool D = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    bool LSHIFT = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
    bool LMOUSE = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    bool ENTER = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
    bool twoKeys = ((W || S) && (A || D));
    bool triedMoving = W || A || S || D;

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

    if (LSHIFT) {
        player_->Dash();
    }

    if (LMOUSE) {
        sf::Vector2f mousePos = window_->mapPixelToCoords(sf::Mouse::getPosition(*window_));
        std::list<Projectile*> projectileListToAdd = player_->Attack(mousePos);
        addProjectiles(projectileListToAdd);
    }
    if (ENTER) {
        if (player_->IsAlive()) {
        } else {
            gameReset();
        }
    }

    if (triedMoving) {
        // std::cout << player_->GetPos().x << " " << player_->GetPos().y << std::endl;
        if (collidesWithWall(player_)) {
            player_->RevertMove();
        }
        if (ShouldChangeRoom()) {
            dtClock.restart(); // generating monsters makes dt quite big
        }
    }
}

void Game::checkCollisions(Character* character, Projectile::Type projectileType)
{
    for (auto projectile : projectiles_) {
        if (projectile->GetType() == projectileType && !projectile->hasHit(character)) {
            if (Collision::PixelPerfectTest(projectile->GetSprite(), character->GetSprite())) {
                projectile->hit(character);
                character->TakeDamage(projectile->GetDamage());
                if (!projectile->Penetrates()) {
                    projectile->Kill();
                }
            }
        }
    }
}

void Game::gameReset()
{
}

void Game::checkMonsterCollisions()
{
    if (projectiles_.empty()) {
        return;
    }
    std::vector<Monster*> deadMonsters;
    for (auto& monster : dungeonMap_.GetCurrentRoom()->GetMonsters()) {
        checkCollisions(monster, Projectile::Type::PlayerProjectile);
        if (!monster->IsAlive()) {
            deadMonsters.push_back(monster);
        }
    }
    for (auto monster : deadMonsters) {
        deleteMonster(monster);
    }
}

void Game::checkPlayerCollisions()
{
    if (projectiles_.empty()) {
        return;
    }
    checkCollisions(player_, Projectile::Type::EnemyProjectile);
}

void Game::checkAndHandleProjectileWallCollisions()
{
    for (auto projectile : projectiles_) {
        if (collidesWithWall(projectile)) {
            projectile->Kill();
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

void Game::addProjectiles(std::list<Projectile*> projectiles)
{
    if (projectiles.empty()) {
        return;
    }

    for (auto projectile : projectiles) {
        projectiles_.push_back(projectile);
    }
}

void Game::deleteMonster(Character* m)
{
    auto& monsters = dungeonMap_.GetCurrentRoom()->GetMonsters();
    if (monsters.empty())
        return;

    for (auto it = monsters.begin(); it != monsters.end(); ++it) {
        if (*it == m) {
            delete *it;
            it = monsters.erase(it);
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
bool Game::collidesWithWall(Character* character)
{
    return !dungeonMap_.GetCurrentRoom()->positionIsWalkable(character->GetBaseBoxAt(character->GetPos()));
}
bool Game::collidesWithWall(Projectile* object)
{
    return !dungeonMap_.GetCurrentRoom()->positionIsPenetratable(object->getSpriteBounds());
}

bool Game::ShouldChangeRoom()
{
    if (videomode_.width < player_->GetPos().x) {
        dungeonMap_.MovePlayer(Direction::Right);
        return true;
    } else if (player_->GetPos().x + player_->getSpriteBounds().width < 0) {
        dungeonMap_.MovePlayer(Direction::Left);
        return true;
    } else if (player_->GetPos().y + player_->getSpriteBounds().height < 0) {
        dungeonMap_.MovePlayer(Direction::Up);
        return true;
    } else if (player_->GetPos().y > videomode_.height) {
        dungeonMap_.MovePlayer(Direction::Down);
        return true;
    }
    return false;
}

bool Game::gameLost()
{
    if (!player_->IsAlive()) {
        gameEnder_ = true;
    }
    return gameEnder_;
}
