#include "GameEngine.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    // Create the SFML window
    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(width, height)), name);
    if (!mWindow) {
        std::cerr << "WARNING: Window did not create." << "\n";
        return;
    }
    // Set the framerate limit and prevent key repeats
    mWindow->setFramerateLimit(60);
    std::shared_ptr<sf::Font> font = std::make_shared<sf::Font>();
    // Load font from memory
    if (!font->openFromMemory(&_font, _font_len)) {
        std::cerr << "WARNING: Font did not load." << "\n";
    }
    mGameContext.ScreenContext = new DrawContext(mWindow, font);
    mGameContext.mEngineView = this;
}

GameEngine::~GameEngine() {
    // Cleanup resources
    // Clean up game objects
    for (auto gameObject : mGameObjects) {
        gameObject->Kill();
    }
    mGameObjects.clear();
    mWindow->close();
}

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
                for (auto gameObject : mGameObjects) {
                    gameObject->HandleKeyEvent(&mGameContext, keyPressed->unicode);
                }
            }
        }

        // 3. Update game objects
        for (auto gameObject : mGameObjects) {
            gameObject->Update(&mGameContext);
        }


        // 4. Process collision events
        for (int i = 0; i < mGameObjects.size(); i++) {
            std::shared_ptr<CollisionObject> collisionObject = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
            if (!collisionObject) {
                continue;
            }
            for (int j = i + 1; j < mGameObjects.size(); j++) {
                std::shared_ptr<CollisionObject> collisionObject2 = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[j]);
                if (collisionObject2) {
                    collisionObject->CollisionEnter(collisionObject2);
                }
            }
        }

        // 5. Late updates
        for (auto gameObject : mGameObjects) {
            gameObject->LateUpdate(&mGameContext);
        }

        // Clear window
        mWindow->clear();

        for (auto gameObject : mGameObjects) {
            std::shared_ptr<GraphicsObject> graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject) {
                // 6. Render background
                graphicsObject->RenderBackground(&mGameContext);
                // 7. Render foreground
                graphicsObject->RenderForeground(&mGameContext);
            }
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
