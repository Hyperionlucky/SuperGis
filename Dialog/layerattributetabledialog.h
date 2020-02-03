#ifndef LAYERATTRIBUTETABLEDIALOG_H
#define LAYERATTRIBUTETABLEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QAction>
#include "geolayer.h"

class LayerAttributeTableDialog : public QDialog
{
    Q_OBJECT

public:
    LayerAttributeTableDialog(GeoLayer* layerIn, QWidget *parent);
    ~LayerAttributeTableDialog();

signals:
    void featureSelected(int,std::vector<int>&);
    void closed();
    void unsave();

public slots:
    void onSelectRows();

public:
    void createWidgets();
    void createActions();
    void createToolBar();
    void setupLayout();

private:
    static void readAttributeTable(GeoLayer* layer, QTableWidget* tableWidget);

public:
    GeoLayer* layer;

    // widgets
    QTableWidget* tableWidget;

    // tooBra
    QToolBar* toolBar;

    // actions
    QAction* removeRecorsdAction;
protected:
    virtual void closeEvent(QCloseEvent* event) override;
};


#endif // LAYERATTRIBUTETABLEDIALOG_H
