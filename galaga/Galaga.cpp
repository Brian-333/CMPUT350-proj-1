#include "Enemy.h"
#include "Galaga.h"
#include "Player.h"
#include "Stars.h"
#include "GameContext.h"
#include "Keyboard.h"
#include <iostream>

namespace {
// AI-assisted: keep readable key labels in game code without an SFML dependency.
std::string KeyName(int key) {
    using namespace CMPUT350;
    if (key >= Key::A && key <= Key::Z) return std::string(1, 'A' + key - Key::A);
    if (key >= Key::Num1 && key <= Key::Num9) return std::to_string(key - Key::Num1 + 1);
    switch (key) {
        case Key::Num0: return "0";
        case Key::Space: return "Space";
        case Key::Left: return "Left Arrow";
        case Key::Right: return "Right Arrow";
        case Key::Up: return "Up Arrow";
        case Key::Down: return "Down Arrow";
        case Key::Enter: return "Enter";
        case Key::Tab: return "Tab";
        case Key::Backspace: return "Backspace";
        case Key::LShift: return "Left Shift";
        case Key::RShift: return "Right Shift";
        case Key::LControl: return "Left Control";
        case Key::RControl: return "Right Control";
        default: return "Key " + std::to_string(key);
    }
}
}

Galaga::Galaga(int width, int height): screenWidth(width), screenHeight(height) 
{
    bindings = {{
        {"Move Left", CMPUT350::Key::A, "MoveLeftDown", "MoveLeftUp"},
        {"Move Right", CMPUT350::Key::D, "MoveRightDown", "MoveRightUp"},
        {"Fire", CMPUT350::Key::Space, "Fire", ""}
    }};
    keysHeld.resize(CMPUT350::Key::ScanCount, false);
    playerSpawn = CMPUT350::Point2D(screenWidth / 2.f, screenHeight * 0.6f);
}

void Galaga::Initialize(CMPUT350::GameContext* context) {
    // AI-assisted: create one background for the lifetime of the game manager.
    context->mEngineView->AddGameObject(std::make_shared<Stars>(
        200, CMPUT350::Rect(0, 0, screenWidth, screenHeight)));
    notifications = context->mNotificationManager;
    // AI-assisted: use existing engine APIs to report raw key messages to
    // Galaga. Galaga translates them to actions, so rebinding just changes a key.
    for (int key = 0; key < static_cast<int>(CMPUT350::Key::ScanCount); key++) {
        if (key == CMPUT350::Key::P) continue; // Engine reserves pause.
        std::string down = "KeyDown:" + std::to_string(key);
        std::string up = "KeyUp:" + std::to_string(key);
        context->mEngineView->InstallKeyDownNotification(key, down);
        context->mEngineView->InstallKeyUpNotification(key, up);
        notifications->Register(context->CurrObject, down);
        notifications->Register(context->CurrObject, up);
    }

    notifications->Register(context->CurrObject, "InsertCoin");
    notifications->Register(context->CurrObject, "StartGame");
    notifications->Register(context->CurrObject, "EnemyDestroyed:100");
    notifications->Register(context->CurrObject, "EnemyDestroyed:150");
    notifications->Register(context->CurrObject, "EnemyDestroyed:200");
    notifications->Register(context->CurrObject, "EnemyDestroyed:300");
    notifications->Register(context->CurrObject, "EnemyDestroyed:400");
}

// AI-assisted: select an action with its existing key, then accept a new key.
// All mapping decisions stay here; Player still receives its original messages.
void Galaga::HandleKey(int key, bool pressed) {
    using namespace CMPUT350;
    if (key < 0 || key >= static_cast<int>(Key::ScanCount)) return;
    if (pressed && keysHeld[key]) return; // Ignore OS key-repeat events.
    keysHeld[key] = pressed;

    if (state == State::Keybinds) {
        if (!pressed) return;
        if (key == Key::Escape) {
            if (selectedBinding >= 0) {
                selectedBinding = -1;
                keybindMessage = "Change cancelled.";
            } else {
                state = State::Waiting;
            }
        } else if (selectedBinding >= 0) {
            if (key == Key::K || key == Key::Num1 || key == Key::Num5 || key == Key::P) {
                keybindMessage = "K, 1, 5 and P are reserved.";
                return;
            }
            for (int i = 0; i < static_cast<int>(bindings.size()); i++) {
                if (i != selectedBinding && bindings[i].key == key) {
                    keybindMessage = "That key is already assigned. Try another.";
                    return;
                }
            }
            bindings[selectedBinding].key = key;
            selectedBinding = -1;
            keybindMessage = "Binding updated.";
        } else {
            for (int i = 0; i < static_cast<int>(bindings.size()); i++) {
                if (bindings[i].key == key) {
                    selectedBinding = i;
                    keybindMessage.clear();
                    break;
                }
            }
        }
        return;
    }

    if (pressed && state == State::Waiting) {
        if (key == Key::K) {
            state = State::Keybinds;
            selectedBinding = -1;
            keybindMessage.clear();
        } else if (key == Key::Num5) {
            ReceiveNotification("InsertCoin");
        } else if (key == Key::Num1) {
            ReceiveNotification("StartGame");
        }
        return;
    }

    auto currentPlayer = player.lock();
    if ((state == State::Startup || state == State::Waves || state == State::Gameplay) &&
        currentPlayer && currentPlayer->IsAlive()) {
        for (const auto& binding : bindings) {
            if (binding.key == key) {
                const auto& message = pressed ? binding.down : binding.up;
                if (!message.empty()) notifications->Notify(message);
            }
        }
    }
}

void Galaga::Update(CMPUT350::GameContext* context) {
    // AI-assisted: count deaths in both active phases, even if the engine
    // still holds the killed player until its next removal pass.
    if ((state == State::Waves || state == State::Gameplay) && !respawnPending) {
        auto currentPlayer = player.lock();
        if (!currentPlayer || !currentPlayer->IsAlive()) {
            respawnPending = true;
            respawnAtFrame = -1;
            playerLives -= 1;
            if (playerLives <= 0) {
                returnToHomeFrame = context->currentFrame + 150;
                state = State::GameOver;
                return;
            }
        }
    }

    switch (state) {
        case (State::Keybinds): {
            break;
        }
        case (State::Waiting): {
            break;
        }
        case (State::Startup): {
            if (startUpPending) {
                startAtFrame = context->currentFrame + startUpFrames;
                startUpPending = false;
            }
            if (player.expired()) {
                auto newPlayer = std::make_shared<Player>(playerSpawn);
                player = newPlayer;
                context->mEngineView->AddGameObject(newPlayer);
                respawnPending = false;
                respawnAtFrame = -1;
            }
            
            if (context->currentFrame >= startAtFrame) {
                wavePending = true;
                nextWaveFrame = context->currentFrame;
                state = State::Waves;
            }
            break;
        }

        // AI-assisted: wave ideas, enemy groupings, and the switch/loop structure
        // were supplied by the student. Codex implemented the spawning and
        // Bezier point calculations, distance-based entrance staggering, and
        // expansion to eight ships per row. See README.md for prompt details.
        case (State::Waves): {
            if (wavePending && context->currentFrame >= nextWaveFrame) {
                switch (currWave) {
                    case (1): {
                        for (int i = 0; i < 8; i++) {
                            CMPUT350::Point2D formation(
                                (i + 0.5f) * screenWidth / 8.0f, 100.0f);

                            // fly in from the left side
                            std::vector<CMPUT350::Point2D> path = {
                                {formation.x - screenWidth - 80.0f, screenHeight * 0.30f},
                                {formation.x - screenWidth * 0.60f, screenHeight * 0.30f},
                                {formation.x - screenWidth * 0.20f, formation.y},
                                formation
                            };
                            auto enemy = std::make_shared<Enemy1>(formation, path);
                            context->mEngineView->AddGameObject(enemy);
                            enemyCount += 1;
                        }

                        currWave += 1;
                        // Give the first row time to enter before wave 2
                        nextWaveFrame = context->currentFrame + 120;
                        break;
                    }
                    case (2): {
                        for (int i = 0; i < 8; i++) {
                            bool endColumn = i == 0 || i == 7;

                            // Six normal ships, one heavy per edge
                            float x = (i + 0.5f) * screenWidth / 8.0f;
                            CMPUT350::Point2D formation(x, 150.0f);
                            float side = (i == 7) ? 1.0f : -1.0f;
                            float entranceDistance = screenWidth + 80.0f;
                            // Heavy ships spawn further so they take longer to show up
                            if (endColumn) entranceDistance += screenWidth;
                            std::vector<CMPUT350::Point2D> path = {
                                {formation.x + side * entranceDistance, screenHeight * 0.30f},
                                {formation.x + side * screenWidth * 0.60f, screenHeight * 0.30f},
                                {formation.x + side * screenWidth * 0.20f, formation.y},
                                formation
                            };

                            if (endColumn) {
                                context->mEngineView->AddGameObject(
                                    std::make_shared<Enemy2>(formation, path));
                            } else {
                                context->mEngineView->AddGameObject(
                                    std::make_shared<Enemy1>(formation, path));
                            }
                            enemyCount += 1;
                        }

                        currWave += 1;
                        nextWaveFrame = context->currentFrame + 120;
                        break;
                    }
                    case (3): {
                        auto crossing = playerSpawn;
                        if (auto currentPlayer = player.lock()) {
                            crossing = currentPlayer->GetLocation();
                        }

                        for (int i = 0; i < 8; i++) {
                            bool fromRight = i < 4;
                            float extraDistance = (fromRight ? i : i + 2) * 120.0f;
                            CMPUT350::Point2D start(
                                fromRight ? screenWidth + 80.0f + extraDistance
                                          : -80.0f - extraDistance,
                                screenHeight + 80.0f + extraDistance);
                            CMPUT350::Point2D formation(
                                (i + 0.5f) * screenWidth / 8.0f, 200.0f);
                            auto toPlayer = crossing - start;

                            // The player position is passed through
                            std::vector<CMPUT350::Point2D> path = {
                                start,
                                start + toPlayer * (1.0f / 3.0f),
                                start + toPlayer * (2.0f / 3.0f),
                                crossing,
                                {crossing.x, crossing.y - screenHeight * 0.20f},
                                {formation.x, formation.y + 100.0f},
                                formation
                            };

                            if (i == 1 || i == 3) {
                                context->mEngineView->AddGameObject(
                                    std::make_shared<Enemy3>(formation, path));
                            } else if (i == 4 || i == 5) {
                                context->mEngineView->AddGameObject(
                                    std::make_shared<Enemy2>(formation, path));
                            } else {
                                context->mEngineView->AddGameObject(
                                    std::make_shared<Enemy1>(formation, path));
                            }
                            enemyCount += 1;
                        }

                        currWave += 1;
                        nextWaveFrame = context->currentFrame + 120;
                        break;
                    }
                    case (4): {
                        auto crossing = playerSpawn;
                        if (auto currentPlayer = player.lock()) {
                            crossing = currentPlayer->GetLocation();
                        }

                        for (int i = 0; i < 8; i++) {
                            CMPUT350::Point2D formation(
                                (i + 0.5f) * screenWidth / 8.0f, 250.0f);
                            std::vector<CMPUT350::Point2D> path;

                            if (i < 4) {
                                float extraDistance = i * 120.0f;
                                CMPUT350::Point2D start(
                                    screenWidth + 80.0f + extraDistance,
                                    screenHeight + 80.0f + extraDistance);
                                auto toPlayer = crossing - start;
                                path = {
                                    start,
                                    start + toPlayer * (1.0f / 3.0f),
                                    start + toPlayer * (2.0f / 3.0f),
                                    crossing,
                                    {crossing.x, crossing.y - screenHeight * 0.20f},
                                    {formation.x, formation.y + 100.0f},
                                    formation
                                };
                            } else {
                                // The final four come from left like normal.
                                path = {
                                    {formation.x - screenWidth - 80.0f, screenHeight * 0.30f},
                                    {formation.x - screenWidth * 0.60f, screenHeight * 0.30f},
                                    {formation.x - screenWidth * 0.20f, formation.y},
                                    formation
                                };
                            }

                            context->mEngineView->AddGameObject(
                                std::make_shared<Enemy1>(formation, path));
                            enemyCount += 1;
                        }

                        currWave += 1;
                        nextWaveFrame = context->currentFrame + 120;
                        break;
                    }
                    case (5): {
                        auto crossing = playerSpawn;
                        if (auto currentPlayer = player.lock()) {
                            crossing = currentPlayer->GetLocation();
                        }

                        for (int i = 0; i < 8; i++) {
                            CMPUT350::Point2D formation(
                                (i + 0.5f) * screenWidth / 8.0f, 300.0f);
                            std::vector<CMPUT350::Point2D> path;

                            if (i == 2 || i == 5) {
                                // Inner ships cross from the opposite bottom corner, delayed
                                float extraDistance = screenWidth * 1.5f;
                                CMPUT350::Point2D start(
                                    i == 2 ? screenWidth + 80.0f + extraDistance
                                           : -80.0f - extraDistance,
                                    screenHeight + 80.0f + extraDistance);
                                auto toPlayer = crossing - start;
                                path = {
                                    start,
                                    start + toPlayer * (1.0f / 3.0f),
                                    start + toPlayer * (2.0f / 3.0f),
                                    crossing,
                                    {crossing.x, crossing.y - screenHeight * 0.20f},
                                    {formation.x, formation.y + 100.0f},
                                    formation
                                };
                                context->mEngineView->AddGameObject(
                                    std::make_shared<Enemy3>(formation, path));
                            } else {
                                float side = i < 2 ? -1.0f : 1.0f;
                                float entranceDistance = screenWidth + 80.0f;
                                // The two heavy ships arrive last in the middle.
                                if (i == 3 || i == 4) entranceDistance += screenWidth * 3.0f;
                                path = {
                                    {formation.x + side * entranceDistance, screenHeight * 0.30f},
                                    {formation.x + side * screenWidth * 0.60f, screenHeight * 0.30f},
                                    {formation.x + side * screenWidth * 0.20f, formation.y},
                                    formation
                                };
                                if (i == 3 || i == 4) {
                                    context->mEngineView->AddGameObject(
                                        std::make_shared<Enemy2>(formation, path));
                                } else {
                                    context->mEngineView->AddGameObject(
                                        std::make_shared<Enemy1>(formation, path));
                                }
                            }
                            enemyCount += 1;
                        }
                        currWave += 1;
                        break;
                    }
                    case (6): {
                        bool stillEntering = false;
                        for (auto& object : *context->mEngineView) {
                            auto enemy = std::dynamic_pointer_cast<Enemy>(object);

                            if (enemy && enemy->IsAlive() && enemy->IsEntering()) {
                                stillEntering = true;
                                break;
                            }
                        }
                        if (!stillEntering) {
                            wavePending = false;
                            state = State::Gameplay;
                            currWave = 1;
                            attackGroupActive = false;
                            attackPattern = 0;
                            nextEnemyAttackFrame = context->currentFrame + enemyAttackPauseFrames;
                        }
                    }
                }
            }
            break;
        }
        case (State::Gameplay): {
            if (enemyCount == 0 && !respawnPending) {  // progress to next level
                level += 1;
                startUpPending = true;
                state = State::Startup;
                break;
            }

            bool enemiesAttacking = false;
            for (auto& object : *context->mEngineView) {
                auto enemy = std::dynamic_pointer_cast<Enemy>(object);
                if (enemy && enemy->IsAlive() && enemy->IsAttacking()) {
                    enemiesAttacking = true;
                }
            }

            // Start the pause when the last attacker finishes, rather than
            // while the group is still flying its paths.
            if (attackGroupActive && !enemiesAttacking) {
                nextEnemyAttackFrame = context->currentFrame + enemyAttackPauseFrames;
            }
            attackGroupActive = enemiesAttacking;

            // AI-assisted: leave the lives message visible for two seconds
            // after all enemies return, then spawn exactly one replacement.
            if (respawnPending && !enemiesAttacking) {
                if (respawnAtFrame == -1) {
                    respawnAtFrame = context->currentFrame + respawnDelayFrames;
                }
                if (context->currentFrame >= respawnAtFrame) {
                    auto newPlayer = std::make_shared<Player>(playerSpawn);
                    player = newPlayer;
                    context->mEngineView->AddGameObject(newPlayer);
                    respawnPending = false;
                    respawnAtFrame = -1;
                    nextEnemyAttackFrame = context->currentFrame + enemyAttackPauseFrames;
                }
            }

            auto currPlayer = player.lock();
            if (!enemiesAttacking && currPlayer && 
                currPlayer->IsAlive() && !respawnPending
                && context->currentFrame >= nextEnemyAttackFrame) 
            {   // only have enemies attack when the player is alive and no other enemies are attacking
                std::vector<std::shared_ptr<Enemy>> enemies;
                for (auto object : *context->mEngineView) {
                    auto enemy = std::dynamic_pointer_cast<Enemy>(object);
                    if (enemy && enemy->IsAlive()) {
                        enemies.push_back(enemy);
                    }
                }

                // AI-assisted: cycle through the student's four selection
                // patterns, counting only living enemies in engine order.
                int count = static_cast<int>(enemies.size());
                std::vector<int> selected;
                switch (attackPattern) {
                    case 0: { // First three.
                        for (int i = 0; i < count && i < 3; i++) {
                            selected.push_back(i);
                        }
                        break;
                    }
                    case 1: { // Third, fifth, seventh, ninth (up to four).
                        for (int i = 2; i < count && selected.size() < 4; i += 2) {
                            selected.push_back(i);
                        }
                        break;
                    }
                    case 2: { // First two and last two, without duplicates.
                        for (int i = 0; i < count; i++) {
                            if (i < 2 || i >= count - 2) {
                                selected.push_back(i);
                            }
                        }
                        break;
                    }
                    case 3: { // Six consecutive enemies from fifth, wrapping.
                        for (int i = 0; i < count && i < 6; i++) {
                            selected.push_back((4 + i) % count);
                        }
                        break;
                    }
                }

                int attacksUsed = 0;
                for (int index : selected) {
                    if (enemies[index]->Attack(context, currPlayer->GetLocation())) {
                        attacksUsed += 1;
                    }
                }
                attackPattern = (attackPattern + 1) % 4;
                attackGroupActive = attacksUsed > 0;
                if (!attackGroupActive) {
                    nextEnemyAttackFrame = context->currentFrame + enemyAttackPauseFrames;
                }
            }
            break;
        }
        case (State::GameOver): {
            if (context->currentFrame >= returnToHomeFrame) {
                for (auto& object : *context->mEngineView) {
                    auto galaga = std::dynamic_pointer_cast<Galaga>(object);
                    // AI-assisted: preserve the background when resetting the game.
                    if (galaga || std::dynamic_pointer_cast<Stars>(object)) {
                        continue;
                    }
                    object->Kill();
                }
                playerLives = 3;
                playerScore = 0;
                level = 1;
                enemyCount = 0;
                currWave = 1;
                wavePending = false;
                respawnPending = false;
                respawnAtFrame = -1;
                attackGroupActive = false;
                attackPattern = 0;
                nextEnemyAttackFrame = 0;
                startUpPending = false;
                coinInserted = false;
                
                state = State::Waiting;
            }
            break;
        }
    }
}

void Galaga::RenderUI(CMPUT350::GameContext* context) {
     context->GUIContext->DrawText(
        std::to_string(playerScore),
        24,
        CMPUT350::Point2D(20, 20),
        CMPUT350::Colors::white);
    
    if (respawnPending && state != State::GameOver) {
            context->GUIContext->DrawCenteredText(
                "Lives: " + std::to_string(playerLives),
                24,
                CMPUT350::Point2D(screenWidth / 2.f, screenHeight / 2.f),
                CMPUT350::Colors::white
            );
        }

    switch (state) {
        case (State::Keybinds): {
            context->GUIContext->DrawCenteredText(
                "Controls", 32, {screenWidth / 2.f, screenHeight * 0.25f}, CMPUT350::Colors::white);
            for (int i = 0; i < static_cast<int>(bindings.size()); i++) {
                context->GUIContext->DrawCenteredText(
                    bindings[i].label + ": " + KeyName(bindings[i].key), 26,
                    {screenWidth / 2.f, screenHeight * 0.35f + i * 60.f},
                    selectedBinding == i ? CMPUT350::Colors::yellow : CMPUT350::Colors::white);
            }
            context->GUIContext->DrawCenteredText(
                selectedBinding >= 0 ? "Press a replacement key. Escape cancels."
                                     : "Press a current control to change it. Escape: Back",
                14, {screenWidth / 2.f, screenHeight * 0.60f}, CMPUT350::Colors::white);
            context->GUIContext->DrawCenteredText(
                keybindMessage, 14, {screenWidth / 2.f, screenHeight * 0.66f}, CMPUT350::Colors::white);
            break;
        }
        case (State::Waiting): {
            context->GUIContext->DrawCenteredText(
                "Press K to configure controls", 20,
                {screenWidth / 2.f, screenHeight / 2.f + 120.f}, CMPUT350::Colors::white);
            context->GUIContext->DrawCenteredText(
                "Press 5 to Insert a Coin",
                24,
                CMPUT350::Point2D(screenWidth / 2.f, screenHeight / 2.f - 100.0f),
                CMPUT350::Colors::white
            );
            if (coinInserted) {
                context->GUIContext->DrawCenteredText(
                    "Press 1 to Start the Game",
                    24,
                    CMPUT350::Point2D(screenWidth / 2.f, screenHeight / 2.f + 50.0f),
                    CMPUT350::Colors::white
                );
            }
            break;
        }
        case (State::Startup): {
            context->GUIContext->DrawCenteredText(
                "Level " + std::to_string(level),
                24,
                CMPUT350::Point2D(screenWidth / 2.f, screenHeight / 2.f),
                CMPUT350::Colors::white
            );
            break;
        }
        case (State::Waves): {
            break;
        }
        case (State::Gameplay): {
            break;
        }
        case (State::GameOver): {
            context->GUIContext->DrawCenteredText(
                "Game Over!",
                24,
                CMPUT350::Point2D(screenWidth / 2.f, screenHeight / 2.f),
                CMPUT350::Colors::white
            );
            break;
        }
    }
}

void Galaga::ReceiveNotification(const std::string& key) {
    if (key.starts_with("KeyDown:")) {
        HandleKey(std::stoi(key.substr(8)), true);
    } else if (key.starts_with("KeyUp:")) {
        HandleKey(std::stoi(key.substr(6)), false);
    } else if (key == "EnemyDestroyed:100") {
        if (state == State::Waiting || state == State::GameOver) return;
        enemyCount -= 1;
        playerScore += 100;
    } else if (key == "EnemyDestroyed:150") {
        if (state == State::Waiting || state == State::GameOver) return;
        enemyCount -= 1;
        playerScore += 150;
    } else if (key == "EnemyDestroyed:200") {
        if (state == State::Waiting || state == State::GameOver) return;
        enemyCount -= 1;
        playerScore += 200;
    } else if (key == "EnemyDestroyed:300") {
        if (state == State::Waiting || state == State::GameOver) return;
        enemyCount -= 1;
        playerScore += 300;
    } else if (key == "EnemyDestroyed:400") {
        if (state == State::Waiting || state == State::GameOver) return;
        enemyCount -= 1;
        playerScore += 400;
    } else if (key == "InsertCoin") {
        if (state == State::Waiting) {
            coinInserted = true;
        }
    } else if (key == "StartGame") {
        if (coinInserted && state == State::Waiting) {
            coinInserted = false;
            startUpPending = true;
            state = State::Startup;
        }
    } 
}
