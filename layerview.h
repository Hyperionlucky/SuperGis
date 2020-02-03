#ifndef LAYERVIEW_H
#define LAYERVIEW_H

#include <QWidget>
#include "geomap.h"
#include <QTreeWidgetItem>
#include "Dialog/layerattributetabledialog.h"
#include "Dialog/kernelcaculatedialog.h"
namespace Ui {
class LayerView;
}

class LayerView : public QWidget
{
    Q_OBJECT

public:
    explicit LayerView(QWidget *parent = nullptr);
    ~LayerView();

    void createActions();
    void createMenu();
    QTreeWidgetItem *item = nullptr;
    QTreeWidgetItem* curItem;
    QMenu* popMenu;

    GeoMap* map;

    int loc,deletesize;
private:
    Ui::LayerView *ui;
private slots:
    void  slotShowLayerTree();
    void itemClick();
    void slotfeatureSelected(int nLID, std::vector<int>& nFIDS)
    {
        emit featureSelected(nLID,nFIDS);
    }
    void slotUnsave()
    {
        emit unsave();
    }
public slots:
    //  void updateMyTreeWidgetSlot(CGeoMap *map);
    void deleteIt();
    void slotShowPopMenu(const QPoint&);
//    void serProp();
    void openAttri();
    void getColorAndWidth(QColor fillColor,QColor strokeColor,float width);
    void showIndexGrids();
    void hideIndexGrids();
    void analyzeKDE();
    void getAttribute(int layerID,QString attribute);  // 得到用于分级设色的属性
    void SlotAddSldToLayer();

signals:
    void showAttri(GeoLayer* layer);
    void IndexGrids();
    void KDEAnalyze(int layerID);
    void updateLayerView();
    void drawSelectedLayer();
    void updateRGB();
    void updateInputTextMeaning();
    void removeMap();
    void featureSelected(int ,std::vector<int>&);
    void unsave();
private:
    LayerAttributeTableDialog* attributeTableDialog;
    KernelCaculateDialog* kdeDialog;
};

#endif // LAYERVIEW_H
