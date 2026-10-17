#ifndef GALAGA_H
#define GALAGA_H

#include "GameObject.h"
#include "Player.h"
#include <memory>
#include "MathUtil.h"

class Galaga : public CMPUT350::GameObject {
public:
    Galaga(int width, int height);

    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& key) override;

private:
    enum class State {
        Waiting,
        Startup,
        Waves,
        Gameplay,
        GameOver
    };

    State state = State::Waiting;
    int screenWidth;
    int screenHeight;
    CMPUT350::Point2D playerSpawn;
    int startUpFrames = 90; // 3 seconds
    int startAtFrame = 0;
    bool startUpPending = false;
    int enemyCount = 0;
    int playerScore = 0;
    int playerLives = 3;
    int level = 1;
    bool coinInserted = false;
    std::weak_ptr<Player> player;
};

#endif