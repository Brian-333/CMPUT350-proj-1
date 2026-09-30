#include <cassert>
#include <algorithm>
#include "Player.h"
#include "Bullet.h"

Player::Player(CMPUT350::Point2D loc):
    location(loc), width(40.0f), height(40.0f), isAlive(true)
{
    bounds = {location.x - width / 2, location.y - height / 2, width, height};
}

void Player::Initialize(CMPUT350::GameContext* context)
{
}

void Player::Update(CMPUT350::GameContext* context)
{
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    float movement = 10.0f;

    if (key == 'A' || key == 'a') {
        location.x -= movement;

        location.x = std::max(location.x, width / 2);
    }
    else if (key == 'D' || key == 'd') {
        location.x += movement;

        location.x = std::min(location.x, 
            context->ScreenContext->GetWindowWidth() - width / 2);
    }
    else if (key == ' ') {
        for (auto& bullet : bullets) {
            if (bullet.expired()) {
                auto newBullet = std::make_shared<Bullet>(
                    location, CMPUT350::Point2D({0, -10}), true
                );
                context->mEngineView->AddGameObject(newBullet);
                bullet = newBullet;
                return true;
            }
        }
        return true;   // 2 bullets already existing
    }
    else {
        return false;
    }

    bounds = {location.x - width / 2, location.y - height / 2, width, height};
    return true;
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

    if (bullet && !bullet->IsPlayerBullet()) {
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
