#ifndef GEOMAP_H
#define GEOMAP_H
#include "geolayer.h"

class GeoMap
{
public:
    GeoMap();
    ~GeoMap();
    std::vector<GeoLayer*> geolayers;
    float left,top,right,bottom;
    void addLayer(GeoLayer* layer);
    // remove one of geolayers
    void deleteLayerAt(int index);
    // remove all
    void deleteLayerAll();
    GeoLayer* getLayerByLID(int nLID);
};

#endif // GEOMAP_H
