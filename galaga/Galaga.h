#ifndef GALAGA_H
#define GALAGA_H

#include "GameObject.h"
#include "Player.h"
#include <memory>
#include "MathUtil.h"
#include <array>
#include <vector>

class Galaga : public CMPUT350::GameObject {
public:
    Galaga(int width, int height);

    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& key) override;

private:
    // AI-assisted: game-owned controls; engine mappings remain installed.
    struct KeyBinding {
        std::string label;
        int key;
        std::string down;
        std::string up;
    };
    std::array<KeyBinding, 3> bindings;
    int selectedBinding = -1; // Select by pressing the action's current key.
    std::string keybindMessage;
    std::vector<bool> keysHeld;
    CMPUT350::NotificationManager* notifications = nullptr; // Engine-owned.
    void HandleKey(int key, bool pressed);

    enum class State {
        Keybinds,
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
    int respawnDelayFrames = 60; // Two seconds after enemies finish returning.
    int respawnAtFrame = -1; // -1 means the countdown has not started.
    int level = 1;
    bool coinInserted = false;
    std::weak_ptr<Player> player;
};

#endif
