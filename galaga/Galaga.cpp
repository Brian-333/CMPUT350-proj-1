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
                state = State::Waves;
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