#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

class CollisionObject : public GraphicsObject {
public:
    /**
     * @brief Called when this object begins overlapping another collision object.
     *
     * @param obj The other collision object involved in the overlap.
     */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) = 0;

    /**
     * @brief Returns the axis-aligned bounds used for collision detection.
     *
     * @return A const reference to this object's bounding rectangle.
     */
    virtual const Rect &GetBounds() = 0;
};

}  // namespace CMPUT350

#endif
