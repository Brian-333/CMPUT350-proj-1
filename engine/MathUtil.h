#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <algorithm>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;

    /**
     * @brief Constructs a 2D point.
     *
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     */
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}

    /**
     * @brief Computes the Euclidean distance to another point.
     *
     * @param other The point to measure distance to.
     * @return The distance between this point and other.
     */
    double Distance(const Point2D &other) const {
        return std::sqrt(std::pow((x - other.x), 2) + std::pow((y - other.y), 2));  // distance formula 
    }

    /**
     * @brief Adds two points component-wise.
     *
     * @param other The point to add.
     * @return A new point equal to the component-wise sum.
     */
    Point2D operator+(const Point2D &other) const {
        return Point2D(x + other.x, y + other.y);   // x1 + x2, y1 + y2
    }

    /**
     * @brief Adds a scalar to both components.
     *
     * @param other The scalar to add.
     * @return A new point with the scalar added to x and y.
     */
    Point2D operator+(const float &other) const {
        return Point2D(x + other, y + other);   // x + c, y + c
    }

    /**
     * @brief Subtracts another point component-wise.
     *
     * @param other The point to subtract.
     * @return A new point equal to the component-wise difference.
     */
    Point2D operator-(const Point2D &other) const {
        return Point2D(x - other.x, y - other.y);
    }

    /**
     * @brief Subtracts a scalar from both components.
     *
     * @param other The scalar to subtract.
     * @return A new point with the scalar subtracted from x and y.
     */
    Point2D operator-(const float &other) const {
        return Point2D(x - other, y - other);
    }

    /**
     * @brief Scales both components by a scalar.
     *
     * @param scalar The scale factor.
     * @return A new scaled point.
     */
    Point2D operator*(const float &scalar) const {
        return Point2D(x * scalar, y * scalar);
    }

    /**
     * @brief Adds a scalar to both components in place.
     *
     * @param scalar The scalar to add.
     * @return A reference to this point.
     */
    Point2D &operator+=(const float &scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }

    /**
     * @brief Adds another point component-wise in place.
     *
     * @param other The point to add.
     * @return A reference to this point.
     */
    Point2D &operator+=(const Point2D &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    /**
     * @brief Subtracts another point component-wise in place.
     *
     * @param other The point to subtract.
     * @return A reference to this point.
     */
    Point2D &operator-=(const Point2D &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /**
     * @brief Checks whether two points have equal components.
     *
     * @param other The point to compare against.
     * @return True if both components are equal; false otherwise.
     */
    bool operator==(const Point2D &other) const {
        return x == other.x && y == other.y;
    }

    /**
     * @brief Scales both components by an integer in place.
     *
     * @param scalar The integer scale factor.
     * @return A reference to this point.
     */
    Point2D &operator*=(const int &scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    /**
     * @brief Divides both components by an integer in place.
     *
     * @param scalar The integer divisor. Division is skipped if scalar is 0.
     * @return A reference to this point.
     */
    Point2D &operator/=(const int &scalar) {
        if (scalar != 0) 
        {
            x /= scalar;
            y /= scalar;
        }
        return *this;
    }

    /**
     * @brief Computes the dot product with another point treated as a vector.
     *
     * @param other The other vector.
     * @return The scalar dot product.
     */
    float operator*(const Point2D &other) const {   // Dot product
        return x * other.x + y * other.y;
    }

    /**
     * @brief Computes the dot product with another point.
     *
     * @param b The other point.
     * @return The scalar dot product.
     */
    float Dot(Point2D b) const {    // Dot product of this and another point
        return *this * b;
    }

    /**
     * @brief Computes the dot product of two points.
     *
     * @param a The first point.
     * @param b The second point.
     * @return The scalar dot product.
     */
    static float Dot(Point2D a, Point2D b) {    // Dot product of 2 points
        return a * b;
    }

    /**
     * @brief Computes the 2D cross-product magnitude (determinant) of two points.
     *
     * @param a The first point.
     * @param b The second point.
     * @return The scalar cross product a.x * b.y - a.y * b.x.
     */
    static float Cross(Point2D a, Point2D b) {  // Determinant
        return a.x * b.y - a.y * b.x;
    }

    /**
     * @brief Normalizes this point to unit length in place.
     *
     * If the point is at the origin, it is left unchanged.
     */
    void Normalize() {
        double length = Distance(Point2D(0, 0));
        if (length == 0) {
            return;
        }
        x = x / length;
        y = y / length;
    }
};

/**
 * @brief Writes a point to an output stream as "(x, y)".
 *
 * @param os The output stream.
 * @param p The point to write.
 * @return A reference to the output stream.
 */
static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    os << '(' << p.x << ", " << p.y << ')';
    return os;
}

/**
 * @brief Scales a point by a scalar on the left-hand side.
 *
 * @param number The scale factor.
 * @param rhs The point to scale.
 * @return A new scaled point.
 */
static Point2D operator*(float number, const Point2D &rhs) {    // Just the reverse of our already existing implementation
    return rhs * number;
}

struct Line {
    Point2D p1, p2;

    /**
     * @brief Constructs a line from two endpoints.
     *
     * @param p1 The first endpoint.
     * @param p2 The second endpoint.
     */
    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}

    /**
     * @brief Constructs a line from four coordinate components.
     *
     * @param x1 The x-coordinate of the first endpoint.
     * @param y1 The y-coordinate of the first endpoint.
     * @param x2 The x-coordinate of the second endpoint.
     * @param y2 The y-coordinate of the second endpoint.
     */
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}

    /**
     * @brief Returns the length of the line segment.
     *
     * @return The distance between the two endpoints.
     */
    float Length() const {
        return p1.Distance(p2);
    }

    /**
     * @brief Finds the closest point on this segment to a given point.
     *
     * @param p The query point.
     * @return The closest point on the segment to p.
     */
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

    /**
     * @brief Tests whether this segment intersects another segment.
     *
     * @param other The other line segment.
     * @param crossingPoint Set to an intersection point when the segments cross.
     * @return True if the segments intersect; false otherwise.
     */
    bool Crosses(Line other, Point2D &crossingPoint) const {
        // Logic found from:
        // https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/565282#565282
        // Cases are taken from here
        // The segments are p1 + t*r and other.p1 + u*s, where t and u are in [0, 1]
        Point2D r = p2 - p1;
        Point2D s = other.p2 - other.p1;
        Point2D difference = other.p1 - p1;
        float determinant = Point2D::Cross(r, s);

        if (determinant == 0) { 
            // case 2: parallel, non-collinear segments cannot cross
            if (Point2D::Cross(difference, r) != 0) {
                return false;  
            }

            // A zero-length first segment is just a point
            float rSquared = Point2D::Dot(r, r);
            if (rSquared == 0) {
                crossingPoint = p1;
                return other.ClosestPoint(p1) == p1;
            }

            // case 1: project the collinear segment onto this one
            // t0 and t1 locate its endpoints along this segment's [0, 1] range
            float t0 = Point2D::Dot(difference, r) / rSquared;
            float t1 = t0 + Point2D::Dot(s, r) / rSquared;
            float overlapStart = std::max(0.0f, std::min(t0, t1));
            float overlapEnd = std::min(1.0f, std::max(t0, t1));

            if (overlapStart > overlapEnd) 
            {
                return false;
            }
            // Use the first shared point when the segments overlap
            crossingPoint = p1 + overlapStart * r;
            return true;
        }

        float t = Point2D::Cross(difference, s) / determinant;
        float u = Point2D::Cross(difference, r) / determinant;

        // case 4: lines not parallel but do not intersect
        if (t < 0 || t > 1 || u < 0 || u > 1) 
        {
            return false;
        }

        // case 3: both intersection positions are on the segments.
        crossingPoint = p1 + t * r;
        return true;
    }
};

/**
 * @brief Writes a line to an output stream.
 *
 * @param os The output stream.
 * @param l The line to write.
 * @return A reference to the output stream.
 */
static std::ostream &operator<<(std::ostream &os, const Line &l) {
    os << "P1:" << l.p1 << ", P2:" << l.p2;
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    /**
     * @brief Constructs a circle from a center point and radius.
     *
     * @param c The center point.
     * @param r The radius.
     */
    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    /**
     * @brief Constructs a circle from center coordinates and a radius.
     *
     * @param x The center x-coordinate.
     * @param y The center y-coordinate.
     * @param r The radius.
     */
    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

struct Rect {
    Point2D topLeft;
    float width, height;

    /**
     * @brief Constructs a rectangle from left, top, width, and height.
     *
     * @param left The x-coordinate of the top-left corner.
     * @param top The y-coordinate of the top-left corner.
     * @param width The rectangle width.
     * @param height The rectangle height.
     */
    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top)), width(width), height(height) {}

    /**
     * @brief Constructs a rectangle from a top-left point, width, and height.
     *
     * @param tl The top-left corner.
     * @param w The rectangle width.
     * @param h The rectangle height.
     */
    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    /**
     * @brief Constructs a bounding box around two points with positive width and height.
     *
     * @param p1 The first corner point.
     * @param p2 The second corner point.
     */
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    /**
     * @brief Constructs a square bounding box around a circle defined by center and radius.
     *
     * @param center The circle center.
     * @param radius The circle radius.
     */
    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    /**
     * @brief Expands this rectangle to include another rectangle.
     *
     * @param other The rectangle to union with.
     * @return A reference to this rectangle.
     */
    Rect &operator|=(const Rect &other) {
        *this |= other.topLeft;    // top left
        *this |= Point2D(other.topLeft.x + other.width, other.topLeft.y + other.height);    // bottom right
        return *this;
    }

    /**
     * @brief Expands this rectangle to include a point.
     *
     * @param other The point to include.
     * @return A reference to this rectangle.
     */
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

    /**
     * @brief Expands this rectangle to include both endpoints of a line.
     *
     * @param other The line to include.
     * @return A reference to this rectangle.
     */
    Rect &operator|=(const Line &other) {
        *this |= other.p1;
        *this |= other.p2;
        return *this;
    }

    /**
     * @brief Intersects this rectangle with another rectangle in place.
     *
     * @param other The rectangle to intersect with.
     * @return A reference to this rectangle. Width and height become 0 if there is no overlap.
     */
    Rect &operator&=(const Rect &other) {
        float left = std::max(topLeft.x, other.topLeft.x);
        float top = std::max(topLeft.y, other.topLeft.y);
        float right = std::min(topLeft.x + width, other.topLeft.x + other.width);
        float bot = std::min(topLeft.y + height, other.topLeft.y + other.height);

        // kinda confusing because we work in bottom right of grid but top < bot actually means the opposite of the wording
        // bottom will be a larger number if the rect is allowed
        if (left > right || top > bot) {  
            width = 0;
            height = 0;
            return *this;
        }
        topLeft = {left, top};
        width = right - left;
        height = bot - top;
        return *this;
    }

    /**
     * @brief Translates this rectangle by a point in place.
     *
     * @param other The translation offset.
     * @return A reference to this rectangle.
     */
    Rect &operator+=(const Point2D &other) {
        topLeft += other;
        return *this;
    }

    /**
     * @brief Returns a copy of this rectangle translated by a point.
     *
     * @param other The translation offset.
     * @return A new translated rectangle.
     */
    Rect operator+(const Point2D &other) const {
        Rect result = *this;
        result.topLeft += other;
        return result;
    }

    /**
     * @brief Insets (shrinks) the rectangle equally on all sides.
     *
     * @param inset The amount to inset from each edge.
     */
    void Inset(int inset) {
        topLeft.x = topLeft.x + inset;
        topLeft.y = topLeft.y + inset;
        width = width - 2 * inset;
        height = height - 2 * inset;
    }

    /**
     * @brief Checks whether a point lies inside this rectangle.
     *
     * @param p The point to test.
     * @return True if the point is inside or on the boundary; false if outside or the rect is empty.
     */
    bool IsInside(const Point2D &p) const {
        if (width <= 0 || height <= 0) {
            return false;
        }
        return p.x >= topLeft.x && p.x <= topLeft.x + width && p.y >= topLeft.y && p.y <= topLeft.y + height;
    }
};

/**
 * @brief Writes a rectangle to an output stream.
 *
 * @param os The output stream.
 * @param l The rectangle to write.
 * @return A reference to the output stream.
 */
static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    os << '(' << l.topLeft << ", " << l.width << ", " << l.height << ')';
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
