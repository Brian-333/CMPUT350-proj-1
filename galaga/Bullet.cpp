#include "Bullet.h"
#include "Enemy.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player):
    location(location), heading(heading), isPlayerBullet(player), isAlive(true)
{
    width = 5.0f;
    height = 20.0f;
    bounds = CMPUT350::Rect(location.x - width / 2, location.y - height / 2, width, height);
}

bool Bullet::IsPlayerBullet()
{
    return isPlayerBullet;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
}

void Bullet::Update(CMPUT350::GameContext* context)
{   
    CMPUT350::Rect oldBounds(location.x - width / 2, location.y - height / 2, width, height);
    location += heading;
    CMPUT350::Rect newBounds(location.x - width / 2, location.y - height / 2, width, height);
    newBounds |= oldBounds;
    bounds = newBounds;

    // If the bullet goes off screen
    if (location.y > context->ScreenContext->GetWindowHeight() 
        || location.y < 0
        || location.x < 0
        || location.x > context->ScreenContext->GetWindowWidth()) 
    {
        Kill();
    }
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect({location.x - width / 2, location.y - height / 2, width, height},
        CMPUT350::Colors::magenta);
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    auto enemy = std::dynamic_pointer_cast<Enemy>(obj);

    if (enemy && isPlayerBullet) {
        Kill();
    }
}

void Bullet::Kill()
{
    isAlive = false;
}

bool Bullet::IsAlive() const
{
    return isAlive;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    return bounds;
}
