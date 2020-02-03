#include "geomultilinestring.h"

GeoMultiLineString::GeoMultiLineString()
{
   strncpy_s(type, 20, "MULTILINESTRING", strlen("MULTILINESTRING"));
}

GeoMultiLineString :: ~GeoMultiLineString()
{
    for (auto lineString:polylines)
    {
        delete lineString;
    }
}

void GeoMultiLineString :: addPolyline(GeoPolyline* lineString)
{
    polylines.emplace_back(lineString);
    polylines.back()->setId(id++);
}

size_t GeoMultiLineString :: getNumPoints()
{
    size_t count = 0;
    size_t numOfLineString = polylines.size();
    for(size_t i = 0; i < numOfLineString; i++)
    {
       count += polylines[i]->points.size();
    }
    return count;
}

GeoExtent GeoMultiLineString::getExtent()
{
    size_t numOfLineString = polylines.size();
    GeoExtent extent(polylines[0]->points[0].x, polylines[0]->points[0].y);
    for(size_t i = 0; i < numOfLineString; i++)
    {
        size_t size = polylines[i]->points.size();
        for(size_t j = 0; j < size; j++)
        {
             extent.merge(polylines[i]->points[j].x, polylines[i]->points[j].y);
        }
    }
    return extent;
}
