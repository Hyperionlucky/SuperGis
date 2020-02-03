#include "geopoint.h"

GeoPoint::GeoPoint()
{

}
GeoPoint::~GeoPoint()
{

}

GeoExtent GeoPoint::getExtent()
{
   return GeoExtent(this->x,this->x,this->y,this->y);
}
