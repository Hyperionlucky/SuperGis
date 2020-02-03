#include "geomap.h"

GeoMap::GeoMap()
{

}

GeoMap::~GeoMap()
{
    for (auto& layer : geolayers)
        delete layer;
}

void GeoMap :: addLayer(GeoLayer* layer)
{
    if (geolayers.empty()) {
        geolayers.emplace_back(layer);
        geolayers.back()->setId(0);
    }
    else
    {
        unsigned int nextId = geolayers.back()->getId();
        geolayers.emplace_back(layer);
        geolayers.back()->setId(nextId);
    }
}

// 移除layer
void GeoMap::deleteLayerAt(int index){
    GeoLayer* layer = getLayerByLID(index);
    if(layer)
    {
        delete layer;
        geolayers.erase(geolayers.begin()+index);
    }
}

// 全部移除
void GeoMap::deleteLayerAll(){
    geolayers.clear();
}

GeoLayer* GeoMap::getLayerByLID(int nLID)
{
    for(auto&layer:geolayers)
    {
        if(layer->getId() == nLID)
        {
            return layer;
        }
    }
    return nullptr;
}
