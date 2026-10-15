#include "Bezier.h"
#include <stdexcept>

namespace CMPUT350 {

namespace {

// AI-assisted: select four control points and convert global t to local t.
// Throws for invalid point counts or t outside the curve's parameter range.
std::size_t SelectSegment(const std::vector<Point2D>& pts, float& t) {
    if (pts.size() < 4 || (pts.size() - 1) % 3 != 0) {
        throw std::invalid_argument("Bezier requires 3*k + 1 points, with k >= 1");
    }
    const std::size_t segmentCount = (pts.size() - 1) / 3;
    if (!std::isfinite(t) || t < 0 || t > static_cast<float>(segmentCount)) {
        throw std::out_of_range("Bezier t is outside the curve's parameter range");
    }
    // The final endpoint belongs to the last segment at local t = 1.
    const std::size_t segment = std::min(
        static_cast<std::size_t>(t), segmentCount - 1);
    t -= static_cast<float>(segment);
    return segment * 3;
}

}


Bezier::Bezier(const std::vector<Point2D> &pts): mPoints(pts)
{
}

Point2D Bezier::GetPoint(float t) const {
    return Bezier::GetPoint(mPoints, t);
}

Point2D Bezier::GetSlope(float t) const {
    return Bezier::GetSlope(mPoints, t);
}

Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t) {
    // AI-assisted: apply de Casteljau interpolation to the selected cubic only.
    const std::size_t start = SelectSegment(pts, t);
    Point2D tmp[4] = {pts[start], pts[start + 1], pts[start + 2], pts[start + 3]};
    for (int remaining = 3; remaining > 0; --remaining) {
        for (int x = 0; x < remaining; ++x) {
            tmp[x] = tmp[x] * (1.0f - t) + tmp[x + 1] * t;
        }
    }
    return tmp[0];
}

Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t) {
    // AI-assisted: evaluate the derivative on the same segment as GetPoint.
    const std::size_t start = SelectSegment(pts, t);
    const float c = 1 - t;
    float slopeX = 3 * (c * c) * (pts[start + 1].x - pts[start].x) +
        6 * c * t * (pts[start + 2].x - pts[start + 1].x) +
        3 * (t * t) * (pts[start + 3].x - pts[start + 2].x);
    float slopeY = 3 * (c * c) * (pts[start + 1].y - pts[start].y) +
        6 * c * t * (pts[start + 2].y - pts[start + 1].y) +
        3 * (t * t) * (pts[start + 3].y - pts[start + 2].y);
    return Point2D{slopeX, slopeY}; 
}

}
