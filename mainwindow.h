#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "geopostgislinker.h"
#include "layerview.h"
#include "geooglwidget.h"
#include "Widget/globalsearchwidget.h"
#include "Dialog/globalsearchresult.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    GeoMap * geoMap;
signals:
    void importLayers();
    void readshp();

private slots:
  //  void slotOnclicked();
    void slotSetLayerTree();
    void slotDrawSelectedLayer();
    void SlotOpenShapeFile();
    void SlotOpenGeoJSON();
    void SlotLinkToPostGis();


private:
    Ui::MainWindow *ui;
    GeoPostGisLinker * postGisLinker = nullptr;
    LayerView *layerView = nullptr;
    GeoOGLWidget *openGlWidget = nullptr;
    GlobalSearchWidget* searchWidget = nullptr;
    void setupLayout();
    bool isFileOpen;
};

#endif // MAINWINDOW_H
