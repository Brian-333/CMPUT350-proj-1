#include "NotificationManager.h"
#include <algorithm>

namespace CMPUT350 {

void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) {
    if (object.expired()) {
        return;
    }
    if (!mListeners.contains(key)) {
        mListeners[key] = std::vector<std::weak_ptr<GameObject>>();
    }
    mListeners[key].push_back(object);
}

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