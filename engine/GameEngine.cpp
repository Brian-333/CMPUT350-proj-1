#include "GameEngine.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

/**
 * @method GameEngine
 * @arguments width - the width of the game window in pixels
 *            height - the height of the game window in pixels
 *            name - the title displayed on the game window
 * @description Creates the SFML render window, limits the framerate to 30 FPS, loads the embedded
 * font, and sets up the game context (draw context and engine view) that is passed to game objects.
 */
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    // Create the SFML window
    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(width, height)), name);
    if (!mWindow) {
        std::cerr << "WARNING: Window did not create." << "\n";
        return;
    }
    // Set the framerate limit
    mWindow->setFramerateLimit(30);
    
    std::shared_ptr<sf::Font> font = std::make_shared<sf::Font>();
    // Load font from memory
    if (!font->openFromMemory(&_font, _font_len)) {
        std::cerr << "WARNING: Font did not load." << "\n";
    }
    mGameContext.ScreenContext = new DrawContext(mWindow, font);
    mGameContext.mEngineView = this;
}

/**
 * @method ~GameEngine
 * @arguments None
 * @description Kills all active game objects, removes them from the engine, and closes the game window.
 */
GameEngine::~GameEngine() {
    // Cleanup resources
    // Clean up game objects
    delete mGameContext.ScreenContext;
    for (auto gameObject : mGameObjects) {
        gameObject->Kill();
    }
    mGameObjects.clear();
    mWindow->close();
}

/**
 * @method AddGameObject
 * @arguments gameObject - the game object to add to the engine
 * @description Queues a game object to be added to the engine. The object is initialized and becomes
 * active at the start of the next frame.
 */
void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mGameObjectsToAdd.emplace_back(gameObject);
}

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */
void GameEngine::Run() {
    while (mWindow->isOpen())  // window is open
    {
        // 0. Remove any objects that are now dead
        mGameObjects.erase(
            std::remove_if(
                mGameObjects.begin(), 
                mGameObjects.end(), 
                // Lambda function to remove any objects that are not alive
                [](std::shared_ptr<GameObject> gameObject) { return !gameObject->IsAlive(); }
            ),
            mGameObjects.end()
        );

        // 1. Activate and initialize any objects added during the last frame
        while (!mGameObjectsToAdd.empty()) {
            std::shared_ptr<GameObject> gameObject = mGameObjectsToAdd.front();
            mGameObjectsToAdd.erase(mGameObjectsToAdd.begin());
            gameObject->Initialize(&mGameContext);
            mGameObjects.emplace_back(gameObject);
        }

        // 2. Process events
        while (const std::optional event = mWindow->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mWindow->close();
            }
            else if (event->is<sf::Event::Resized>()) {
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>()) {
                if (keyPressed->unicode == 'p') {
                    gameRunning = !gameRunning;
                }
                for (auto gameObject : mGameObjects) {
                    gameObject->HandleKeyEvent(&mGameContext, keyPressed->unicode);
                }
            }
        }

        // 3. Update game objects
        if (gameRunning) {
            for (auto gameObject : mGameObjects) {
                gameObject->Update(&mGameContext);
            }
        }


        // 4. Process collision events
        if (gameRunning) {
            for (int i = 0; i < mGameObjects.size(); i++) {
                std::shared_ptr<CollisionObject> collisionObject = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
                if (!collisionObject) {
                    continue;
                }
                for (int j = i + 1; j < mGameObjects.size(); j++) {
                    std::shared_ptr<CollisionObject> collisionObject2 = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[j]);
                    if (collisionObject2) {
                        Rect bounds = collisionObject->GetBounds();
                        bounds &= collisionObject2->GetBounds();
                        if (bounds.width > 0 && bounds.height > 0) {
                            collisionObject->CollisionEnter(collisionObject2);
                            collisionObject2->CollisionEnter(collisionObject);
                        }
                    }
                }
            }
        }

        // 5. Late updates
        if (gameRunning) {
            for (auto gameObject : mGameObjects) {
                gameObject->LateUpdate(&mGameContext);
            }
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (auto gameObject : mGameObjects) {
            std::shared_ptr<GraphicsObject> graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject) {
                graphicsObject->RenderBackground(&mGameContext);
            }
        }

        // 7. Render foreground
        for (auto gameObject : mGameObjects) {
            std::shared_ptr<GraphicsObject> graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject) {
                graphicsObject->RenderForeground(&mGameContext);
            }
        }
        // Draw collision bounds
        // for (int i = 0; i < mGameObjects.size(); i++) {
        //     std::shared_ptr<CollisionObject> collisionObject = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
        //     if (!collisionObject) {
        //         continue;
        //     }
        //     for (int j = i + 1; j < mGameObjects.size(); j++) {
        //         std::shared_ptr<CollisionObject> collisionObject2 = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[j]);
        //         if (collisionObject2) {
        //             Rect bounds = collisionObject->GetBounds();
        //             mGameContext.ScreenContext->FrameRect(collisionObject->GetBounds(), 1, Colors::green);
        //             bounds &= collisionObject2->GetBounds();
        //             mGameContext.ScreenContext->FrameRect(collisionObject2->GetBounds(), 1, Colors::green);
        //         }
        //     }
        // }
        if (!gameRunning) {
            mGameContext.ScreenContext->DrawCenteredText("Game Paused", 20, Point2D(mGameContext.ScreenContext->GetWindowWidth() / 2, mGameContext.ScreenContext->GetWindowHeight() / 2), Colors::white);
        }

        // Actually render to window
        mWindow->display();
    }
}

// Sample code for processing events

// bool GameEngine::ProcessEvents(GameContext *context)
// {
	
// }

}  // namespace CMPUT350
