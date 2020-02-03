#ifndef GEOMULTIPOLYGON_H
#define GEOMULTIPOLYGON_H
#include "geopolygon.h"
#include <cstring>

class GeoMultiPolygon : public GeoObject
{
public:
    GeoMultiPolygon();
    ~GeoMultiPolygon();
    std::vector<GeoPolygon*> polygons;
    void addPolygon(GeoPolygon* polygon);
    void setId(unsigned int id){this->id=id;}
    unsigned int getId(){return this->id;}
    void setType(const char* typeIn) {
       strncpy_s(type, 16, typeIn, strlen(type));
    }
    const char*getType(){return type;}
    int getNumPoints();
    GeoExtent getExtent();
    void reserve(int count) { polygons.reserve(count); }
    QColor fillColor;
private:
    unsigned int id;
    char type[16] = { 0 };
};

#endif // GEOMULTIPOLYGON_H
