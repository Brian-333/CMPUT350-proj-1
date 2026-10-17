#include "Enemy.h"
#include "Galaga.h"
#include "Player.h"
#include "GameContext.h"
#include "Keyboard.h"
#include <iostream>

Galaga::Galaga(int width, int height): screenWidth(width), screenHeight(height) 
{
    playerSpawn = CMPUT350::Point2D(screenWidth / 2.f, screenHeight * 0.75f);
}

void Galaga::Initialize(CMPUT350::GameContext* context) {
    auto* notifications = context->mNotificationManager;
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num5, "InsertCoin");
    notifications->Register(context->CurrObject, "InsertCoin");
    notifications->Register(context->CurrObject, "StartGame");
    notifications->Register(context->CurrObject, "EnemyDestroyed:100");
    notifications->Register(context->CurrObject, "EnemyDestroyed:150");
    notifications->Register(context->CurrObject, "EnemyDestroyed:200");
    notifications->Register(context->CurrObject, "EnemyDestroyed:300");
    notifications->Register(context->CurrObject, "EnemyDestroyed:400");
}

void Galaga::Update(CMPUT350::GameContext* context) {
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
                        nextWaveFrame = context->currentFrame + 360;
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
                        nextWaveFrame = context->currentFrame + 360;
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
                        nextWaveFrame = context->currentFrame + 360;
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
                        nextWaveFrame = context->currentFrame + 360;
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
                        }
                    }
                }
            }
            break;
        }
        case (State::Gameplay): {
            if (player.expired() && !respawnPending) {  // player has just died
                respawnPending = true;
                playerLives -= 1;
                if (playerLives <= 0) {
                    returnToHomeFrame = context->currentFrame + 360;
                    state = State::GameOver;
                    break;
                }
            }
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

            if (respawnPending && !enemiesAttacking) {  // only respawn player when enemies are done attacking
                auto newPlayer = std::make_shared<Player>(playerSpawn);
                player = newPlayer;
                context->mEngineView->AddGameObject(newPlayer);
                respawnPending = false;
            }

            auto currPlayer = player.lock();
            if (!enemiesAttacking && currPlayer && 
                currPlayer->IsAlive() && !respawnPending) 
            {   // only have enemies attack when the player is alive and no other enemies are attacking


            }
            break;
        }
        case (State::GameOver): {
            if (context->currentFrame >= returnToHomeFrame) {
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
    }
}

void Galaga::ReceiveNotification(const std::string& key) {
    if (key == "EnemyDestroyed:100") {
        enemyCount -= 1;
        playerScore += 100;
    } else if (key == "EnemyDestroyed:150") {
        enemyCount -= 1;
        playerScore += 150;
    } else if (key == "EnemyDestroyed:200") {
        enemyCount -= 1;
        playerScore += 200;
    } else if (key == "EnemyDestroyed:300") {
        enemyCount -= 1;
        playerScore += 300;
    } else if (key == "EnemyDestroyed:400") {
        enemyCount -= 1;
        playerScore += 400;
    } else if (key == "InsertCoin") {
        if (state == State::Waiting) {
            coinInserted = true;
            std::cout << "COIN INSERTED :D" << std::endl;
        }
    } else if (key == "StartGame") {
        if (coinInserted && state == State::Waiting) {
            coinInserted = false;
            startUpPending = true;
            state = State::Startup;
        }
    } 
}
