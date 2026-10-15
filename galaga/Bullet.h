#ifndef BULLET_H
#define BULLET_H

#include <vector>
#include "CollisionObject.h"
#include "GraphicsObject.h"
#include "GameContext.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
	Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent);
	bool IsPlayerBullet();
	void Update(CMPUT350::GameContext *context) override;
	void LateUpdate(CMPUT350::GameContext *context) override;
	void RenderForeground(CMPUT350::GameContext *context) override;
	bool IsAlive() const override;
	void Kill() override;
	const std::vector<CMPUT350::Shape> &GetShapes() override;
	void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject> &obj) override;
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) override;
	const CMPUT350::Rect &GetBounds() override;

	CMPUT350::Point2D GetLocation() const override;
	float GetRotation() const override;
private:
    bool isPlayerBullet;
    bool isAlive;
    CMPUT350::Point2D heading;
    CMPUT350::Point2D location;
    float width;
    float height;
    CMPUT350::Rect bounds;
    std::vector<CMPUT350::Shape> shapes;
    std::weak_ptr<CMPUT350::GameObject> parent;
    bool isColliding;
    CMPUT350::Point2D explodePoint;
};
#endif // BULLET_H
