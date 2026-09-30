#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include "DrawContext.h"
#include "EngineView.h"
#include "GameObject.h"

namespace CMPUT350 {

/**
 * @brief Holds shared engine state passed to game objects each frame.
 *
 * Provides access to the engine view for spawning objects and the draw
 * context for rendering.
 */
class GameContext {
public:
    EngineView *mEngineView;
    DrawContext *ScreenContext;
};

}  // namespace CMPUT350

#endif  // GAMECONTEXT_H
