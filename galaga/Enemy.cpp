#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include <cmath>
#include <algorithm>

Enemy::Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path):
    formationLocation(loc),
    width(30.f), height(25.f),
    rotation(0.f),
    pathProgress(0.f),
    endProgress(0.f),
    state(State::Spawning),
    maxShots(4),
    shotCooldown(15),
    bulletSpeed(10.0f),
    health(1),
    pointsAwarded(100),
    color(CMPUT350::Colors::yellow)
{
    SetPath(path);
    bounds = {-width / 2, -height / 2, width, height};
    shapes.emplace_back(bounds);
    isAlive = true;
}

// AI-assisted: approximate path length using 100 samples per cubic segment.
// GetPoint validates the control points before the path is stored.
void Enemy::SetPath(const std::vector<CMPUT350::Point2D>& points)
{
    auto previous = CMPUT350::Bezier::GetPoint(points, 0.0f);
    path = points;
    endProgress = static_cast<float>((path.size() - 1) / 3);
    pathProgress = 0.0f;
    distanceTravelled = 0.0f;
    location = previous;
    pathSamples.clear();
    pathSamples.push_back({0.0f, 0.0f});

    const std::size_t sampleCount = ((path.size() - 1) / 3) * 100;
    float distance = 0.0f;
    for (std::size_t i = 1; i <= sampleCount; ++i) {
        float t = endProgress * (static_cast<float>(i) / sampleCount);
        auto point = CMPUT350::Bezier::GetPoint(path, t);
        distance += previous.Distance(point);
        pathSamples.push_back({t, distance});
        previous = point;
    }
}

// AI-assisted: convert distance travelled to t, then update position and facing.
// Linear interpolation between samples gives approximately constant path speed.
void Enemy::AdvancePath()
{
    distanceTravelled = std::min(
        distanceTravelled + movementSpeed, pathSamples.back().distance);

    if (distanceTravelled >= pathSamples.back().distance) {
        pathProgress = endProgress; // Also handles a zero-length path.
    } else {
        auto upper = std::lower_bound(
            pathSamples.begin(), pathSamples.end(), distanceTravelled,
            [](const PathSample& sample, float distance) {
                return sample.distance < distance;
            });
        const auto& lower = *(upper - 1);
        float fraction = (distanceTravelled - lower.distance) /
                         (upper->distance - lower.distance);
        pathProgress = lower.t + fraction * (upper->t - lower.t);
    }

    location = CMPUT350::Bezier::GetPoint(path, pathProgress);
    auto slope = CMPUT350::Bezier::GetSlope(path, pathProgress);
    if (slope.x != 0 || slope.y != 0) {
        rotation = std::atan2(slope.y, slope.x);
    }
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
    notifications = context->mNotificationManager;
}

void Enemy::Shoot(CMPUT350::GameContext* context) {

    float direction = std::cos(GetRotation());  // if the enemy is facing right or left
    float shootAngle = 0.0f;
    if (direction < -0.2f) {    // enemy facing left
        shootAngle -= 0.25;
    } else if (direction > 0.2f) {  // enemy facing right
        shootAngle += 0.25;
    }
    CMPUT350::Point2D heading(shootAngle, 1.0f);
    heading.Normalize();    // so angled bullets dont move faster
    heading *= bulletSpeed;

    auto bullet = std::make_shared<Bullet>(
        location, heading, false, context->CurrObject
    );
    context->mEngineView->AddGameObject(bullet);
}

void Enemy::Update(CMPUT350::GameContext* context)
{
    switch (state) {
        case State::Spawning: {
            AdvancePath();

            if (pathProgress >= endProgress) {
                location = formationLocation;
                state = State::Idle;
            }
            break;
        }
        case State::Idle: {
            location.x = formationLocation.x +
                std::sin(context->currentFrame * 0.03f) * 15.0f;    // movement = 15.0f pixels while hovering, speed = 0.03f
            location.y = formationLocation.y;
            rotation = 0.f;
            break;
        }
        case State::Attacking: {
            AdvancePath();

            // Shooting logic
            if (shotsFired < maxShots &&
                context->currentFrame >= nextShotFrame) {
                    Shoot(context);
                    shotsFired += 1;
                    nextShotFrame = context->currentFrame + shotCooldown;
                }

            if (pathProgress >= endProgress) {
                location = formationLocation;
                state = State::Idle;
            }
            break;
        }
    }
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(bounds, color);
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (!IsAlive()) {   // enemy already dead
        return;
    }

    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    auto player = std::dynamic_pointer_cast<Player>(obj);
    std::string notifyString = "EnemyDestroyed:";

    if (player) {
        notifyString += std::to_string(pointsAwarded * 2);  // enemy would have to not be idle
        Kill();
        notifications->Notify(notifyString);
        return;
    }

    if ((bullet && bullet->IsPlayerBullet())) {
        health -= 1;
        if (health <= 0) {
            if (state == State::Idle) { // normal points awarded
                notifyString += std::to_string(pointsAwarded);
            } else {    // 2x points awarded
                notifyString += std::to_string(pointsAwarded * 2);
            }
            Kill();
            notifications->Notify(notifyString);
        }
        color = CMPUT350::RGBColor({
            static_cast<uint8_t>(color.r * 0.7f),
            static_cast<uint8_t>(color.g * 0.7f), 
            static_cast<uint8_t>(color.b * 0.7f)
        });
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

CMPUT350::Point2D Enemy::GetLocation() const {
    return location;
}

float Enemy::GetRotation() const {
    return rotation;
}

const std::vector<CMPUT350::Shape>& Enemy::GetShapes() {
    return shapes;
}

/////////////////////////////////////////////////////////
// Different enemy types below

// Normal Enemy
Enemy1::Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path): 
    Enemy(loc, path) {}

bool Enemy1::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) {

    if (!IsAlive() || state != State::Idle) {
        return false;
    }
    
    float w = context->ScreenContext->GetWindowWidth() * 0.15f;
    float h =
        (context->ScreenContext->GetWindowHeight() - GetLocation().y - 40.f) * 0.45f;
    auto start = GetLocation();
    // enemy attacks mirror based on screen side
    float direction = start.x < context->ScreenContext->GetWindowWidth() / 2.0f ? -1.0f : 1.0f; 

    // AI used for the offset calculations
    // Figure 8 pattern
    std::vector<CMPUT350::Point2D> offsets = {  
        {0, 0},                         // Start at top
        {-w, 0}, {-w, h / 2}, {0, h},   // Curve left into crossing
        {w, 1.5f * h}, {w, 2 * h}, {0, 2 * h}, // Lower-right to bottom
        {-w, 2 * h}, {-w, 1.5f * h}, {0, h},   // Lower-left to crossing
        {w, h / 2}, {w, 0}, {0, 0}     // Upper-right back to top
    };

    for (auto& point : offsets) {
        point.x *= direction;
        point += start;
    }

    offsets.back() = formationLocation;

    shotsFired = 0;
    nextShotFrame = context->currentFrame + shotCooldown * 2;

    SetPath(offsets);
    state = State::Attacking;
    
    return true;
}

void Enemy1::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(GetBounds(), color);
}


// Heavy Enemy - Slower shots, more hp
Enemy2::Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path):
    Enemy(loc, path) {
        maxShots = 8;
        health = 2;
        shotCooldown = 20;
        bulletSpeed = 6.0f;
        pointsAwarded = 150;
        color = CMPUT350::Colors::white;
}

bool Enemy2::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) {

    if (!IsAlive() || state != State::Idle) {
        return false;
    }
    
    auto start = GetLocation();
    // enemy attacks mirror based on screen side
    float direction = start.x < context->ScreenContext->GetWindowWidth() / 2.0f ? 1.0f : -1.0f; 

    // AI used for the offset calculations
    // Circular pattern
    float r = context->ScreenContext->GetWindowWidth() * 0.15f;
    float k = 0.55228475f; // Control-point factor for a quarter circle

    std::vector<CMPUT350::Point2D> offsets = {
        {0, 0}, // Start at left side of circle

        {0, k*r}, {(1-k)*r, r}, {r, r},       // Down to bottom
        {(1+k)*r, r}, {2*r, k*r}, {2*r, 0},   // Around to right side
        {2*r, -k*r}, {(1+k)*r, -r}, {r, -r},  // Up to top
        {(1-k)*r, -r}, {0, -k*r}, {0, 0}      // Back to start
    };

    for (auto& point : offsets) {
        point.x *= direction;
        point += start;
    }

    offsets.back() = formationLocation;

    shotsFired = 0;
    nextShotFrame = context->currentFrame + shotCooldown * 3;
    SetPath(offsets);
    state = State::Attacking;
    return true;
}

void Enemy2::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(GetBounds(), color);
}

// Special Enemy - lots of shots that go very slow
Enemy3::Enemy3(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path):
    Enemy(loc, path) {
        maxShots = 30;
        health = 2;
        shotCooldown = 10;
        bulletSpeed = 2.0f;
        pointsAwarded = 200;
        color = CMPUT350::Colors::red;
    }

bool Enemy3::Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target)
{
    if (!IsAlive() || state != State::Idle) {
        return false;
    }

    // AI-assisted: two sine-like waves drifting inward, then an offscreen
    // crossing and an upward curve back to the original formation position.
    auto start = GetLocation();
    float screenWidth = context->ScreenContext->GetWindowWidth();
    float screenHeight = context->ScreenContext->GetWindowHeight();
    float direction = start.x < screenWidth / 2.0f ? 1.0f : -1.0f;
    float waveWidth = screenWidth * 0.04f;
    float drift = direction * screenWidth * 0.20f;
    float offscreenY = screenHeight + 80.0f;
    float drop = offscreenY - start.y;
    float oppositeX = screenWidth - start.x;

    std::vector<CMPUT350::Point2D> points = {start};
    // Each segment is one half-wave. The 4/3 factor makes its sideways
    // displacement at the midpoint equal to waveWidth, relative to the drift.
    for (int i = 0; i < 4; ++i) {
        float wave = (i % 2 == 0 ? direction : -direction) * waveWidth * 4.0f / 3.0f;
        float first = (i + 1.0f / 3.0f) / 4.0f;
        float second = (i + 2.0f / 3.0f) / 4.0f;
        float end = (i + 1.0f) / 4.0f;
        points.emplace_back(start.x + drift * first + wave, start.y + drop * first);
        points.emplace_back(start.x + drift * second + wave, start.y + drop * second);
        points.emplace_back(start.x + drift * end, start.y + drop * end);
    }

    // All four points of this crossing segment are below the screen.
    points.emplace_back(start.x + drift, offscreenY + 80.0f);
    points.emplace_back(oppositeX, offscreenY + 80.0f);
    points.emplace_back(oppositeX, offscreenY);

    // Rise on the opposite side, then curve across into formation.
    float returnY = formationLocation.y + 80.0f;
    points.emplace_back(oppositeX, offscreenY - drop / 3.0f);
    points.emplace_back(oppositeX, returnY + drop / 3.0f);
    points.emplace_back(oppositeX, returnY);
    points.emplace_back(oppositeX, formationLocation.y - 60.0f);
    points.emplace_back(formationLocation.x, formationLocation.y - 60.0f);
    points.push_back(formationLocation);

    shotsFired = 0;
    nextShotFrame = context->currentFrame + shotCooldown;
    SetPath(points);
    state = State::Attacking;
    return true;
}

void Enemy3::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(GetBounds(), color);
}
