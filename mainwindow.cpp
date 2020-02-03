#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "layerview.h"
#include <QFileDialog>
#include <QHBoxLayout>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, true);
    //ui->GeoSearch_button->setShortcut(Qt::Key_Return);
    //ui->lineEdit->setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint);

  //  ui->GeoSearch_button->setWindowFlags(Qt::Widget|Qt::WindowStaysOnTopHint);
    // ui->GeoSearch_button->setShortcut(QKeySequence::InsertParagraphSeparator);
  //  connect(ui->GeoSearch_button,SIGNAL(clicked(bool)),this,SLOT(slotOnclicked()));
    connect(this,SIGNAL(readshp()),this,SLOT(slotSetLayerTree()));

    geoMap = new GeoMap();
    openGlWidget = new GeoOGLWidget(this);

    layerView = new LayerView();
    layerView->map = this->geoMap;
    connect(this,SIGNAL(importLayers()),layerView,SLOT(slotShowLayerTree()));
    connect(layerView,&LayerView::drawSelectedLayer,this,&MainWindow::slotDrawSelectedLayer);
    connect(layerView,SIGNAL(updateRGB()),openGlWidget,SLOT(SlotUpdateRGB()));
    connect(layerView,SIGNAL(featureSelected(int,std::vector<int>&)),openGlWidget,SLOT(onSelectFeature(int,std::vector<int>&)));
    connect(layerView,SIGNAL(unsave()),openGlWidget,SLOT(slotRecoverColor()));
    connect(layerView,SIGNAL(removeMap()),openGlWidget,SLOT(slotDeleteLayer()));
    searchWidget = new GlobalSearchWidget(geoMap, openGlWidget);
    isFileOpen = true;
    connect(searchWidget,SIGNAL(onReadShp()),this,SLOT(SlotOpenShapeFile()));
    connect(searchWidget,SIGNAL(onReadGeoJSON()),this,SLOT(SlotOpenGeoJSON()));
    connect(searchWidget,SIGNAL(onLinkToPostGis()),this,SLOT(SlotLinkToPostGis()));
    connect(layerView,SIGNAL(updateInputTextMeaning()),searchWidget,SLOT(slotUpdateInputTextMeaning()));
    connect(searchWidget, SIGNAL(featureSelected(int, int)),openGlWidget, SLOT(onSelectFeature(int, int)));
    connect(searchWidget,SIGNAL(unSaveColor()),openGlWidget,SLOT(slotRecoverColor()));
    connect(openGlWidget,SIGNAL(OpenIndexResult(GeoLayer*,std::vector<GeoObject*>)),searchWidget,SLOT(slotOpenIndex(GeoLayer*,std::vector<GeoObject*>)));
    setupLayout();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete postGisLinker;
    delete geoMap;
    layerView->close();
    delete layerView;
}


void MainWindow::slotSetLayerTree()
{
    layerView->setGeometry(22,249,242,597);
    layerView->show();
    emit importLayers();
}

void MainWindow::slotDrawSelectedLayer()
{

    for (size_t i = 0; i < this->geoMap->geolayers.size(); i++)
    {
        if (geoMap->geolayers[i]->isSend == true)
            continue;
        else
        {
            openGlWidget->geoLayer = this->geoMap->geolayers[i];
            openGlWidget->sendDataToGPU();
        }
    }
    openGlWidget->update();
}

void MainWindow::setupLayout()
{
    QWidget* centerWidget = new QWidget();
    this->setCentralWidget(centerWidget);
   // setCentralWidget(openGlWidget);
    searchWidget->setGeometry(280, 40, 200, 31);
    QHBoxLayout* mainLayout = new QHBoxLayout(centerWidget);
    mainLayout->setSpacing(6);
    mainLayout->addWidget(openGlWidget);
    mainLayout->setContentsMargins(11, 11, 11, 11);
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 3);

}

void MainWindow::SlotOpenShapeFile()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("shapefile(*.shp)"));

    if (filepath.isEmpty())
        return;
    const char* str;
    QByteArray ba = filepath.toLatin1();
    str = ba.data();
    GeoLayer *layer = new GeoLayer();
    layer->readShp(str);
    geoMap->addLayer(layer);
    emit readshp();
}

void MainWindow::SlotOpenGeoJSON()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("json files(*.json)"));
    if (filepath.isEmpty())
        return;
    QFileInfo fi = QFileInfo(filepath);
    QString fileName = fi.baseName();
    const char* str;
    const char* str1;
    QByteArray ba = filepath.toLatin1();
    QByteArray ba1 = fileName.toLatin1();
    str = ba.data();
    str1 = ba1.data();
    GeoLayer *layer = new GeoLayer();

    layer->setLayerName(fileName);
    layer->readGeojson(str);
    geoMap->addLayer(layer);
    emit readshp();



}
void MainWindow::SlotLinkToPostGis()
{
    if(postGisLinker != nullptr)
    {
        postGisLinker->close();
        delete postGisLinker;
        disconnect(postGisLinker,&GeoPostGisLinker::importNewLayers,this,&MainWindow::slotSetLayerTree);
    }
    else
    {
        postGisLinker = new GeoPostGisLinker();
        postGisLinker->map = this->geoMap;
        postGisLinker->show();
        connect(postGisLinker,&GeoPostGisLinker::importNewLayers,this,&MainWindow::slotSetLayerTree);

    }
}

