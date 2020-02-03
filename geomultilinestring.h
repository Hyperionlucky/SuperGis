#ifndef GEOMULTILINESTRING_H
#define GEOMULTILINESTRING_H

#include "geopolyline.h"

class GeoMultiLineString : public GeoObject
{
public:
    GeoMultiLineString();
    ~GeoMultiLineString();
    std::vector<GeoPolyline* >polylines;
    void addPolyline(GeoPolyline*);
    void setId(unsigned int id){this->id=id;}
    unsigned int getId(){return this->id;}
    const char*getType(){return type;}
    void setType(const char* typeIn) {
       strncpy_s(type, 16, typeIn, strlen(type));
    }
    size_t getNumPoints();
    GeoExtent getExtent();
    void reserve(int count) { polylines.reserve(count); }
private:
        unsigned int id;
        char type[20] = { 0 };
};

#endif // GEOMULTILINESTRING_H
