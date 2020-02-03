#ifndef GEOPOSTGISLINKER_H
#define GEOPOSTGISLINKER_H

#include <QWidget>
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include "gdal.h"
#include "geomap.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "geoobject.h"
#include "geomultipolygon.h"

namespace Ui {
class GeoPostGisLinker;
}

class GeoPostGisLinker : public QWidget
{
    Q_OBJECT

public:
    explicit GeoPostGisLinker(QWidget *parent = nullptr);
    ~GeoPostGisLinker();
    const char* wkbTypeToString(int enumValue);
    GDALDataset* poDS = nullptr;
    GeoMap* map = nullptr;

signals:
    void importNewLayers();

private:
    Ui::GeoPostGisLinker *ui;


private slots:
    void slotShowLayerList();
    void slotAddLayerToMap();
    void slotCancel();
    void slotCellCheckboxChanged();
};

#endif // GEOPOSTGISLINKER_H
