#include "GameObject.h"

namespace CMPUT350 {

/**
 * @brief Initializes the game object with the given game context.
 *
 * @param context The game context providing engine and draw access.
 *
 * Default implementation does nothing. Override to set up object state
 * when the object becomes active.
 */
void GameObject::Initialize(GameContext *context) { return; }

/**
 * @brief Updates the game object for the current frame.
 *
 * @param context The game context providing engine and draw access.
 *
 * Default implementation does nothing. Override to implement per-frame logic.
 */
void GameObject::Update(GameContext *context) { return; }

/**
 * @brief Performs a late update after collisions have been processed.
 *
 * @param context The game context providing engine and draw access.
 *
 * Default implementation does nothing. Override for logic that should run
 * after the main update and collision pass.
 */
void GameObject::LateUpdate(GameContext *context) { return; }

/**
 * @brief Renders UI elements for the game object.
 *
 * @param contextrender The game context providing engine and draw access.
 *
 * Default implementation does nothing. Override to draw UI overlays.
 */
void GameObject::RenderUI(GameContext *contextrender) { return; }

/**
 * @brief Handles a keyboard text-entered event.
 *
 * @param context The game context providing engine and draw access.
 * @param key The unicode character that was entered.
 * @return True if the event was handled; false otherwise.
 *
 * Default implementation ignores the key and returns false.
 */
bool GameObject::HandleKeyEvent(GameContext *context, char key) { return false; }

/**
 * @brief Reports whether the game object is still alive.
 *
 * @return True if the object should remain in the engine; false to remove it.
 *
 * Default implementation always returns true.
 */
bool GameObject::IsAlive() const { return true; }

/**
 * @brief Marks the game object for removal or cleanup.
 *
 * Default implementation does nothing. Override to signal that the object
 * should no longer be considered alive.
 */
void GameObject::Kill() {}

}  // namespace CMPUT350
