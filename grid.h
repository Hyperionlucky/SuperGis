#ifndef GRID_H
#define GRID_H

#include "geobase.hpp"
#include "geoobject.h"

#include <vector>

class Grid
{
public:
    Grid(int id) : id(id) {}
    Grid(int id, const GeoExtent& extent)
        : id(id), extent(extent) {}

    int getFeatureCount() { return geoObjects.size(); }
    int getId() const { return id; }
    const GeoExtent& getExtent() const { return extent; }
    void setExtent(const GeoExtent& extentIn) { extent = extentIn; }
    void addFeature(GeoObject* feature) { geoObjects.push_back(feature); }
    GeoObject* getFeature(int idx){return geoObjects[idx];}

private:
    int id;
    GeoExtent extent;
    std::vector<GeoObject*>geoObjects;
};

#endif // GRID_H
