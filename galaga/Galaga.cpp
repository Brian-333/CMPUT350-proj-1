#include "Enemy.h"
#include "Galaga.h"
#include "Player.h"
#include "GameContext.h"
#include "Keyboard.h"
#include <iostream>

Galaga::Galaga(int width, int height): screenWidth(width), screenHeight(height) 
{
    playerSpawn = CMPUT350::Point2D(screenWidth / 2.f, screenHeight * 0.6f);
}

void Galaga::Initialize(CMPUT350::GameContext* context) {
    auto* notifications = context->mNotificationManager;
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num5, "InsertCoin");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num1, "StartGame");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::A, "MoveLeftDown");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::D, "MoveRightDown");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::A, "MoveLeftUp");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::D, "MoveRightUp");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Space, "Fire");

    notifications->Register(context->CurrObject, "InsertCoin");
    notifications->Register(context->CurrObject, "StartGame");
    notifications->Register(context->CurrObject, "EnemyDestroyed:100");
    notifications->Register(context->CurrObject, "EnemyDestroyed:150");
    notifications->Register(context->CurrObject, "EnemyDestroyed:200");
    notifications->Register(context->CurrObject, "EnemyDestroyed:300");
    notifications->Register(context->CurrObject, "EnemyDestroyed:400");
}

void Galaga::Update(CMPUT350::GameContext* context) {
    // AI-assisted: count deaths in both active phases, even if the engine
    // still holds the killed player until its next removal pass.
    if ((state == State::Waves || state == State::Gameplay) && !respawnPending) {
        auto currentPlayer = player.lock();
        if (!currentPlayer || !currentPlayer->IsAlive()) {
            respawnPending = true;
            playerLives -= 1;
            if (playerLives <= 0) {
                returnToHomeFrame = context->currentFrame + 150;
                state = State::GameOver;
                return;
            }
        }
    }

    switch (state) {
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
            if (enemyCount == 0) {  // progress to next level
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

            if (respawnPending && !enemiesAttacking) {  // only respawn player when enemies are done attacking
                auto newPlayer = std::make_shared<Player>(playerSpawn);
                player = newPlayer;
                context->mEngineView->AddGameObject(newPlayer);
                respawnPending = false;
                nextEnemyAttackFrame = context->currentFrame + enemyAttackPauseFrames;
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
                    if (galaga) {
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
        case (State::Waiting): {
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
    if (key == "EnemyDestroyed:100") {
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
