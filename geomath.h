#ifndef GEOMATH_H
#define GEOMATH_H

#include "geobase.hpp"
#include "geopolyline.h"
#include "geopolygon.h"

using Rect = GeoExtent;

/* 浮点数比较 */
int dcmp(double x, double y, double precision = 1e-6);

/* 叉积 */
double cross(const GeoPoint& vec1Start, const GeoPoint& vec1End, const GeoPoint& vec2Start, const GeoPoint& vec2End);

/* 两点间的距离 */
double distancePointToPoint(const GeoRawPoint& ptA, const GeoRawPoint& ptB);

/* 两点是否相等（给定精度下） */
bool isPointEqPoint(const GeoRawPoint& ptA, const GeoRawPoint& ptB, double precision = 2);

/* 点是否在线上 */
bool isPointOnLine(const GeoPoint& pt, const GeoPoint& lineStartPt, const GeoPoint& lineEndPt, double precision = 0.001);
bool isPointOnLineString(const GeoPoint& pt, GeoPolyline* lineString, double precision = 0.001);

/* Point & LinearRing */
bool isPointInLinearRing(const GeoPoint& pt, GeoPolygon* ring, double precision = 0.001);

/* Point & Polygon */
bool isPointInPolygon(const GeoPoint& pt, GeoPolygon* polygon, double precision = 0.001);

/* 两线是否相交 */
bool isLineIntersect(const GeoPoint& line1Start, const GeoPoint& line1End,
    const GeoPoint& line2Start, const GeoPoint& line2End, double precision = 1e-6);

/* 两个矩形是否相交 */
bool isRectIntersect(const Rect& rect1, const Rect& rect2);

/* 多段线与矩形相交 */
bool isLineStringRectIntersect(GeoPolyline* lineString, const Rect& rect);

/* 多边形与矩形相交 */
bool isPolygonRectIntersect(GeoPolygon* polygon, const Rect& rect);



#endif // GEOMATH_H
