#include "geopolyline.h"

#include <QDebug>

GeoPolyline::GeoPolyline()
{
    strncpy_s(type, 16, "POLYLINE", strlen("POLYLINE"));
}
GeoPolyline::~GeoPolyline()
{

}
void GeoPolyline::addPoint(float x, float y)
{
  //  qDebug() << points.size();


    points.emplace_back(x, y);
}

int GeoPolyline::getNumPoints()
{
    return points.size();
}
GeoExtent GeoPolyline::getExtent()
{
    size_t size = points.size();
    float minX,minY,maxX,maxY;
    minX  = points[0].x;
    minY  = points[0].y;
    maxX = 0.0;
    maxY = 0.0;
   for(size_t i = 0; i < size; i++)
   {
       if (points[i].x > maxX)
       {
           maxX = points[i].x;
       }
      else if (points[i].x < minX)
           minX = points[i].x;
       if (points[i].y > maxY)
       {
           maxY = points[i].y;
       }
       else if(points[i].y < minY)
           minY = points[i].y;
   }
   return GeoExtent(minX,maxX,minY,maxY);
}
