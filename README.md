# CMPUT 350 HW 1

## Design Choices


### Keybinding

Keys can have custom binding.  
The key "P" is reserved for pausing the game so it cannot be override to use for other functions.  

### Game Engine

std::unordered_map<int, std::string> mKeybindsDown;  
std::unordered_map<int, std::string> mKeybindsUp;  
Two separate unordered_map were used to maintain keybinding of the physical key number to the message requested from the game.

### MathUtil.h

When two line segments overlap at multiple points, `Line::Crosses` returns the first shared point along the first segment.

### Enemy path movement

`Enemy::SetPath` samples each cubic segment 100 times and stores cumulative distances. `AdvancePath` advances 3 pixels per frame, looks up the corresponding Bezier parameter, and updates position and rotation. Speed is approximate because distances are sampled. Call `SetPath` whenever assigning an entrance, attack, or return path so the table and travelled distance reset together.

## Library Versions

SFML - version 3.1.0

## Resources

### AI Usage

- Galaga wave spawning (`Galaga::Update`, `State::Waves`): I supplied the wave ideas, enemy groupings, entrance directions, and the existing switch/for-loop structure. Codex implemented the actual spawning and Bezier control-point calculations, formation positions, offscreen starting distances, and expansion to eight enemies per row. AI also added initial frame-based wave timing; automatic wave progression is still unfinished. The wave case is marked with an AI-assisted comment.
    - Prompts requested: a synchronized left-side entrance for the first row; normal middle ships followed by heavy edge ships in row 2; bottom-corner entrances through the player for row 3; normal ships split between bottom-right and left entrances for row 4; and normal outer ships, crossing special ships, and central heavy ships for row 5.
    - Follow-up prompts: "instead of doing seconds, just spawn the outer ones farther away so they take longer to show up, that should simplify things a lot yea?" and "oh shit i forgot its 8 per row, take some liberty and fix the remaining 4 rows, i dont care which enemies you use, you can pair them with already existing entrances".
    - Validation: headless C++ checks passed for five waves of eight enemies, expected types, 40 total enemies, and all entrance paths reaching distinct formation slots. Earlier checks covered entrance ordering and player-position crossings. Full gameplay and automatic wave progression have not been validated; team review and reflection remain to be completed.

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
