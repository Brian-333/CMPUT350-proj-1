#include "GraphicsObject.h"

namespace CMPUT350 {

/**
 * @brief Renders the object's background layer.
 *
 * @param contextrender The game context providing engine and draw access.
 *
 * Default implementation does nothing. Override to draw behind other objects.
 */
void GraphicsObject::RenderBackground(GameContext *contextrender) { return; }

/**
 * @brief Renders the object's foreground layer.
 *
 * @param contextrender The game context providing engine and draw access.
 *
 * Default implementation does nothing. Override to draw in front of the background.
 */
void GraphicsObject::RenderForeground(GameContext *contextrender) { return; }

/**
 * @brief Returns the center of the object with respect to rotation.
 *
 * @return Center of object with respect to rotation. The point around which all rotations occur.
 */
Point2D GraphicsObject::GetLocation() const { return Point2D(0, 0); }

/**
 * @brief Returns the rotation of the object.
 *
 * @return The rotation of the object. The angle of rotation in radians.
 */
float GraphicsObject::GetRotation() const { return 0.0f; }

}  // namespace CMPUT350
