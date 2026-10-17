#include "DrawContext.h"
#include "MathUtil.h"

// TODO: Add error handling for invalid window or font

namespace CMPUT350 {

/**
 * @brief Constructs a draw context for rendering to a window.
 *
 * @param window The SFML render window to draw onto.
 * @param font The font used when drawing text.
 */
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font), mContextOffset(0, 0), mContextRotation(0) {}

/**
 * @brief Draws text centered on a point with a specified size and color.
 *
 * @param text The string to draw.
 * @param pixelSize The character size in pixels.
 * @param p The center point of the text (Point2D).
 * @param c The color of the text, specified as an RGBColor object.
 *
 * Positions the text so its bounding box is centered on the given point,
 * then renders it onto the associated window.
 */
void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text printedText(*mFont);
    Point2D transformedPoint = Transform(p);
    printedText.setString(text);
    printedText.setCharacterSize(pixelSize);
    printedText.setFillColor(sf::Color(c.r, c.g, c.b));
    printedText.setPosition({transformedPoint.x - printedText.getGlobalBounds().size.x / 2, transformedPoint.y - printedText.getGlobalBounds().size.y / 2});
    
    mWindow->draw(printedText);
}

/**
 * @brief Draws text at a point with a specified size and color.
 *
 * @param text The string to draw.
 * @param pixelSize The character size in pixels.
 * @param p The top-left position of the text (Point2D).
 * @param c The color of the text, specified as an RGBColor object.
 *
 * Places the text at the given position and renders it onto the associated window.
 */
void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text printedText(*mFont);
    Point2D transformedPoint = Transform(p);
    printedText.setString(text);
    printedText.setCharacterSize(pixelSize);
    printedText.setFillColor(sf::Color(c.r, c.g, c.b));
    printedText.setPosition({transformedPoint.x, transformedPoint.y});
    
    mWindow->draw(printedText);
}

/**
 * @brief Draws a filled circle with a specified radius and color.
 *
 * @param p The center point of the circle (Point2D).
 * @param radius The radius of the circle in pixels.
 * @param c The fill color of the circle, specified as an RGBColor object.
 *
 * Creates a circle shape centered on the given point and renders it onto
 * the associated window.
 */
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape circle(radius);
    Point2D transformedPoint = Transform(p);

    circle.setFillColor(sf::Color(c.r, c.g, c.b));
    circle.setPosition({transformedPoint.x, transformedPoint.y});
    circle.setOrigin({radius, radius});
    
    mWindow->draw(circle);
}

/**
 * @brief Draws a filled rectangle with a specified color.
 *
 * @param r The rectangle to draw (Rect).
 * @param c The fill color of the rectangle, specified as an RGBColor object.
 *
 * Creates a rectangle shape from the given bounds and renders it onto
 * the associated window.
 */
void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape rectangle;
    Point2D newTopLeft = Transform(r.topLeft);

    rectangle.setPosition({newTopLeft.x, newTopLeft.y});
    rectangle.setSize({r.width, r.height});
    rectangle.setFillColor(sf::Color(c.r, c.g, c.b));
    rectangle.setRotation(sf::radians(mContextRotation));

    mWindow->draw(rectangle);
}

/**
 * @brief Draws an outlined rectangle with a specified outline width and color.
 *
 * @param r The rectangle to outline (Rect).
 * @param width The outline thickness in pixels.
 * @param c The outline color, specified as an RGBColor object.
 *
 * Creates a transparent-filled rectangle with an outline and renders it onto
 * the associated window.
 */
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    Point2D newTopLeft = Transform(r.topLeft);
    sf::RectangleShape rectangle;
    
    rectangle.setPosition({newTopLeft.x, newTopLeft.y});
    rectangle.setSize({r.width, r.height});
    rectangle.setOutlineColor(sf::Color(c.r, c.g, c.b));
    rectangle.setFillColor(sf::Color(0, 0, 0, 0));
    rectangle.setOutlineThickness(width);
    rectangle.setRotation(sf::degrees(mContextRotation));

    mWindow->draw(rectangle);
}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line (Point2D).
 * @param to The ending point of the line (Point2D).
 * @param width The width of the line in pixels.
 * @param c The color of the line, specified as an RGBColor object.
 *
 * This function calculates the distance and angle between the two points
 * and uses a polygone shape to represent the line. The line is drawn
 * relative to the world offset and rendered onto the associated window.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    sf::ConvexShape polygon;
    polygon.setPointCount(4);

    Point2D dir = to - from;
    dir.Normalize();

    // Get perpendicular vector scaled by half-thickness
    Point2D perp = Point2D(-dir.y, dir.x);
    Point2D offset = perp * (width / 2.f);

    // Set 4 corners of the rectangular polygon
    polygon.setPoint(0, {from.x - offset.x, from.y - offset.y});
    polygon.setPoint(1, {to.x - offset.x, to.y - offset.y});
    polygon.setPoint(2, {to.x + offset.x, to.y + offset.y});
    polygon.setPoint(3, {from.x + offset.x, from.y + offset.y});

    polygon.setFillColor(sf::Color(c.r, c.g, c.b));
    Point2D newOrigin = Transform(Point2D(0, 0));
    polygon.setPosition({newOrigin.x, newOrigin.y});
    polygon.setRotation(sf::radians(mContextRotation));

    mWindow->draw(polygon);
}

/**
 * @brief Returns the width of the associated window in pixels.
 *
 * @return The window width in pixels.
 */
int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

/**
 * @brief Returns the height of the associated window in pixels.
 *
 * @return The window height in pixels.
 */
int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }


void DrawContext::SetContextOffset(Point2D p) {
    mContextOffset = p;
}

void DrawContext::SetContextRotation(float rotation) {
    mContextRotation = rotation;
}

/**
 * @brief Transforms a point by the context offset and rotation.
 *
 * @param p The point to transform (Point2D).
 * @return The transformed point (Point2D).
 */
Point2D DrawContext::Transform(Point2D p) {
    float c = std::cos(mContextRotation);
    float s = std::sin(mContextRotation);
    Point2D rotated(p.x * c - p.y * s, p.x * s + p.y * c);
    return rotated + mContextOffset;

}

/**
 * @brief Reverses the transformation of a point by the context offset and rotation.
 *
 * @param p The point to reverse transform (Point2D).
 * @return The reversed transformed point (Point2D).
 */
Point2D DrawContext::ReverseTransform(Point2D p) {
    Point2D local = p - mContextOffset;
    float c = std::cos(-mContextRotation);
    float s = std::sin(-mContextRotation);
    return Point2D(local.x * c - local.y * s, local.x * s + local.y * c);
}

/**
 * @brief Rotates rectangle and computes new bounding box after rotation
 * 
 * This will provide a new AABB around the rotated rectangle
 *
 * @param r The rectangle to rotate (Rect).
 * @return The transformed rectangle (Rect).
 */
Rect DrawContext::Transform(Rect r) {
    static Point2D topLeft;
    static Point2D topRight;
    static Point2D bottomRight;
    static Point2D bottomLeft;
    static Point2D newTopLeft;
    static Point2D newBottomRight;
    topLeft = Transform(r.topLeft);
    topRight = Transform(Point2D(r.topLeft.x + r.width, r.topLeft.y));
    bottomRight = Transform(Point2D(r.topLeft.x + r.width, r.topLeft.y + r.height));
    bottomLeft = Transform(Point2D(r.topLeft.x, r.topLeft.y + r.height));
    newTopLeft = Point2D(
        std::min(topLeft.x, std::min(topRight.x, std::min(bottomRight.x, bottomLeft.x))),
        std::min(topLeft.y, std::min(topRight.y, std::min(bottomRight.y, bottomLeft.y))));
    newBottomRight = Point2D(
        std::max(topLeft.x, std::max(topRight.x, std::max(bottomRight.x, bottomLeft.x))),
        std::max(topLeft.y, std::max(topRight.y, std::max(bottomRight.y, bottomLeft.y))));
    return Rect(newTopLeft, newBottomRight);
}

}  // namespace CMPUT350
