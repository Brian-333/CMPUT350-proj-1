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

}  // namespace CMPUT350
