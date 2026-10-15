# CMPUT 350 HW 1

## Design Choices

### MathUtil.h

When two line segments overlap at multiple points, `Line::Crosses` returns the first shared point along the first segment.

## Library Versions

SFML - version 3.1.0

## Resources

### AI Usage

- Codex prompt: "fix them" (referring to `Bezier::GetPoint` and `Bezier::GetSlope` after reviewing their missing support for connected cubic segments).
    - AI updated both methods to select a cubic segment, handle the final endpoint, and reject invalid input. AI also added the segment helper and header includes/guard. Changed implementation sections are marked in `Bezier.cpp`.
    - Validation: a standalone C++ check passed for single and multiple segments, derivatives, shared/final endpoints, and invalid inputs. Team review and reflection on these changes remain to be completed.

- Cursor Prompt: @DrawContext.cpp (65-76) Using this format of docstring, update all function docstrings in @engine folder
    - This was helpful for documentation and it uses the requested format of docstring. 

## Memory

- Valgrind and leaks report some leak but looked it up, it's some noise produced by the graphics renderer.