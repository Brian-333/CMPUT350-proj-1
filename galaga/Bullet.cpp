#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, 
        std::weak_ptr<CMPUT350::GameObject> parent):
        location(location), heading(heading), isPlayerBullet(player), isAlive(true), parent(parent)
{
    width = 5.0f;
    height = 20.0f;
    bounds = CMPUT350::Rect(location.x - width / 2, location.y - height / 2, width, height);
    isColliding = false;
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
    isColliding = false;

    auto shooter = parent.lock();

    // If the bullet goes off screen or if the parent is dead
    if (location.y > context->ScreenContext->GetWindowHeight() 
        || location.y < 0
        || location.x < 0
        || location.x > context->ScreenContext->GetWindowWidth()
        || (!shooter || !shooter->IsAlive()))
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
    CMPUT350::RGBColor color = CMPUT350::Colors::red;
    if (IsPlayerBullet()) { // if a player bullet, make more pink
        color.b += 180;
    }

    if (isColliding) {  // Bullet explodes
        context->ScreenContext->DrawCircle(explodePoint, 5.0f, CMPUT350::RGBColor(255, 128, 0)); // orange circle
    } else {    // Bullet still traveling
        context->ScreenContext->DrawRect({location.x - width / 2, location.y - height / 2, width, height},
        color);
    }

}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
    auto player = std::dynamic_pointer_cast<Player>(obj);

    if ((enemy && IsPlayerBullet()) || (player && !IsPlayerBullet())) {
        isColliding = true;
        Kill();
    }
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) {
    auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
    auto player = std::dynamic_pointer_cast<Player>(obj);

    bool validHit = (enemy && IsPlayerBullet()) || (player && !IsPlayerBullet());

    if (!validHit) {
        return;
    }
   
    if (!isColliding || collisionPoint.y > explodePoint.y) {
        explodePoint = collisionPoint;
    }    

    CollisionEnter(obj);
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

CMPUT350::Point2D Bullet::GetLocation() const {
    return location;
}

const std::vector<CMPUT350::Shape>& Bullet::GetShapes() {
    return shapes;
}


