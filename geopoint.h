#ifndef GEOPOINT_H
#define GEOPOINT_H
#include "geoobject.h"

#include <cstring>

class GeoPoint : public GeoObject
{
public:
    GeoPoint();
    ~GeoPoint();
    GeoPoint(float X, float Y) : x(X), y(Y) {
        strncpy_s(type, 16, "POINT", strlen("POINT"));
    }
    //GeoPoint(const GeoPoint&);
    float x = 0.0,y = 0.0;
    char ptName[16] = { 0 };
    void setId(unsigned int id){this->id=id;}
    unsigned int getId(){return this->id;}
    void setType(const char* typeIn) {
       strncpy_s(type, 16, typeIn, strlen(type));
    }
    const char*getType(){return type;}
    GeoExtent getExtent();
private:
    unsigned int id ;

    char type[16] = { 0 };
    //const char* type = "Point";
};

#endif // GEOPOINT_H
