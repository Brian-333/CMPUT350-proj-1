#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "GameObject.h"

namespace CMPUT350 {

class NotificationManager {
public:
    /**
        * @brief Registers a game object to receive notifications for a specific key.
        * 
        * @param object The game object to register.
        * @param key The key to register the game object for.
        */
    void Register(std::weak_ptr<GameObject> object, const std::string& key);
    /**
        * @brief Unregisters a game object from receiving notifications for a specific key.
        * 
        * @param object The game object to unregister.
        * @param key The key to unregister the game object for.
        */
    void Unregister(std::weak_ptr<GameObject> object, const std::string& key);
    /**
        * @brief Notifies all game objects registered for a specific key.
        * 
        * @param message The message to notify the game objects with.
        */
    void Notify(const std::string& message);
private:
    std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> mListeners;
};
}

#endif