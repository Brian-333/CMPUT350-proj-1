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
 * @brief Compares two local shapes after transforming them into global coordinates.
 *
 * When either shape is a line, CollisionEnter receives the directed crossing
 * point converted back into each object's local coordinates. Circle/rect pairs
 * report an overlap without a single collision point.
 *
 * @param shape1 The first shape in object1's local coordinates.
 * @param shape2 The second shape in object2's local coordinates.
 * @param object1 The collision object that owns shape1.
 * @param object2 The collision object that owns shape2.
 */
void GameEngine::ProcessShapeCollision(Shape shape1, Shape shape2, std::shared_ptr<CollisionObject> object1, std::shared_ptr<CollisionObject> object2) {
    // Shapes are local. Move them into global coordinates before comparing.
    DrawContext *screen = mGameContext.ScreenContext;
    auto use = [&](const std::shared_ptr<CollisionObject> &object) {
        screen->SetContextOffset(object->GetLocation());
        screen->SetContextRotation(object->GetRotation());
    };
    auto toGlobal = [&](const std::shared_ptr<CollisionObject> &object, Point2D point) {
        use(object);
        return screen->Transform(point);
    };
    auto toLocal = [&](const std::shared_ptr<CollisionObject> &object, Point2D point) {
        use(object);
        return screen->ReverseTransform(point);
    };
    auto cornersOf = [&](const std::shared_ptr<CollisionObject> &object, Rect rect, Point2D corners[4]) {
        corners[0] = toGlobal(object, rect.topLeft);
        corners[1] = toGlobal(object, Point2D(rect.topLeft.x + rect.width, rect.topLeft.y));
        corners[2] = toGlobal(object, Point2D(rect.topLeft.x + rect.width, rect.topLeft.y + rect.height));
        corners[3] = toGlobal(object, Point2D(rect.topLeft.x, rect.topLeft.y + rect.height));
    };
    // A line collision also returns the hit, converted back into each object's local coordinates.
    auto notifyPoint = [&](Point2D globalPoint) {
        object1->CollisionEnter(object2, toLocal(object1, globalPoint));
        object2->CollisionEnter(object1, toLocal(object2, globalPoint));
    };
    auto notify = [&]() {
        object1->CollisionEnter(object2);
        object2->CollisionEnter(object1);
    };

    switch (shape1.t) {
        case ShapeType::kLine: {
            Line line1(toGlobal(object1, shape1.shape.line.p1), toGlobal(object1, shape1.shape.line.p2));
            Point2D crossingPoint;
            switch (shape2.t) {
                case ShapeType::kLine: {
                    Line line2(toGlobal(object2, shape2.shape.line.p1), toGlobal(object2, shape2.shape.line.p2));
                    if (line1.Crosses(line2, crossingPoint)) {
                        object1->CollisionEnter(object2, toLocal(object1, crossingPoint));
                        line2.Crosses(line1, crossingPoint);
                        object2->CollisionEnter(object1, toLocal(object2, crossingPoint));
                    }
                    break;
                }
                case ShapeType::kCircle: {
                    Circle circle(toGlobal(object2, shape2.shape.circle.center), shape2.shape.circle.radius);
                    crossingPoint = line1.ClosestPoint(circle.center);
                    if (crossingPoint.Distance(circle.center) <= circle.radius) {
                        notifyPoint(crossingPoint);
                    }
                    break;
                }
                case ShapeType::kRect: {
                    Rect rect = shape2.shape.rect;
                    Point2D corners[4];
                    cornersOf(object2, rect, corners);
                    bool hit = false;
                    for (int i = 0; i < 4; i++) {
                        if (line1.Crosses(Line(corners[i], corners[(i + 1) % 4]), crossingPoint)) {
                            hit = true;
                            break;
                        }
                    }
                    if (!hit && rect.IsInside(toLocal(object2, line1.p1))) {
                        crossingPoint = line1.p1;
                        hit = true;
                    } else if (!hit && rect.IsInside(toLocal(object2, line1.p2))) {
                        crossingPoint = line1.p2;
                        hit = true;
                    }
                    if (hit) {
                        notifyPoint(crossingPoint);
                    }
                    break;
                }
            }
            break;
        }
        case ShapeType::kCircle: {
            switch (shape2.t) {
                case ShapeType::kCircle: {
                    Circle circle1(toGlobal(object1, shape1.shape.circle.center), shape1.shape.circle.radius);
                    Circle circle2(toGlobal(object2, shape2.shape.circle.center), shape2.shape.circle.radius);
                    if (circle1.center.Distance(circle2.center) <= circle1.radius + circle2.radius) {
                        notify();
                    }
                    break;
                }
                case ShapeType::kRect: {
                    Circle circle(toGlobal(object1, shape1.shape.circle.center), shape1.shape.circle.radius);
                    Rect rect = shape2.shape.rect;
                    Point2D localCenter = toLocal(object2, circle.center);
                    float closestX = std::clamp(localCenter.x, rect.topLeft.x, rect.topLeft.x + rect.width);
                    float closestY = std::clamp(localCenter.y, rect.topLeft.y, rect.topLeft.y + rect.height);
                    if (localCenter.Distance(Point2D(closestX, closestY)) <= circle.radius) {
                        notify();
                    }
                    break;
                }
                case ShapeType::kLine:
                    ProcessShapeCollision(shape2, shape1, object2, object1);
                    break;
            }
            break;
        }
        case ShapeType::kRect: {
            switch (shape2.t) {
                case ShapeType::kRect: {
                    Point2D a[4];
                    Point2D b[4];
                    Point2D unused;
                    cornersOf(object1, shape1.shape.rect, a);
                    cornersOf(object2, shape2.shape.rect, b);
                    bool hit = false;
                    for (int i = 0; i < 4 && !hit; i++) {
                        for (int j = 0; j < 4 && !hit; j++) {
                            hit = Line(a[i], a[(i + 1) % 4]).Crosses(Line(b[j], b[(j + 1) % 4]), unused);
                        }
                    }
                    for (int i = 0; i < 4 && !hit; i++) {
                        hit = shape2.shape.rect.IsInside(toLocal(object2, a[i])) || shape1.shape.rect.IsInside(toLocal(object1, b[i]));
                    }
                    if (hit) {
                        notify();
                    }
                    break;
                }
                case ShapeType::kLine:
                    ProcessShapeCollision(shape2, shape1, object2, object1);
                    break;
                case ShapeType::kCircle:
                    ProcessShapeCollision(shape2, shape1, object2, object1);
                    break;
            }
            break;
        }
        default:
            break;
    }
}

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

    mGameContext.ScreenContext->SetContextOffset(object2->GetLocation());
    mGameContext.ScreenContext->SetContextRotation(object2->GetRotation());
    bounds2 = mGameContext.ScreenContext->Transform(object2->GetBounds());

    // Check AABB collision
    if (!(bounds.topLeft.x + bounds.width < bounds2.topLeft.x ||
        bounds.topLeft.x > bounds2.topLeft.x + bounds2.width ||
        bounds.topLeft.y + bounds.height < bounds2.topLeft.y ||
        bounds.topLeft.y > bounds2.topLeft.y + bounds2.height))
        {
        // Check internal collision
        for (Shape shape1 : object1->GetShapes()) {
            for (Shape shape2 : object2->GetShapes()) {
                ProcessShapeCollision(shape1, shape2, object1, object2);
            }
        }
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
        // Draw AABB collision bounds
        // for (int i = 0; i < mGameObjects.size(); i++) {
        //     std::shared_ptr<CollisionObject> collisionObject = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
        //     if (!collisionObject) {
        //         continue;
        //     }
        //     mGameContext.ScreenContext->SetContextOffset(collisionObject->GetLocation());
        //     mGameContext.ScreenContext->SetContextRotation(collisionObject->GetRotation());
        //     Rect bounds1 = mGameContext.ScreenContext->Transform(collisionObject->GetBounds());
        //     mGameContext.GUIContext->FrameRect(bounds1, 1, Colors::green);
        // }
        if (!gameRunning) {
            mGameContext.GUIContext->DrawCenteredText("Game Paused", 20, Point2D(mGameContext.GUIContext->GetWindowWidth() * 1.0f / 2, mGameContext.GUIContext->GetWindowHeight() * 1.0f / 2), Colors::white);
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
