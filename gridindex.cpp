#include "gridindex.h"
#include "geomath.h"
#include "geomultipolygon.h"
#include <iostream>
#include <algorithm>

GridIndex::GridIndex()
{

}

GridIndex::~GridIndex()
{
    clear();
}


void GridIndex::clear()
{
    if (grids.empty())
        return;

    for (auto& grid : grids)
        delete grid;

    grids.clear();
}

/* 点查询 ： 格网 */
void GridIndex::queryGrids(float x, float y, Grid*& gridResult)
{
    for (auto& grid : grids) {
        if (grid->getExtent().contain(x, y)) {
            gridResult = grid;
            return;
        }
    }
}

/* 矩形框查询 ：格网 */
void GridIndex::queryGrids(const GeoExtent& extent, std::vector<Grid*>& gridsResult)
{
    for (auto& grid : grids) {
        if (grid->getExtent().isIntersect(extent)) {
            gridsResult.push_back(grid);
        }
    }
}

/* 点查询 ：要素 */
void GridIndex::queryFeatures(float x, float y, GeoObject*& featureResult)
{
    Grid* inGrid = nullptr;
    queryGrids(x, y, inGrid);
    if (!inGrid) {
        return;
    }

    std::cout << "in Grid: " << inGrid->getId() << std::endl;

    int count = inGrid->getFeatureCount();
    for (int i = 0; i < count; ++i) {
        GeoObject* feature = inGrid->getFeature(i);
        const char* geomType = feature->getType();
        if(strcmp(geomType,"POINT") == 0)
            return;
        if(strcmp(geomType,"POLYGON") == 0)
        {
            GeoPolygon* polygon = (GeoPolygon*)feature;
            if (isPointInPolygon({ x, y }, polygon)) {
                featureResult = feature;
                return;
            }
        }
        if(strcmp(geomType,"POLYLINE") == 0)
            return;
        if(strcmp(geomType,"MULTIPOINT") == 0)
            return;
        if(strcmp(geomType,"MULTIPOLYLINE") == 0)
            return;
        if(strcmp(geomType,"MULTIPOLYGON") == 0)
        {
            GeoMultiPolygon* multiPolygon = (GeoMultiPolygon*)feature;
            int polygonsCount = multiPolygon->polygons.size();
            GeoPolygon* polygon = nullptr;
            for (int j = 0; j < polygonsCount; ++j) {
                polygon = multiPolygon->polygons[j];
                if (isPointInPolygon({ x, y }, polygon)) {
                    featureResult = feature;
                    return;
                }
            }
        }
    }
}


/* 矩形查询 ：要素 */
void GridIndex::queryFeatures(const GeoExtent& extent, std::vector<GeoObject*>& featuresResult)
{
    std::vector<Grid*> grids;
    queryGrids(extent, grids);

    int gridsCount = grids.size();
    for (int i = 0; i < gridsCount; ++i) {
        int featuresCount = grids[i]->getFeatureCount();
        for (int j = 0; j < featuresCount; ++j) {
            GeoObject* feature = grids[i]->getFeature(j);
            const char* geomType = feature->getType();
            if(strcmp(geomType,"POINT") == 0)
                return;
            if(strcmp(geomType,"POLYGON") == 0)
            {
                GeoPolygon* polygon = (GeoPolygon*)feature;
                if (isPolygonRectIntersect(polygon, extent)) {
                    featuresResult.push_back(feature);
                }

            }
            if(strcmp(geomType,"POLYLINE") == 0)
                return;
            if(strcmp(geomType,"MULTIPOINT") == 0)
                return;
            if(strcmp(geomType,"MULTIPOLYLINE") == 0)
                return;
            if(strcmp(geomType,"MULTIPOLYGON") == 0)
            {
                GeoMultiPolygon* multiPolygon = (GeoMultiPolygon*)feature;
                int polygonsCount = multiPolygon->polygons.size();
                GeoPolygon* polygon = nullptr;
                for (int k = 0; k < polygonsCount; ++k) {
                    polygon = multiPolygon->polygons[k];
                    if (isPolygonRectIntersect(polygon, extent)) {
                        if (std::find(featuresResult.begin(), featuresResult.end(), feature) == featuresResult.end()) {
                            featuresResult.push_back(feature);
                        }
                    }
                }
            }
        }
    } // end for j
} // end for i

