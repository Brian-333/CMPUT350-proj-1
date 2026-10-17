# CMPUT 350 Project 1b


## Overview and controls

Galaga is started by adding one `Galaga` object to the engine. It creates the player and five rows of eight enemies, tracks score and lives, selects attack groups, and starts another level after all enemies are destroyed. Enemy1 follows a figure eight, Enemy2 follows a deep oval, and Enemy3 follows a descending and ascending wave. Enemy types have different colors, health, and firing settings; surviving damage darkens their color.

Press **5** to insert a coin, then **1** to start. Default controls are **A/D** for movement and **Space** to fire. **P** pauses the engine. **K** opens keyboard setup from the main screen. Timing uses `currentFrame` with the engine capped at 30 frames per second.

## Game state flow

`Galaga` keeps its state in a private `enum class State`:

- `Waiting`: show the main screen and accept coin/start input; K opens `Keybinds`.
- `Keybinds`: select an action by its current key and assign a replacement; Escape returns to `Waiting`.
- `Startup`: create the player if needed and display the level for 90 frames, then enter `Waves`.
- `Waves`: create five rows of eight enemies with predefined entrance paths. Rows are scheduled 120 frames apart. After the fifth row, wait until no living enemy is entering before switching to `Gameplay`.
- `Gameplay`: cycle through four deterministic attack selections using the engine iterator: first three; every other ship starting at the third (up to four); first two and last two; then six starting at the fifth with wraparound. Wait 45 frames after a group finishes before starting another. Clearing the level returns to `Startup` with an increased level number.
- `GameOver`: show the message for 150 frames, then kill remaining game objects and reset score, lives, level, and wave/attack bookkeeping before returning to `Waiting`. Keybindings persist for the current run.

Player death is checked during both `Waves` and `Gameplay`. A death deducts one life and marks a respawn pending. The replacement waits until wave entry and active attacks finish, then waits another 60 frames. Losing the final life enters `GameOver`.

Enemies have a separate protected `enum class State`: `Spawning` follows the entrance path; `Idle` hovers in formation; `Attacking` follows its attack/return path and fires. Returning to formation restores `Idle`. These enums describe game behavior and do not add game-specific states to the engine API.

## Interfaces and API decisions

- `GraphicsObject::GetLocation()` returns the world position of the local origin, and `GetRotation()` returns radians. Rendered shapes, collision components, and `GetBounds()` use local coordinates. The engine applies each object's transform; `GUIContext` stays in screen coordinates.
- `CollisionObject::GetShapes()` exposes line, circle, and rectangle components. The point overload of `CollisionEnter` receives an impact in the receiving object's local coordinates; its default implementation forwards to the overload without a point. Player and Enemy use that default because their damage logic only needs the other object. Bullet overrides both overloads and keeps the largest local y among valid impacts that frame.
- `GameContext` provides `ScreenContext`, `GUIContext`, `mEngineView`, `mNotificationManager`, the weak `CurrObject`, and `currentFrame`. Objects use `CurrObject` to register themselves or pass a weak parent to bullets. Bullets are removed when their parent is gone or dead.
- `EngineView` exposes `begin/end/cbegin/cend` as const iterators over active objects. `AddGameObject` queues objects for initialization at the next frame's activation step. Keyboard input is registered through `InstallKeyDownNotification` and `InstallKeyUpNotification`.
- The engine currently stores one notification per scancode per direction; installing another replaces it. This differs from the specification's list of notifications per key. P is reserved by the engine for pause. Galaga's keybinding translation is described below.
- `NotificationManager` registers weak object references and delivers string messages immediately. Current behavior throws for `Notify` or `Unregister` on a key that has never been registered. Expired listeners are removed when that message is sent. Galaga registers raw key messages, Player registers movement/fire actions, and enemies report score messages (`100`, `150`, `200`, `300`, `400`) which Galaga also uses to decrement its enemy count.
- `Bezier` supports instance and static `GetPoint/GetSlope` calls. A path contains `3*k + 1` control points, and its parameter ranges from 0 through k. Invalid point counts and out-of-range parameters throw exceptions when evaluated.
- `Enemy::Attack` remains pure virtual. Each enemy type supplies its path and firing setup. Added public queries `IsEntering()` and `IsAttacking()` let Galaga check readiness without exposing the state enum. Protected `SetPath` rebuilds movement samples and resets progress; private `AdvancePath` handles shared movement. Constructors receive a formation position and entrance path.
- `GameObject::IsStatic()` is an additional virtual query returning false by default; the current engine does not use it to skip collision checks.

## Team contributions

- Brian Lin handled most of the game engine implementation.
- Damon Grant handled the Galaga game implementation.

The AI usage log below records assistance separately from these team responsibilities.

## Extra credit

Implemented: keyboard setup at the main screen, with gameplay action notifications unchanged after remapping. Replay, ship capture/double ships, and the `Vector2D` alias are not implemented.

## Design Choices

### Star background

Galaga creates one 200-star background during initialization. Stars use a zero world offset and rotation and render in the background pass, behind ships and the UI. The existing downward scrolling and color/twinkle behavior is retained. The same background persists through levels and game-over resets.

### Lives display

We chose to display the player's remaining lives while the player is waiting to respawn.
After wave entry or attacking enemies finish, a 60-frame (two-second) delay keeps this message visible before the replacement player spawns.


### Keybinding

Press **K** at the main screen to configure controls. Press an action's current key to select it, then press its replacement. **Escape** cancels a pending change or returns to the main screen. Duplicate bindings are rejected, and K, Escape, 1, 5, and P remain reserved. Changes persist across games during the current run.

Rebinding is handled entirely by Galaga using the engine's existing notification API. Galaga installs key-down/key-up messages for all scancodes except the engine-reserved pause key P, then translates those messages into the currently assigned gameplay actions. Changing a binding only updates Galaga's action-to-key table; no engine mappings are removed. For example, after changing Move Left from A to Left Arrow, A still generates a key notification but matches no action, while Left Arrow forwards the existing MoveLeftDown/MoveLeftUp messages to the player. The player code does not need to change. This implements the keyboard setup screen extra credit.

### Game Engine

std::unordered_map<int, std::string> mKeybindsDown;  
std::unordered_map<int, std::string> mKeybindsUp;  
Two separate unordered_map were used to maintain keybinding of the physical key number to the message requested from the game.

### MathUtil.h

When two line segments overlap at multiple points, `Line::Crosses` returns the first shared point along the first segment.

### Enemy path movement

`Enemy::SetPath` samples each cubic segment 100 times and stores cumulative distances. `AdvancePath` advances 9 pixels per frame, looks up the corresponding Bezier parameter, and updates position and rotation. Speed is approximate because distances are sampled. Call `SetPath` whenever assigning an entrance, attack, or return path so the table and travelled distance reset together.

## Build and verification

Build with CMake using C++20 (`cmake -S . -B build`, then `cmake --build build`). The local macOS build passed during the rubric review. A build and gameplay check on the course lab machines is still required. See `RUBRIC_REVIEW.md` for the current review findings; a successful build does not establish full rubric compliance.

The rubric requests `project.txt`, while the earlier documentation instructions mention `README.md`. `project.txt` contains a copy of this README for submission; keep the two files synchronized when editing documentation.

## Library Versions

SFML - version 3.1.0

## Resources

### Damon Grant's AI reflection

I found it very useful for getting the point calculations done for the Bezier curves, as well as implementing the different attack pattern ideas that I had. In order to save time, I tried to always do the first implementation by myself, and then after I got it working, I would get the AI to mirror my logic for the remaining cases. It was also very helpful for debugging and quick last-minute fixes. I still feel like I got to do a lot by hand and that the AI only skipped some of the tedious calculations for me.

### AI Usage

- Codex prompt: "Ok put that in" (adding the existing Stars background).
    - AI added Stars' location/rotation overrides, queued one background from Galaga::Initialize, and excluded Stars from game-over cleanup. The existing star drawing and scrolling behavior was retained. The local CMake build passed; visual review and the team's reflection remain pending.

- Codex prompt: "Fix the collision points"
    - AI corrected directed line/circle impacts by solving the segment-circle quadratic for the first hit, and line/rectangle impacts by selecting the nearest edge intersection to the line's start. A start already inside either shape is the first impact, including zero-length lines. Existing local-coordinate callbacks and rotated rectangle support are retained. Changed sections are marked AI-assisted in GameEngine.cpp. A headless check using the current collision method and coordinate transforms passed 21 cases covering reversed directions, tangency, misses, endpoints, inside starts, zero-length lines, rotated/translated objects, and reversed shape ordering. The local CMake build passed; visual gameplay review and the team's reflection remain pending.

- Codex prompt: "Ok, go over the rubric and check our work. I know we probably have to update the readme with additional api changes we made such as having the state enums etc.."
    - AI compared the assignment with the source, checked the local build and selected headless checks, and wrote the overview, state flow, interface/design notes, extra-credit summary, and rubric review. AI corrected stale wave-progression documentation and copied the README to the rubric-requested `project.txt`. No gameplay or engine implementation was changed during this review. The team supplied the contribution breakdown; the team's own AI reflections still need to be completed.


- Codex prompts: "Ok, can you finish up the keybind implementation that I started" and "I undid it, can you only change the galaga and nothing in the game engine? Also we dont need to be able to remove keybinds, keep it simple. We can just select the keybind we want to change by pressing that keybind rather than scrolling to it. Would that simplify the implementation?"
    - The student supplied the Keybinds state, initial screen work, and the requirement to select actions by their current key without changing the engine or removing mappings. AI completed the implementation in Galaga.h/.cpp: raw-key notification registration, the action-to-key lookup, menu rendering, replacement/conflict handling, key labels, and suppression of repeated key-down events. The earlier engine-based implementation was undone; the final version leaves engine and player code unchanged. Headless checks passed for selection, replacement, cancellation, reserved/duplicate keys, unchanged engine mappings, player movement/release/fire, and persistence across games. The full CMake build passed. Visual gameplay review and the team's reflection remain to be completed.

- Codex prompt: "Add a respawn delay for me"
    - AI added a 60-frame respawn countdown that starts after wave entry and attacking enemies finish. Lives remain visible during the wait, and a simultaneous level clear waits for the respawn before starting the next level. Headless checks passed for timing, single life deductions/spawns, post-respawn attack grace, wave deaths, simultaneous level clear/death, and final-life game over.

- Codex prompt: "Theres a bug right now where if player dies during wave phase, the lives are not subtracted"
    - AI centralized death detection for Waves and Gameplay, deducting a life once and keeping respawns pending until entrances finish. Removed the wave-specific flag assignment that bypassed life deduction. Final-life deaths enter GameOver immediately, and spawning during Startup clears a pending respawn. Headless checks passed for wave deaths, single deductions, delayed respawning, final-life game over, and next-level spawning. The full CMake build passed.

- Codex prompt: "for the attacking, lets do 4 cases. I want the first case to be what we have, just the first 3 enemies. Then the next case, i want it to do every other enemy (max of 4) starting at the 3rd enemy. Then I want the first 2 and last 2. And for the last case start at maybe enemy 5 and grab 6 enemies in a row"
    - The student supplied the four patterns. AI implemented a repeating switch over living enemies in engine order: first three; third/fifth/seventh/ninth; first two plus last two; and six starting at fifth with wraparound. Groups never select the same enemy twice, and selection resets each level/game. Existing pauses between groups are preserved. Headless checks passed for all patterns with 1-10 living enemies, dead-object filtering, wraparound, and pattern cycling. Full gameplay review remains to be completed.

- Codex prompt: "The 3rd enemy does not do its little sine wave on the way back up"
    - AI replaced Enemy3's straight ascent with four cubic half-waves (two cycles), using the existing wave width and mirroring. The final return to formation remains a separate curve. Headless checks passed for oscillation during ascent on both sides and arrival at the formation slot; visual gameplay review remains to be completed.

- Codex prompt: "before we do that, enemy2 is only firing 2 shots for some reason and its circle is too small, i would like it to loop further down the screen, also enemy attacks start immediately after they are finished if the player is alive and i would like that to be slightly offset"
    - AI enlarged Enemy2's loop into a deeper oval and reduced its initial firing delay to one cooldown. AI also added a 45-frame pause measured from attack-group completion, with the same grace period after wave entry and player respawning. Headless checks confirmed eight shots from Enemy2 at three formation heights and a full pause after a long attack. Visual gameplay review remains to be completed.

- Codex prompt: "Ok so the enemies dont really spawn in uniformly and they just sort of snap into place when they are done spawning, also they are way too slow for their movement"
    - AI increased path speed from 3 to 9 pixels/frame, shortened row-spawn intervals from 360 to 120 frames, and added a gradual transition into formation hovering and rotation. A headless check passed for row spacing and smooth arrival; visual gameplay review remains to be completed.

- Galaga wave spawning (`Galaga::Update`, `State::Waves`): I supplied the wave ideas, enemy groupings, entrance directions, and the existing switch/for-loop structure. Codex implemented the actual spawning and Bezier control-point calculations, formation positions, offscreen starting distances, and expansion to eight enemies per row. AI also added initial frame-based wave timing. The current implementation advances through all five rows and waits for entry to finish before gameplay. The wave case is marked with an AI-assisted comment.
    - Prompts requested: a synchronized left-side entrance for the first row; normal middle ships followed by heavy edge ships in row 2; bottom-corner entrances through the player for row 3; normal ships split between bottom-right and left entrances for row 4; and normal outer ships, crossing special ships, and central heavy ships for row 5.
    - Follow-up prompts: "instead of doing seconds, just spawn the outer ones farther away so they take longer to show up, that should simplify things a lot yea?" and "oh shit i forgot its 8 per row, take some liberty and fix the remaining 4 rows, i dont care which enemies you use, you can pair them with already existing entrances".
    - Validation: headless C++ checks passed for five waves of eight enemies, expected types, 40 total enemies, and all entrance paths reaching distinct formation slots. Earlier checks covered entrance ordering and player-position crossings. Full visual gameplay review and the team's reflection remain to be completed.

- Codex prompt: "implement these offsets for me and add to the readme" (following the request for Enemy3 to descend in a sine wave angled toward the center, cross below the screen, rise on the opposite side, and return to formation).
    - AI implemented `Enemy3::Attack`: four cubic half-waves approximate two sine-wave cycles with inward drift; three further segments cross offscreen, rise, and return home. Dimensions scale with the screen, and left/right starts mirror the pattern. The method also checks readiness and resets shooting for each attack. The implementation is marked AI-assisted.
    - Validation: headless C++ checks passed for left, right, and center starts, readiness checks, continuous movement, crossing below the screen, opposite-side re-entry, the formation endpoint, and repeated attacks. Rendering and full gameplay were not tested; the test supplied a placeholder for the unfinished Enemy3 renderer. Team review and reflection remain to be completed.

- Codex prompt: "for the basic enemy I am imagining just a figure 8, how can I derive my points from that?"
    - AI suggested Bezier control-point offsets for a figure-eight enemy attack pattern and explained how to translate the offsets into world coordinates and mirror them horizontally. This assistance covered pattern offset calculations; gameplay validation and the team's reflection on the resulting pattern remain to be completed.

- Codex prompt: "enemy 2 i just want it to do a circle such that its starting point would be considered the side of the circle, so it would swoop down towards the middle of the screen"
    - AI suggested offsets for four cubic Bezier segments approximating a circle, using the quarter-circle control-point factor `0.55228475`. AI also explained screen-relative sizing, translation from the enemy's starting position, and horizontal mirroring so the circle extends toward the screen center. Gameplay validation and the team's reflection on the resulting pattern remain to be completed.

- Codex prompt: "make this change for me, remember to update the ai usage in the readme. also keep the changes small and easy to follow" (referring to constant-speed enemy movement using a distance lookup table).
    - AI added `SetPath`, `AdvancePath`, the distance table and movement members in `Enemy.h/.cpp`, and replaced the three fixed-parameter movement blocks with calls to `AdvancePath`. AI-assisted sections are marked in the code.
    - Validation: a headless C++ check using the actual movement implementation passed for uneven straight and multi-segment paths, curved paths, endpoints, replacing a path, zero-length paths, and invalid paths. Straight-path speed differed from 3 pixels/frame by at most 0.020 pixels in this check. Rendering and full gameplay were not tested. Team review and reflection remain to be completed.

- Codex prompt: "fix them" (referring to `Bezier::GetPoint` and `Bezier::GetSlope` after reviewing their missing support for connected cubic segments).
    - AI updated both methods to select a cubic segment, handle the final endpoint, and reject invalid input. AI also added the segment helper and header includes/guard. Changed implementation sections are marked in `Bezier.cpp`.
    - Validation: a standalone C++ check passed for single and multiple segments, derivatives, shared/final endpoints, and invalid inputs. Team review and reflection on these changes remain to be completed.

- Cursor Prompt: @DrawContext.cpp (65-76) Using this format of docstring, update all function docstrings in @engine folder
    - This was helpful for documentation and it uses the requested format of docstring. 

## Memory

- Valgrind and leaks report some leak but looked it up, it's some noise produced by the graphics renderer.
