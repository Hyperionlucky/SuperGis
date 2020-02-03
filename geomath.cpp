#include "geomath.h"
#include <cmath>
#include <QDebug>

/* double变量的比较 */
int dcmp(double x, double y, double precision /*= 1e-6*/)
{
    if (fabs(x - y) < precision)
        return 0;
    else if (x < y)
        return -1;
    else
        return 1;
}

/* 叉积 */
double cross(const GeoPoint& vec1Start, const GeoPoint& vec1End, const GeoPoint& vec2Start, const GeoPoint& vec2End)
{
    return (vec1End.x - vec1Start.x) * (vec2End.y - vec2Start.y) -
            (vec1End.y - vec1Start.y) * (vec2End.x - vec2Start.x);
}

/* 两点间的距离 */
double distancePointToPoint(const GeoRawPoint& ptA, const GeoRawPoint& ptB)
{
    return sqrt((ptA.x - ptB.x) * (ptA.x - ptB.x) + (ptA.y - ptB.y) * (ptA.y - ptB.y));
}

// 两个点相等
bool isPointEqPoint(const GeoRawPoint& ptA, const GeoRawPoint& ptB, double precision /*= 2*/)
{
    double dis = distancePointToPoint(ptA, ptB);
    if (dis <= precision)
        return true;
    else
        return false;
}

// 点在线段上
bool isPointOnLine(const GeoPoint& pt, const GeoPoint& lineStartPt, const GeoPoint& lineEndPt, double precision /*= 0.001*/)
{
    // 叉积为0
    if (fabs(cross(pt, lineStartPt, pt, lineEndPt)) < precision) {
        if ((lineStartPt.x < pt.x && lineEndPt.x > pt.x) || (lineEndPt.x < pt.x && lineStartPt.x > pt.x)) {
            if ((lineStartPt.y < pt.y && lineEndPt.y > pt.y) || (lineEndPt.y < pt.y && lineStartPt.y > pt.y)) {
                return true;
            }
        }
    }
    return false;
}

// 点在折线上
bool isPointOnLineString(const GeoPoint& pt, GeoPolyline* line, double precision /*= 0.001*/)
{
    int pointCount = line->getNumPoints();
    for (int i = 0; i < pointCount - 1; ++i) {
        if (isPointOnLine(pt, line->points[i], line->points[i + 1], precision)) {
            return true;
        }
    }
    return false;
}

// 点在面内
bool isPointInPolygon(const GeoPoint& pt, GeoPolygon* pRing, double precision)
{

    //size_t pointsCount = pRing->getNumPoints();
    int size = pRing->polylines.size();
    float maxX = pRing->polylines[0]->points[0].x;
    for(int j = 0; j < size; ++j)
    {
        GeoPolyline* exteriorRing = pRing->polylines[j];
        int polyline_size = exteriorRing->points.size();
        for (int i = 1; i < polyline_size; ++i) {
            if (pRing->polylines[j]->points[i].x > maxX)
                maxX = pRing->polylines[j]->points[i].x;
        }
    }

    const GeoPoint& ptLeft = pt;
    GeoPoint ptRight(maxX + 10, pt.y);	// 在maxX基础上随便加个数

    // 交点个数
    bool flag = false;
    for (int i = 0; i < size; i++)
    {
        GeoPolyline* exteriorRing = pRing->polylines[i];
        int polyline_size = exteriorRing->points.size();
        for (int j = 1, k = polyline_size - 1; j < polyline_size; k = j++) {
            if (isPointOnLine(pt, exteriorRing->points[j], exteriorRing->points[k]))
                return true;
            if (dcmp(exteriorRing->points[j].y, exteriorRing->points[k].y) == 0)
                continue;
            if (isLineIntersect(ptLeft, ptRight, exteriorRing->points[j], exteriorRing->points[k])) {
                if (isPointOnLine(exteriorRing->points[j], ptLeft, ptRight)) {
                    if (exteriorRing->points[k].y > pt.y) {
                        flag = !flag;
                    }
                }
                else if (isPointOnLine(exteriorRing->points[k], ptLeft, ptRight)) {
                    if (exteriorRing->points[j].y > pt.y) {
                        flag = !flag;
                    }
                }
                else {
                    flag = !flag;
                }
            }
        }
    }
    return flag;
}


/* 两线是否相交 */
bool isLineIntersect(const GeoPoint& line1Start, const GeoPoint& line1End, const GeoPoint& line2Start, const GeoPoint& line2End, double precision)
{
    // line1 跨立 line2
    double result1 = cross(line2Start, line1Start, line2Start, line2End) * cross(line2Start, line1End, line2Start, line2End);
    double result2 = cross(line1Start, line2Start, line1Start, line1End) * cross(line1Start, line2End, line1Start, line1End);
    if (result1 <= 0 && result2 <= 0)
        return true;
    else
        return false;
}


/* 两个矩形是否相交 */
bool isRectIntersect(const Rect& rect1, const Rect& rect2)
{
    return rect1.isIntersect(rect2);
}


/* 多段线与矩形相交 */
bool isLineStringRectIntersect(GeoPolyline* pLineString, const Rect& rect)
{
    const GeoPolyline& lineString = *pLineString;
    int pointsCount = lineString.points.size();

    // 多段线中任何一个顶点在矩形内，即相交
    for (int i = 0; i < pointsCount; ++i) {
        if (rect.contain(lineString.points[i].x,lineString.points[i].y))
            return true;
    }

    // 依次判断多段线的每一段是否与矩形边界相交
    GeoPoint ptLeftBottom(rect.minX, rect.minY);
    GeoPoint ptRightBottom(rect.maxX, rect.minY);
    GeoPoint ptRightTop(rect.maxX, rect.maxY);
    GeoPoint ptLeftTop(rect.minX, rect.maxY);
    for (int i = 0; i < pointsCount - 1; ++i) {
        if (isLineIntersect(ptLeftBottom, ptRightBottom, lineString.points[i], lineString.points[i + 1])
                || isLineIntersect(ptRightBottom, ptRightTop, lineString.points[i], lineString.points[i + 1])
                || isLineIntersect(ptRightTop, ptLeftTop, lineString.points[i], lineString.points[i + 1])
                || isLineIntersect(ptLeftTop, ptLeftBottom, lineString.points[i], lineString.points[i + 1]))
        {
            return true;
        }
    }

    return false;
}


/* 多边形与矩形相交 */
bool isPolygonRectIntersect(GeoPolygon* pPolygon, const Rect& rect)
{
    const GeoPolygon& polygon = *pPolygon;
    // 外环
    int size = polygon.polylines.size();
    for(int i = 0; i < size; i++)
    {
        GeoPolyline* exteriorRing = polygon.polylines[i];
        if (isLineStringRectIntersect(exteriorRing, rect)) {
            return true;
        }
    }

    return false;

}

