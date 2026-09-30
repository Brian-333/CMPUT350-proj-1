#include "DrawContext.h"

// TODO: Add error handling for invalid window or font

namespace CMPUT350 {

// Constructor
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text printedText(*mFont);

    printedText.setString(text);
    printedText.setCharacterSize(pixelSize);
    printedText.setFillColor(sf::Color(c.r, c.g, c.b));
    printedText.setPosition({p.x - printedText.getGlobalBounds().size.x / 2, p.y - printedText.getGlobalBounds().size.y / 2});
    
    mWindow->draw(printedText);
}

void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text printedText(*mFont);

    printedText.setString(text);
    printedText.setCharacterSize(pixelSize);
    printedText.setFillColor(sf::Color(c.r, c.g, c.b));
    printedText.setPosition({p.x, p.y});
    
    mWindow->draw(printedText);
}

void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape circle(radius);

    circle.setFillColor(sf::Color(c.r, c.g, c.b));
    circle.setPosition({p.x, p.y});
    
    mWindow->draw(circle);
}

void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape rectangle;
    
    rectangle.setPosition({r.topLeft.x, r.topLeft.y});
    rectangle.setSize({r.width, r.height});
    rectangle.setFillColor(sf::Color(c.r, c.g, c.b));

    mWindow->draw(rectangle);
}

void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    sf::RectangleShape rectangle;
    
    rectangle.setPosition({r.topLeft.x, r.topLeft.y});
    rectangle.setSize({r.width, r.height});
    rectangle.setOutlineColor(sf::Color(c.r, c.g, c.b));
    rectangle.setOutlineThickness(width);

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
    polygon.setPoint(0, from - offset);
    polygon.setPoint(1, to - offset);
    polygon.setPoint(2, to + offset);
    polygon.setPoint(3, from + offset);

    polygon.setFillColor(sf::Color(c.r, c.g, c.b));
    polygon.setPosition({0, 0});

    mWindow->draw(polygon);
}

int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
