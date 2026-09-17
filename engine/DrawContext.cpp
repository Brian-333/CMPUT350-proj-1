#include "DrawContext.h"

// TODO: Add error handling for invalid window or font

namespace CMPUT350 {

// Constructor
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text text(*mFont);

    text.setString(text);
    text.setCharacterSize(pixelSize);
    text.setFillColor(sf::Color(c.r, c.g, c.b));
    text.setPosition({p.x - text.getGlobalBounds().width / 2, p.y - text.getGlobalBounds().height / 2});
    
    mWindow->draw(text);
}

void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text text(*mFont);

    text.setString(text);
    text.setCharacterSize(pixelSize);
    text.setFillColor(sf::Color(c.r, c.g, c.b));
    text.setPosition({p.x, p.y});
    
    mWindow->draw(text);
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

    polygon.setPointCount(2);
    polygon.setPoint(0, sf::Vector2f(from.x, from.y));
    polygon.setPoint(1, sf::Vector2f(to.x, to.y));
    polygon.setOutlineColor(sf::Color(c.r, c.g, c.b));
    polygon.setOutlineThickness(width);
    polygon.setPosition({0, 0});

    mWindow->draw(polygon);
}

int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
