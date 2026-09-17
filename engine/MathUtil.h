#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    double Distance(const Point2D &other) const {
        return std::sqrt(std::pow((x - other.x), 2) + std::pow((y - other.y), 2));  // distance formula 
    }
    Point2D operator+(const Point2D &other) const {
        return Point2D(x + other.x, y + other.y);   // x1 + x2, y1 + y2
    }
    Point2D operator+(const float &other) const {
        return Point2D(x + other, y + other);   // x + c, y + c
    }
    Point2D operator-(const Point2D &other) const {
        return Point2D(x - other.x, y - other.y);
    }
    Point2D operator-(const float &other) const {
        return Point2D(x - other, y - other);
    }
    Point2D operator*(const float &scalar) const {
        return Point2D(x * scalar, y * scalar);
    }
    Point2D &operator+=(const float &scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    Point2D &operator+=(const Point2D &other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Point2D &operator-=(const Point2D &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    bool operator==(const Point2D &other) const {
        return x == other.x && y == other.y;
    }
    Point2D &operator*=(const int &scalar) {
        x = x * scalar;
        y = y * scalar;
        return *this;
    }
    Point2D &operator/=(const int &scalar) {
        x = x * (1/scalar);
        y = y * (1/scalar);
        return *this;
    }
    float operator*(const Point2D &other) const {   // Dot product
        return x * other.x + y * other.y;
    }
    float Dot(Point2D b) const {    // Dot product of this and another point
        return *this * b;
    }
    static float Dot(Point2D a, Point2D b) {    // Dot product of 2 points
        return a * b;
    }
    static float Cross(Point2D a, Point2D b) {  // Determinant
        return a.x * b.x - a.y  * b.y;
    }
    void Normalize() {
        double length = Distance(Point2D(0, 0));
        x = x / length;
        y = y / length;
    }
};

static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    os << '(' << p.x << ', ' << p.y << ')';
    return os;
}

static Point2D operator*(float number, const Point2D &rhs) {    // Just the reverse of our already existing implementation
    return rhs * number;
}

struct Line {
    Point2D p1, p2;

    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    float Length() const {
        return p1.Distance(p2);
    }
    Point2D ClosestPoint(const Point2D &p) const {  
        // given point A, B, P
        // AB = B - A, AP = P - A
        Point2D AB = p2 - p1;
        Point2D AP = p - p1;

        float ABdot = Point2D::Dot(AB, AB);
        if (ABdot == 0) {   // check if denominator is 0
            return p1;
        }

        float t = Point2D::Dot(AP, AB) / ABdot;

        if (t < 0) t = 0;
        if (t > 1) t = 1;

        Point2D closestPoint = p1 + t * AB; // projection formula
        return closestPoint;
    }
    bool Crosses(Line other, Point2D &crossingPoint) const {
            // TODO
            return false;
    }
};

static std::ostream &operator<<(std::ostream &os, const Line &l) {
    os << 'P1:' << l.p1 << ', P2:' << l.p2;
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

struct Rect {
    Point2D topLeft;
    float width, height;

    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(top, left)), width(width), height(height) {}

    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    Rect &operator|=(const Rect &other) {
        *this |= other.topLeft;    // top left
        *this |= Point2D(other.topLeft.x + other.width, other.topLeft.y + other.height);    // bottom right
        return *this;
    }
    Rect &operator|=(const Point2D &other) {
        if (other.x < topLeft.x) {
            width += topLeft.x - other.x;
            topLeft.x = other.x;
        }
        if (other.x > topLeft.x + width) {
            width = other.x - topLeft.x;
        }
        if (other.y < topLeft.y) {
            height += topLeft.y - other.y;
            topLeft.y = other.y;
        }
        if (other.y > topLeft.y + height) {
            height = other.y - topLeft.y;
        }
        return *this;
    }
    Rect &operator|=(const Line &other) {
        *this |= other.p1;
        *this |= other.p2;
        return *this;
    }
    Rect &operator&=(const Rect &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator+=(const Point2D &other) {
        topLeft += other;
        return *this;
    }
    Rect operator+(const Point2D &other) const {
        Rect result = *this;
        result.topLeft += other;
        return result;
    }
    void Inset(int inset) {
        topLeft.x = topLeft.x + inset;
        topLeft.y = topLeft.y + inset;
        width = width - 2 * inset;
        height = height - 2 * inset;
    }
    bool IsInside(const Point2D &p) const {
        return p.x >= topLeft.x && p.x <= topLeft.x + width && p.y <= topLeft.y && p.y >= topLeft.y + height;
    }
};

static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    os << '(' << l.topLeft << ', ' << l.width << ', ' << l.height << ')';
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
