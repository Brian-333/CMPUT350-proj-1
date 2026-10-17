#include "NotificationManager.h"
#include <algorithm>
#include <stdexcept>

namespace CMPUT350 {

/**
 * @brief Registers a game object to receive notifications for a message key.
 *
 * Expired weak pointers are ignored. The manager stores a weak reference and
 * does not keep the object alive.
 *
 * @param object Weak reference to the listening game object.
 * @param key The notification string to listen for.
 */
void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) {
    if (object.expired()) {
        return;
    }
    if (!mListeners.contains(key)) {
        mListeners[key] = std::vector<std::weak_ptr<GameObject>>();
    }
    mListeners[key].push_back(object);
}

/**
 * @brief Unregisters a game object from a notification key.
 *
 * Throws if the key has never been registered.
 *
 * @param object Weak reference to the listening game object.
 * @param key The notification string to stop listening for.
 */
void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key) {
    if (!mListeners.contains(key)) {
        throw std::runtime_error("Key not found");
    }
    if (auto listener = object.lock()) {
        mListeners[key].erase(std::remove_if(mListeners[key].begin(), mListeners[key].end(),
        [listener](const auto& other) { return other.lock() == listener; }),
        mListeners[key].end());
    }
}

/**
 * @brief Delivers a notification immediately to all listeners for the message.
 *
 * Calls ReceiveNotification on each live listener. Expired listeners are removed
 * after the delivery pass. Throws if the message key has never been registered.
 *
 * @param message The notification string to deliver.
 */
void NotificationManager::Notify(const std::string& message) {
    if (!mListeners.contains(message)) {
        throw std::runtime_error("Key not found");
    }

    static bool deadListenerFound;
    deadListenerFound = false;
    // Notify listeners
    for (const auto& listener : mListeners[message]) {
        if (auto shared_ref = listener.lock()) {
            shared_ref->ReceiveNotification(message);
        } else {
            deadListenerFound = true;
        }
    }
    // Clean up dead listeners for current message
    if (deadListenerFound) {
        mListeners[message].erase(std::remove_if(mListeners[message].begin(), mListeners[message].end(),
        [](const auto& listener) { return listener.expired(); }),
        mListeners[message].end());
    }
}
}
