#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(CMPUT350::Point2D loc): 
    center(loc), width(30.f), height(25.f)
{
    bounds = {center.x - width / 2, center.y - height / 2, width, height};
    isAlive = true;
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

void Enemy::Update(CMPUT350::GameContext* context)
{
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(bounds, CMPUT350::Colors::yellow);
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);

    if (bullet && bullet->IsPlayerBullet()) {
        Kill();
    }
}

void Enemy::Kill()
{
    isAlive = false;
}

bool Enemy::IsAlive() const
{
    return isAlive;
}

const CMPUT350::Rect& Enemy::GetBounds()
{
    return bounds;
}
