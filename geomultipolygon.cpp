#include "geomultipolygon.h"

GeoMultiPolygon::GeoMultiPolygon()
{
    strncpy_s(type, 16, "MULTIPOLYGON", strlen("MULTIPOLYGON"));
}
GeoMultiPolygon::~GeoMultiPolygon()
{
    size_t size = polygons.size();
    for(unsigned i = 0; i < size; i++)
    {
        delete polygons[i];
    }


}
void GeoMultiPolygon::addPolygon(GeoPolygon* polygon)
{
    polygons.emplace_back(polygon);
    polygons.back()->setId(id++);
}

int GeoMultiPolygon::getNumPoints()
{
    int count = 0;
    int polygonSize = polygons.size();
    for(int i = 0; i < polygonSize; i++)
    {
        unsigned int polySize = polygons[i]->polylines.size();
        for(unsigned int j = 0; j < polySize; j++)
        {

            count += polygons[i]->polylines[j]->points.size();
        }
    }
    return count ;
}

GeoExtent GeoMultiPolygon::getExtent()
{
    size_t polygonSize = polygons.size();
    GeoExtent extent(polygons[0]->polylines[0]->points[0].x, polygons[0]->polylines[0]->points[0].y);
    for(size_t k = 0; k < polygonSize; k++)
    {
        size_t polySize = polygons[k]->polylines.size();
        for(unsigned int i = 0; i < polySize; i++)
        {
            size_t size = polygons[k]->polylines[i]->points.size();
            for(size_t j = 0; j < size; j++)
            {
                 extent.merge(polygons[k]->polylines[i]->points[j].x, polygons[k]->polylines[i]->points[j].y);
            }
        }
    }
    return extent;
}
