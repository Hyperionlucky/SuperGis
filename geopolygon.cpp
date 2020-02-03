#include "geopolygon.h"
#include <QDebug>
GeoPolygon::GeoPolygon()
{
    strncpy_s(type, 16, "POLYGON", strlen("POLYGON"));
}

GeoPolygon::~GeoPolygon()
{
    size_t size = polylines.size();
    for(unsigned i = 0; i < size; i++)
    {
        delete polylines[i];
    }


}

void GeoPolygon::addPolyline(GeoPolyline* polyline)
{
    polylines.push_back(polyline);
}

void GeoPolygon::addPlgonCorrinate(GeoPoint corrdinate)
{
    corrdinates.push_back(corrdinate);
}

size_t GeoPolygon::getNumPoints()
{
    size_t count = 0;
    size_t polySize = polylines.size();
    for(unsigned int i = 0; i < polySize; i++)
    {
        count += polylines[i]->points.size();
    }
    return count ;
}

unsigned int GeoPolygon::getId()
{
    //qDebug() << this->id ;
    unsigned int fid;
    fid = this->id;
    return fid;
}

void GeoPolygon::setId(unsigned int ID)
{
    this->id = ID;
}

GeoExtent GeoPolygon::getExtent()
{
    size_t polySize = polylines.size();
    GeoExtent extent(polylines[0]->points[0].x, polylines[0]->points[0].y);
    for(unsigned int i = 0; i < polySize; i++)
    {
        size_t size = polylines[i]->points.size();
        for(size_t j = 0; j < size; j++)
        {
            extent.merge(polylines[i]->points[j].x, polylines[i]->points[j].y);
        }
    }
    return extent;
}
