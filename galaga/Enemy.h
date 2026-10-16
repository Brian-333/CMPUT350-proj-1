#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"
#include "Bezier.h"

class Enemy : public CMPUT350::CollisionObject
{
public:
    Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    virtual bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) = 0;
    void Shoot(CMPUT350::GameContext *context);

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;

    const std::vector<CMPUT350::Shape> &GetShapes() override;
    CMPUT350::Point2D GetLocation() const override;
	// Return the rotation of the object in radians
	float GetRotation() const override;

protected:
    // AI-assisted: call whenever an entrance, attack, or return path changes.
    // Builds the distance table and resets movement to the start of the path.
    void SetPath(const std::vector<CMPUT350::Point2D>& points);

    int health;
    int maxShots;
    int shotsFired = 0;
    float bulletSpeed;
    int shotCooldown;   // in frames
    int nextShotFrame = 0;
    CMPUT350::Point2D formationLocation;
    int pointsAwarded;

    CMPUT350::RGBColor color;

    enum class State {
        Spawning,
        Idle,
        Attacking
    };
    State state;

private:
    // AI-assisted: advance movementSpeed pixels along the current path per frame.
    void AdvancePath();
    struct PathSample {
        float t;
        float distance;
    };
    std::vector<PathSample> pathSamples;
    float distanceTravelled = 0.0f;
    float movementSpeed = 3.0f; // Pixels per frame along the path.

    CMPUT350::NotificationManager* notifications = nullptr;

    CMPUT350::Rect bounds;
    std::vector<CMPUT350::Point2D> path;
    float pathProgress;
    float endProgress;
    
    CMPUT350::Point2D location;
    std::vector<CMPUT350::Shape> shapes;
    float width;
    float height;
    float rotation;
    bool isAlive;
};

class Enemy1 : public Enemy {
public:
	Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
	bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
	void RenderForeground(CMPUT350::GameContext *context) override;
};

class Enemy2 : public Enemy {
public:
	Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
	bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
	void RenderForeground(CMPUT350::GameContext *context) override;
};

class Enemy3 : public Enemy {
public:
	Enemy3(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
	bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
	void RenderForeground(CMPUT350::GameContext *context) override;
};



#endif
