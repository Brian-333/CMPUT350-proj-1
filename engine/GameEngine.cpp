#include "GameEngine.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

/**
 * @brief Constructs the game engine and initializes the window, font, and game context.
 *
 * @param width The width of the game window in pixels.
 * @param height The height of the game window in pixels.
 * @param name The title displayed on the game window.
 *
 * Creates the SFML render window, limits the framerate to 30 FPS, loads the embedded
 * font, and sets up the game context (draw context and engine view) that is passed to game objects.
 */
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    mGameContext.ScreenContext = nullptr;
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
    mGameContext.GUIContext = new DrawContext(mWindow, font);
    mGameContext.mNotificationManager = new NotificationManager();
    mGameContext.mEngineView = this;
}

/**
 * @brief Destroys the game engine and releases owned resources.
 *
 * Kills all active game objects, removes them from the engine, and closes the game window.
 */
GameEngine::~GameEngine() {
    // Cleanup resources
    // Clean up game objects
    if (mGameContext.ScreenContext) {
        delete mGameContext.ScreenContext;
    }
    if (mGameContext.GUIContext) {
        delete mGameContext.GUIContext;
    }
    if (mGameContext.mNotificationManager) {
        delete mGameContext.mNotificationManager;
    }
    for (auto gameObject : mGameObjects) {
        gameObject->Kill();
    }
    mGameObjects.clear();
    mWindow->close();
}

/**
 * @brief Installs a keydown notification for a given key.
 * 
 * Notification will be overridden if a new notification is installed for the same key.
 *
 * @param key The key to install the notification for.
 * @param notification The notification to install.
 */
void GameEngine::InstallKeyDownNotification(int key, const std::string &notification) {
    if (key == Key::P) {
        throw std::runtime_error("Key P is reserved for pausing the game");
    }
    mKeybindsDown[key] = notification;
}

/**
 * @brief Installs a keyup notification for a given key.
 *
 * Notification will be overridden if a new notification is installed for the same key.
 *
 * @param key The key to install the notification for.
 * @param notification The notification to install.
 */
void GameEngine::InstallKeyUpNotification(int key, const std::string &notification) {
    if (key == Key::P) {
        throw std::runtime_error("Key P is reserved for pausing the game");
    }
    mKeybindsUp[key] = notification;
}

/**
 * @brief Queues a game object to be added to the engine.
 *
 * @param gameObject The game object to add to the engine.
 *
 * The object is initialized and becomes active at the start of the next frame.
 */
void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mGameObjectsToAdd.emplace_back(gameObject);
}

/**
 * @brief Returns a const iterator to the beginning of the game objects vector.
 *
 * @return A const iterator to the beginning of the game objects vector.
 */
GameEngine::const_iterator GameEngine::cbegin() const { return mGameObjects.cbegin(); }
GameEngine::const_iterator GameEngine::cend() const { return mGameObjects.cend(); }
GameEngine::const_iterator GameEngine::begin() const { return mGameObjects.begin(); }
GameEngine::const_iterator GameEngine::end() const { return mGameObjects.end(); }

/**
 * @brief Processes an internal collision between two collision objects.
 *
 * This function is used to process collisions between two collision objects.
 *
 * @param object1 The first collision object.
 * @param object2 The second collision object.
 */
void GameEngine::ProcessCollision(std::shared_ptr<CollisionObject> object1, std::shared_ptr<CollisionObject> object2) {
    static Rect bounds;
    static Rect bounds2;
    mGameContext.ScreenContext->SetContextOffset(object1->GetLocation());
    mGameContext.ScreenContext->SetContextRotation(object1->GetRotation());
    bounds = mGameContext.ScreenContext->Transform(object1->GetBounds());
    // mGameContext.GUIContext->FrameRect(bounds, 5, Colors::green);

    mGameContext.ScreenContext->SetContextOffset(object2->GetLocation());
    mGameContext.ScreenContext->SetContextRotation(object2->GetRotation());
    bounds2 = mGameContext.ScreenContext->Transform(object2->GetBounds());
    // mGameContext.GUIContext->FrameRect(bounds2, 5, Colors::green);
    bounds &= bounds2;

    if (bounds.width > 0 && bounds.height > 0) {
        object1->CollisionEnter(object2);
        object2->CollisionEnter(object1);
    }
}

/**
 * @brief Runs the main game loop until the window is closed.
 *
 * Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */
void GameEngine::Run() {
    // Reset the current frame counter
    mGameContext.currentFrame = 0;

    while (mWindow->isOpen())  // window is open
    {
        // Increment the current frame counter
        mGameContext.currentFrame++;
        
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
            mGameContext.CurrObject = gameObject;
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
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                static int currentKey;
                currentKey = static_cast<int>(keyPressed->scancode);
                if (currentKey == Key::P) {
                    gameRunning = !gameRunning;
                }
                if (mKeybindsDown.contains(currentKey)) {
                    mGameContext.mNotificationManager->Notify(mKeybindsDown[currentKey]);
                }
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                static int currentKey;
                currentKey = static_cast<int>(keyReleased->scancode);
                if (mKeybindsUp.contains(currentKey)) {
                    mGameContext.mNotificationManager->Notify(mKeybindsUp[currentKey]);
                }
            }
        }

        // 3. Update game objects
        if (gameRunning) {
            for (auto gameObject : mGameObjects) {
                mGameContext.CurrObject = gameObject;
                gameObject->Update(&mGameContext);
            }
        }


        // 4. Process collision events
        if (gameRunning) {
            std::vector<std::shared_ptr<CollisionObject>> collisionObjects;
            for (auto gameObject : mGameObjects) {
                std::shared_ptr<CollisionObject> collisionObject = std::dynamic_pointer_cast<CollisionObject>(gameObject);
                if (collisionObject) {
                    collisionObjects.push_back(collisionObject);
                }
            }
            for (int i = 0; i < collisionObjects.size(); i++) {
                for (int j = i + 1; j < collisionObjects.size(); j++) {
                    ProcessCollision(collisionObjects[i], collisionObjects[j]);
                }
            }
        }

        // 5. Late updates
        if (gameRunning) {
            for (auto gameObject : mGameObjects) {
                mGameContext.CurrObject = gameObject;
                gameObject->LateUpdate(&mGameContext);
            }
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (auto gameObject : mGameObjects) {
            std::shared_ptr<GraphicsObject> graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject) {
                mGameContext.CurrObject = gameObject;
                mGameContext.ScreenContext->SetContextOffset(graphicsObject->GetLocation());
                mGameContext.ScreenContext->SetContextRotation(graphicsObject->GetRotation());
                graphicsObject->RenderBackground(&mGameContext);
            }
        }

        // 7. Render foreground
        for (auto gameObject : mGameObjects) {
            std::shared_ptr<GraphicsObject> graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject) {
                mGameContext.CurrObject = gameObject;
                mGameContext.ScreenContext->SetContextOffset(graphicsObject->GetLocation());
                mGameContext.ScreenContext->SetContextRotation(graphicsObject->GetRotation());
                graphicsObject->RenderForeground(&mGameContext);
            }
        }

        // 8. Render GUI
        for (auto gameObject : mGameObjects) {
            mGameContext.CurrObject = gameObject;
            gameObject->RenderUI(&mGameContext);
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
            mGameContext.ScreenContext->DrawCenteredText("Game Paused", 20, Point2D(mGameContext.ScreenContext->GetWindowWidth() * 1.0f / 2, mGameContext.ScreenContext->GetWindowHeight() * 1.0f / 2), Colors::white);
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
