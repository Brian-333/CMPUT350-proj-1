#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>
#include <vector>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

enum class ShapeType {
    kLine, kCircle, kRect
};
union ShapeUnion {
    ShapeUnion(Line l) : line(l) {}
    ShapeUnion(Circle c) : circle(c) {}
    ShapeUnion(Rect r) : rect(r) {}
    Line line;
    Circle circle;
    Rect rect; // Axis-aligned rectangle
};
struct Shape {
    Shape(const Line &l) : shape(l), t(ShapeType::kLine) {}
    Shape(const Circle &c) : shape(c), t(ShapeType::kCircle) {}
    Shape(const Rect &r) : shape(r), t(ShapeType::kRect) {}
    ShapeUnion shape;
    ShapeType t;
};

class CollisionObject : public GraphicsObject {
public:
    /**
     * @brief Called when this object begins overlapping another collision object.
     *
     * @param obj The other collision object involved in the overlap.
     */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) = 0;
    /**
     * @brief Called when this object begins overlapping another collision object at a specific point.
     *
     * @param obj The other collision object involved in the overlap.
     * @param collisionPoint The point at which the collision occurred.
     */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const Point2D &collisionPoint)
	{ CollisionEnter(obj); }


    /**
     * @brief Returns the axis-aligned bounds used for collision detection.
     *
     * @return A const reference to this object's bounding rectangle.
     */
    virtual const Rect &GetBounds() = 0;
    /**
     * @brief Returns the shapes used for collision detection.
     *
     * @return A const reference to this object's shapes.
     */
    virtual const std::vector<Shape> &GetShapes() = 0;
};

}  // namespace CMPUT350

#endif
