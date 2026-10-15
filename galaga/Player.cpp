#include <cassert>
#include <algorithm>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

Player::Player(CMPUT350::Point2D loc):
    isAlive(true),
    location(loc),
    bounds(loc.x - 20.0f, loc.y - 20.0f, 40.0f, 40.0f),
    width(40.0f),
    height(40.0f),
    fireRequested(false),
    left(false),
    right(false)
{
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    auto* notifications = context->mNotificationManager;

    notifications->Register(context->CurrObject, "MoveLeftUp");
    notifications->Register(context->CurrObject, "MoveLeftDown");
    notifications->Register(context->CurrObject, "MoveRightUp");
    notifications->Register(context->CurrObject, "MoveRightDown");
    notifications->Register(context->CurrObject, "Fire");

}

void Player::Update(CMPUT350::GameContext* context)
{
    const float movement = 15.0f;
    int direction = static_cast<int>(right) - static_cast<int>(left);
    location.x += direction * movement;

    location.x = std::clamp(location.x, width / 2, 
        context->ScreenContext->GetWindowWidth() - width / 2);

    bounds = {location.x - width / 2, location.y - height / 2, width, height};

    if (fireRequested) {
        for (auto& bullet : bullets) {
            if (bullet.expired()) {
                auto newBullet = std::make_shared<Bullet>(
                    location, CMPUT350::Point2D({0, -20}), true, context->CurrObject
                );
                context->mEngineView->AddGameObject(newBullet);
                bullet = newBullet;
                break;
            }
        }
    }

    fireRequested = false;
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

void Player::ReceiveNotification(const std::string& key) {
    if (key == "MoveLeftUp") {
        left = false;
    } else if (key == "MoveLeftDown") {
        left = true;
    } else if (key == "MoveRightDown") {
        right = true;
    } else if (key == "MoveRightUp") {
        right = false;
    } else if (key == "Fire") {
        fireRequested = true;
    }
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::Rect mainPiece(location.x - width / 4, location.y - height / 2, width / 2, height);
    CMPUT350::Rect crossPiece(location.x - width / 2, location.y - height / 4, width, height / 2);
    CMPUT350::Rect leftTail(location.x - width / 2, location.y, width / 6, height / 2);
    CMPUT350::Rect rightTail(location.x + width / 2 - width / 6, location.y, width / 6, height / 2);

    context->ScreenContext->DrawRect(mainPiece, CMPUT350::Colors::white);
    context->ScreenContext->DrawRect(crossPiece, CMPUT350::Colors::white);
    context->ScreenContext->DrawRect(leftTail, CMPUT350::Colors::white);
    context->ScreenContext->DrawRect(rightTail, CMPUT350::Colors::white);
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    auto enemy = std::dynamic_pointer_cast<Enemy>(obj);

    if (enemy || (bullet && !bullet->IsPlayerBullet())) {
        Kill();
    }
}

void Player::Kill()
{
    isAlive = false;
}

bool Player::IsAlive() const
{
    return isAlive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    return bounds;
}

CMPUT350::Point2D Player::GetLocation() const {
    return location;
}

float Player::GetRotation() const {
    return 0.f;
}

const std::vector<CMPUT350::Shape>& Player::GetShapes() {
    return shapes;
}
