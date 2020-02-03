#include "layerview.h"
#include "ui_layerview.h"
#include "geooglwidget.h"
#include <QMenu>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QFileDialog>

LayerView::LayerView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerView)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    createActions();
    createMenu();
    ui->treeWidget->setHeaderLabel("Layers");
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);  //设置枚举值
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(slotShowPopMenu(const QPoint&)));
    connect(this, SIGNAL(updateLayerView()),this,SLOT(slotShowLayerTree()));

}

LayerView::~LayerView()
{
    delete ui;
}

void LayerView::slotShowLayerTree()
{
    ui->treeWidget->clear();
    for (size_t i = 0; i < map->geolayers.size(); i++)
    {
        item = new QTreeWidgetItem(ui->treeWidget,QStringList(map->geolayers[i]->getLayerName()));
        if(map->geolayers[i]->getVisible())
            item->setCheckState(0,Qt::Checked);
        else
            item->setCheckState(0,Qt::Unchecked);
    }
    emit drawSelectedLayer();
}

void LayerView::slotShowPopMenu(const QPoint& pos)//槽函数
{

    curItem = ui->treeWidget->itemAt(pos);

    if (curItem == nullptr)
        return;
    //curItem = ui.treeWidget->currentItem();  //获取当前被点击的节点
    //如果没有点击到节点
    popMenu->exec(QCursor::pos());
}

void LayerView::createActions(){
    //connect(ui.view_this, &QAction::triggered, this, &MyTreeWidget::viewIt);
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(itemClick()),Qt::UniqueConnection);
    //	connect(this,SIGNAL(showAttri(CGeoLayer*)),attri,SLOT(showAttri(CGeoLayer*)),Qt::UniqueConnection);
}

void LayerView::itemClick(){
    QTreeWidgetItemIterator it(ui->treeWidget);
    int count = 0;
    while (*it) {
        if((*it)->checkState(0) == Qt::Checked){
            map -> geolayers[count] -> setVisible(true);
        }
        else{
            map -> geolayers[count] -> setVisible(false);
        }
        ++it;
        count++;
    }
    emit updateLayerView();
}


void LayerView::createMenu(){
    popMenu = new QMenu(ui->treeWidget);
    QAction* act2 = popMenu->addAction("deleteIt");
    connect(act2, SIGNAL(triggered(bool)), this, SLOT(deleteIt()));
    QAction* act3 = popMenu->addAction("readSld");
    connect(act3, SIGNAL(triggered(bool)), this, SLOT(SlotAddSldToLayer()));
    QAction* act4 = popMenu->addAction("Open Attribute table");
    connect(act4, SIGNAL(triggered(bool)), this, SLOT(openAttri()));
    QAction* act5 = popMenu->addAction("begin GridIndex");
    connect(act5, SIGNAL(triggered(bool)), this, SLOT(showIndexGrids()));
    QAction* act6 = popMenu->addAction("end GridIndex");
    connect(act6, SIGNAL(triggered(bool)), this, SLOT(hideIndexGrids()));
    QAction* act7 = popMenu->addAction("KDE");
    connect(act7, SIGNAL(triggered(bool)), this, SLOT(analyzeKDE()));
    //    QAction* act8 = popMenu->addAction("Layer Colours");
    //    connect(act8, SIGNAL(triggered(bool)), this, SLOT(layerColours()));
}

void LayerView::openAttri(){
    QModelIndex index = ui->treeWidget->currentIndex();
    int layerID = index.row();
    GeoLayer *layer = map->geolayers[layerID];
    // 属性表展示
    //  emit showAttri(layer);
    if (layer) {
        attributeTableDialog = new LayerAttributeTableDialog(layer, ui->treeWidget);
        connect(attributeTableDialog,SIGNAL(featureSelected(int,std::vector<int>&)),this,SLOT(slotfeatureSelected(int,std::vector<int>&)));
        connect(attributeTableDialog,SIGNAL(unsave()),this,SLOT(slotUnsave()));
        QPoint pos = this->mapToGlobal(this->pos());
        attributeTableDialog->setGeometry(pos.x()+800, pos.y() , 800, 600);
        attributeTableDialog->show();
    }
}

void LayerView::showIndexGrids(){
    QModelIndex index = ui->treeWidget->currentIndex();
    int layerID = index.row();
    GeoLayer *layer = map->geolayers[layerID];
    //	layer->showIndexGrid = true;
    //   emit IndexGrids();
    layer->createGridIndex();
    layer->isEdited = true;
    emit updateInputTextMeaning();
}

void LayerView::hideIndexGrids(){
    QModelIndex index = ui->treeWidget->currentIndex();
    int layerID = index.row();
    GeoLayer *layer = map->geolayers[layerID];
    layer->isEdited = false;
    emit updateInputTextMeaning();
}

void LayerView::getColorAndWidth(QColor fillColor,QColor strokeColor,float width){
    QModelIndex index = ui->treeWidget->currentIndex();
    int layerID = index.row();
    //	emit sendColorAndWidthData(layerID,fillColor,strokeColor,width);
}

void LayerView::getAttribute(int layerID,QString attribute){
    //	emit setLayerCol(layerID,attribute);
}


//右键选项的执行函数
void LayerView::deleteIt()
{
    // 删除map的第几个
    QModelIndex index = ui->treeWidget->currentIndex();
    int layerID = index.row();
    map->deleteLayerAt(layerID);
    emit updateLayerView();
    emit removeMap();
}

void LayerView::analyzeKDE(){
    // 删除map的第几个
    QModelIndex index = ui->treeWidget->currentIndex();
    int layerID = index.row();

    if(strcmp(map->geolayers[layerID]->getProperties() , "POINT") != 0){
        QMessageBox::critical(NULL, QString::fromLocal8Bit("不支持的类型"), QString::fromLocal8Bit("仅支持对点要素图层进行核密度分析"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    kdeDialog = new KernelCaculateDialog(map->geolayers[layerID], nullptr);//ui->treeWidget);
    QPoint pos = this->mapToGlobal(this->pos());
    kdeDialog->setGeometry(pos.x()+800, pos.y(), 800, 600);
    kdeDialog->show();

}


void LayerView::SlotAddSldToLayer()
{
    QModelIndex index = ui->treeWidget->currentIndex();
    int layerID = index.row();
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Styled Layer Descriptor(*.sld)"));
    QFileInfo fi = QFileInfo(filepath);
    QString fileName = fi.baseName();
    const char* str;
    const char* str1;
    QByteArray ba = filepath.toLatin1();
    QByteArray ba1 = fileName.toLatin1();
    str = ba.data();
    str1 = ba1.data();
    if (strcmp(str1,(map->geolayers[layerID]->getLayerName().toLatin1().data())) == 0)
    {
        map->geolayers[layerID]->readSld(str);
        emit updateRGB();
    }
    else
    {
        QMessageBox::information(this,"warning","Please select the relevant sld file!");
        return;
 }
}
