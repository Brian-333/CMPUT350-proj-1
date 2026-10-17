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
    
    bool wavePending = false;
    int nextWaveFrame = 0;
    int currWave = 1;
    bool startUpPending = false;
    int returnToHomeFrame = 0;
    int nextEnemyAttackFrame = 0;
    int enemyAttackPauseFrames = 45; // Pause after an attack group finishes.
    bool attackGroupActive = false;
    int attackPattern = 0;
    int enemyCount = 0;
    int playerScore = 0;
    int playerLives = 3;
    bool respawnPending = false;
    int level = 1;
    bool coinInserted = false;
    std::weak_ptr<Player> player;
};

#endif
