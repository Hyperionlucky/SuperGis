#ifndef GEOPOLYGON_H
#define GEOPOLYGON_H
#include "geopolyline.h"
#include <QVector>
#include <cstring>
#include <QColor>
class GeoPolygon : public GeoObject
{
public:
    GeoPolygon();
    ~GeoPolygon();
    std::vector<GeoPolyline*>polylines;
    std::vector<GeoPoint>corrdinates;
    //Add polylines to polygon
    void addPolyline(GeoPolyline*);
    void addPlgonCorrinate(GeoPoint corrdinate);
    GeoExtent getExtent();
    void setId(unsigned int Id);
    unsigned int getId();
    void setType(const char* typeIn) {
       strncpy_s(type, 16, typeIn, strlen(type));
    }
    const char*getType(){return type;}
    size_t getNumPoints();

    QColor fillColor;
    void reserve(int count) { polylines.reserve(count); }
private:
    unsigned int id = 0;
    char type[16] = { 0 };

};

#endif // GEOPOLYGON_H
