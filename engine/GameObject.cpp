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

/**
 * @brief Receives a notification from the notification manager.
 *
 * @param key The key that the notification was sent for.
 *
 * Default implementation does nothing. Override to handle the notification.
 */
void GameObject::ReceiveNotification(const std::string& key) { return; }

}  // namespace CMPUT350
