#pragma once
#include <iostream>
#include <algorithm>


using namespace std;

/**************************************************/
/*                                                */
/*              GeoRawPoint                       */
/*                                                */
/**************************************************/
class GeoRawPoint {
public:
    GeoRawPoint() : x(0.0), y(0.0) {}
    GeoRawPoint(double xx, double yy) : x(xx), y(yy) {}
    bool operator==(const GeoRawPoint& rhs)
        { return (fabs(x-rhs.x)<1e-6) && (fabs(y - rhs.y) < 1e-6); }
    double x;
    double y;
};


/**************************************************/
/*                                                */
/*               GeoExtent                        */
/*		   minX, maxX, minY, maxY                 */
/*                                                */
/**************************************************/
class GeoExtent {
public:
    GeoExtent()
        : minX(0.0), maxX(0.0), minY(0.0), maxY(0.0) {}
    GeoExtent(double xx, double yy)
        : minX(xx), maxX(xx), minY(yy), maxY(yy) {}
    GeoExtent(const GeoRawPoint& rawPt)
        : minX(rawPt.x), maxX(rawPt.x), minY(rawPt.y), maxY(rawPt.y) {}
    GeoExtent(const GeoRawPoint& leftTop, const GeoRawPoint rightBottom)
        : minX(leftTop.x), maxX(rightBottom.x), minY(rightBottom.y), maxY(leftTop.y) {}
    GeoExtent(double minXIn, double maxXIn, double minYIn, double maxYIn) :
        minX(minXIn), maxX(maxXIn), minY(minYIn), maxY(maxYIn) {}

    double centerX() const { return (maxX + minX) / 2; }
    double centerY() const { return (maxY + minY) / 2; }
    double width()	 const { return maxX - minX; }
    double height()  const { return maxY - minY; }
    double aspectRatio() const { return (maxX - minX) / (maxY - minY); }
    GeoRawPoint center() const { return GeoRawPoint((maxX + minX) / 2, (maxY + minY) / 2); }

    GeoExtent operator+(const GeoExtent& rhs) {
        GeoExtent extent;
        extent.minX = min(minX, rhs.minX);
        extent.maxX = max(maxX, rhs.maxX);
        extent.minY = min(minY, rhs.minY);
        extent.maxY = max(maxY, rhs.maxY);
        return extent;
    }

    GeoExtent& operator+=(const GeoExtent& rhs) {
        merge(rhs);
        return *this;
    }

    void merge(const GeoExtent& rhs) {
        minX = min(minX, rhs.minX);
        minY = min(minY, rhs.minY);
        maxX = max(maxX, rhs.maxX);
        maxY = max(maxY, rhs.maxY);
    }

    void merge(double xx, double yy) {
        minX = min(minX, xx);
        minY = min(minY, yy);
        maxX = max(maxX, xx);
        maxY = max(maxY, yy);
    }

    bool isIntersect(const GeoExtent& rhs) const {
        return minX <= rhs.maxX && maxX >= rhs.minX &&
            minY <= rhs.maxY && maxY >= rhs.minY;
    }

    void intersect(const GeoExtent& rhs) {
        if (isIntersect(rhs)) {
            minX = max(minX, rhs.minX);
            minY = max(minY, rhs.minY);
            maxX = min(maxX, rhs.maxX);
            maxY = min(maxY, rhs.maxY);
        }
        else {
            *this = GeoExtent();
        }
    }

  /*  bool contain(const GeoPoint& pt) const {
        return pt.x >= minX && pt.x <= maxX
            && pt.y >= minY && pt.y <= maxY;
    }
*/
    bool contain(float xx, float yy) const {
        return xx >= minX && xx <= maxX
            && yy >= minY && yy <= maxY;
    }

    void normalize() {
        if (minX > maxX)
            std::swap(minX, maxX);
        if (minY > maxY)
            std::swap(minY, maxY);
    }

public:
    double minX;
    double maxX;
    double minY;
    double maxY;
};
