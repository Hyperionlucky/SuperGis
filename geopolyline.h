#ifndef GEOPOLYLINE_H
#define GEOPOLYLINE_H
#include <QColor>
#include "geopoint.h"
#include <vector>

class GeoPolyline : public GeoObject
{
public:
    GeoPolyline();
    ~GeoPolyline();
    std::vector<GeoPoint> points;
    void setId(unsigned int id){this->id=id;}
    unsigned int getId(){return this->id;}
    void setType(const char* typeIn) {
        strncpy_s(type, 16, typeIn, strlen(type));
    }
    const char*getType(){return type;}
    int getNumPoints();
    //Add point to points
    void addPoint(float x, float y);
    GeoExtent getExtent();
    QColor strokeColor;
    float stroke_width;

    void reserve(int count) { points.reserve(count); }

private:
    unsigned int id = 0;
    char type[16] = { 0 };

};

#endif // GEOPOLYLINE_H
