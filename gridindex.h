#ifndef GRIDINDEX_H
#define GRIDINDEX_H
#include "grid.h"

class GridIndex
{
public:
    GridIndex();
    ~GridIndex();

    /* 预分配网格数量 */
    void reserve(int numGrids) { grids.reserve(numGrids); }

    /* 添加网格 */
    void addGrid(Grid* grid) { grids.push_back(grid); }

    /* 清空索引 */
    void clear();

    /* 获取格网数量 */
    int getNumGrids() const { return grids.size(); }

    /* 查询格网 */
    void queryGrids(float x, float y, Grid*& gridResult);	// 点查询
    void queryGrids(const GeoExtent& extent, std::vector<Grid*>& gridsResult);	// 矩形框查询

    /* 查询空间要素 */
    void queryFeatures(float x, float y, GeoObject*& featureResult);
    void queryFeatures(const GeoExtent& extent, std::vector<GeoObject*>& featuresResult);

private:
    std::vector<Grid*> grids;
};

#endif // GRIDINDEX_H
