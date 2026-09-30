#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <vector>

namespace CMPUT350 {

class GameObject;

class EngineView {
public:
    /**
     * @brief Queues a game object to be added to the engine.
     *
     * @param gameObject The game object to add.
     */
    virtual void AddGameObject(std::shared_ptr<GameObject> gameObject) = 0;
};

}  // namespace CMPUT350

#endif
