#include "geopostgislinker.h"
#include "ui_geopostgislinker.h"
#include "geolayer.h"
#include <QCheckBox>

GeoPostGisLinker::GeoPostGisLinker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeoPostGisLinker)
{
    ui->setupUi(this);
    setFixedSize(this->size());
    this->setWindowTitle("Link PostGIS");
    ui->Password_lineEdit->setEchoMode(QLineEdit::Password);  //show style of password
    ui->layersList->setColumnCount(4);
    ui->layersList->setColumnWidth(0,20);
    ui->UserName_lineEdit->setText("postgres");
    ui->Password_lineEdit->setText("1234567890");
    ui->Host_lineEdit->setText("47.93.249.247");
    ui->dbName_lineEdit->setText("2017301110048");
    ui->Port_lineEdit->setText("5432");
    connect(ui->ConnectButton,SIGNAL(clicked(bool)),this,SLOT(slotShowLayerList()));
    connect(ui->AddButton,SIGNAL(clicked(bool)),this,SLOT(slotAddLayerToMap()));
    connect(ui->CancelButton,SIGNAL(clicked(bool)),this,SLOT(slotCancel()));
}

GeoPostGisLinker::~GeoPostGisLinker()
{
    delete ui;
    //  delete map;
}

void GeoPostGisLinker :: slotShowLayerList()
{
    QString username = ui->UserName_lineEdit->text();
    QString password = ui->Password_lineEdit->text();
    QString ip = ui->Host_lineEdit->text();
    int port = ui->Port_lineEdit->text().toInt();
    QString database = ui->dbName_lineEdit->text();
    if (username.isEmpty())
    {
        QMessageBox::information(this,"warning","Username is null");
        return;
    }
    if (password.isEmpty()){
        QMessageBox::information(this,"warning",tr("Password is null"));
        return;
    }
    if (ip.isEmpty()){
        QMessageBox::information(this,"warning","HostIP is null");
        return;
    }
    if (ui->Port_lineEdit->text().isEmpty()){
        QMessageBox::information(this,"warning","Port is null");
        return;
    }
    if (database.isEmpty()){
        QMessageBox::information(this,"warning","Database is null");
        return;
    }

    qDebug()<< "here" << ip << " " << port << " " << username << " " << password << " " << database;
    OGRRegisterAll();
    char filepath[128] = { 0 };
    sprintf(filepath, "PG:dbname=%s host=%s port=%d user=%s password=%s",
            database.toLocal8Bit().constData(),
            ip.toLocal8Bit().constData(),
            port,
            username.toLocal8Bit().constData(),
            password.toLocal8Bit().constData()
            );

    // link postgis

    this->poDS = (GDALDataset*)GDALOpenEx(filepath, GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    if (!poDS) {
        QMessageBox::critical(nullptr, "Error", "Connect to postgresql error", QMessageBox::Ok);
        return;
    }
    // get numbers of layers(geotables)
    int layerCount = poDS->GetLayerCount();
    ui->layersList->setRowCount(layerCount);
    if (0 == layerCount) {
        GDALClose(poDS);
        return;
    }

    OGRLayer* poLayer = nullptr;

    // go through all tables
    for (int i = 0; i < layerCount; ++i) {
        poLayer = poDS->GetLayer(i);
        //poLayer->ResetReading();

        // first column: checkbox
        QCheckBox* checkBox = new QCheckBox(this);

        QHBoxLayout* hLayout = new QHBoxLayout(this);
        QWidget* widget = new QWidget(ui->layersList);
        hLayout->addWidget(checkBox);
        hLayout->setMargin(0);
        hLayout->setAlignment(checkBox, Qt::AlignCenter);
        widget->setLayout(hLayout);
        checkBox->setCheckState(Qt::Unchecked);
        ui->layersList->setCellWidget(i, 0, widget);
        connect(checkBox, SIGNAL(stateChanged(int)),
                this, SLOT(slotCellCheckboxChanged()));

        // second column: layer name
        QTableWidgetItem* layerName = new QTableWidgetItem();
        layerName->setFlags(layerName->flags() & (~Qt::ItemIsEditable));
        layerName->setTextAlignment(Qt::AlignCenter);
        layerName->setText(poLayer->GetName());
        ui->layersList->setItem(i, 1, layerName);

        // third column: feature count
        QTableWidgetItem* featureCount = new QTableWidgetItem();
        featureCount->setFlags(featureCount->flags() & (~Qt::ItemIsEditable));
        featureCount->setTextAlignment(Qt::AlignCenter);
        featureCount->setText(QString::number(poLayer->GetFeatureCount()));
        ui->layersList->setItem(i, 2, featureCount);

        // fourth column: feature geometry type
        QTableWidgetItem* geometryType = new QTableWidgetItem();
        geometryType->setFlags(featureCount->flags() & (~Qt::ItemIsEditable));
        geometryType->setTextAlignment(Qt::AlignCenter);
        geometryType->setText(wkbTypeToString(poLayer->GetGeomType()));
        ui->layersList->setItem(i, 3, geometryType);
    }
    ui->layersList->show();
    this->show();

    return;

}

void GeoPostGisLinker::slotAddLayerToMap(){
    std::cout << "here" << std::endl;
    if (!poDS) {
        QMessageBox::critical(this, "Error", "Not connected to server", QMessageBox::Ok);
        return;
    }

    int rowCount = ui->layersList->rowCount();
    if (rowCount == 0) {
        GDALClose(poDS);
        return;
    }

    for (int i = 0; i < rowCount; ++i) {
        if (!ui->layersList->cellWidget(i, 0))
            continue;

        QCheckBox* box = (QCheckBox*)(ui->layersList->cellWidget(i, 0)->children().at(1));
        if (!box || box->checkState() == Qt::Unchecked)
            continue;

        QTableWidgetItem* item = ui->layersList->item(i, 1);	// get layername
        QByteArray byteArray = item->text().toLocal8Bit();
        const char* layerName = byteArray.data();
        // The expression following is wrong
        //const char* layerName = item->text().toLocal8Bit().data();

        OGRLayer* poLayer = nullptr;
        GeoLayer* geoLayer = new GeoLayer();
        geoLayer->setLayerName(layerName);

        poLayer = poDS->GetLayerByName(layerName);
        if (!poLayer)
            continue;
        //poLayer->ResetReading();
        OGREnvelope evlop ;

        poLayer -> GetExtent(&evlop);
        geoLayer -> setLayerExtent(GeoExtent(evlop.MinX,evlop.MaxX,evlop.MinY,evlop.MaxY));
        OGRFeature* poFeature = nullptr;
        geoLayer -> featureCount = poLayer->GetFeatureCount();
        poFeature = poLayer->GetNextFeature();
        //read FieldDefns
        int fieldCount = poFeature->GetFieldCount();
        OGRFeatureDefn* poFDefn = poFeature->GetDefnRef();
        OGRFieldDefn* poFieldDefn = nullptr;
        geoLayer->fieldDefns = new std::vector<GeoFieldDefn*>;
        for(int i = 0; i < fieldCount; ++i)
        {
            poFieldDefn = poFDefn->GetFieldDefn(i);
            GeoFieldDefn* geoFieldDefn = new GeoFieldDefn();

            geoFieldDefn->setName(poFieldDefn->GetNameRef());
            geoFieldDefn->setWidth(poFieldDefn->GetWidth());
            geoFieldDefn->setType(geoLayer->convertOGRFieldType(poFieldDefn->GetType()));

            // 暂时只读取int、text、double类型的属性字段
            switch (poFieldDefn->GetType()) {
            default:
                delete geoFieldDefn;
                break;
            case OFTInteger:
                geoLayer->addField(geoFieldDefn);
                break;
            case OFTReal:
                geoLayer->addField(geoFieldDefn);
                break;
            case OFTString:
                geoLayer->addField(geoFieldDefn);
                break;
            }
        }
        poLayer->ResetReading();
        while (poFeature = poLayer->GetNextFeature())
        {
            //OGRFeatureDefn* poFDefn = poLayer->GetLayerDefn();
            OGRwkbGeometryType poGeoType =wkbFlatten(poFeature->GetGeometryRef()->getGeometryType());
            switch (poGeoType) {
            default:
                break;
            case wkbPoint:
                geoLayer->parseWkbPoint(poFeature);
                break;
            case wkbPolygon:
            {
                geoLayer->parseWkbPolygon(poFeature);
                break;
            }
            case wkbLineString:
            {
                geoLayer->parseWkbPolyline(poFeature);
                break;
            }
            case wkbMultiPoint:
                //        parseWkbMultipoint(poGeometry->toMultiPoint(), geoFeature);
                break;
            case wkbMultiPolygon:

            {
                geoLayer->parseWkbMultiPolygon(poFeature);
                break;
            }
            case wkbMultiLineString:

            {
                geoLayer->parseWkbMultiLineString(poFeature);
                break;
            }
            }
            //     geoLayer->addObjects(geoObject,geoObject->getType());
        }
        map->addLayer(geoLayer);
        OGRFeature::DestroyFeature(poFeature);
    } // end for

    // 触发信号， 通知主窗口更新左侧的图层列表
    emit importNewLayers();

    GDALClose(poDS);
    this->close();

}

void GeoPostGisLinker::slotCancel()
{
    if (poDS)
        GDALClose(poDS);

    this->close();	// close the postgislinker ui
}
void GeoPostGisLinker::slotCellCheckboxChanged()
{
    int rowCount = ui->layersList->rowCount();
    if (rowCount == 0)
        return;
    int checkedCount = 0;
    for (int i = 0; i < rowCount; ++i) {
        if (!ui->layersList->cellWidget(i, 0))
            continue;
        QCheckBox* box = (QCheckBox*)(ui->layersList->cellWidget(i, 0)->children().at(1));
        if (box && box->checkState() == Qt::Checked)
            checkedCount++;
    }
}

const char* GeoPostGisLinker::wkbTypeToString(int enumValue)
{
    switch (enumValue) {
    default: return "Unknown";
    case 0:  return "Unknown";
    case 1:  return "Point";
    case 2:  return "LineString";
    case 3:  return "Polygon";
    case 4:  return "MultiPoint";
    case 5:  return "MultiLineString";
    case 6:  return "MultiPolygon";
    case 7:  return "GeometryCollection";
    case 8:  return "CircularString";
    case 9:  return "CompoundCurve";
    case 10: return "CurvePolygon";
    case 11: return "MultiCurve";
    case 12: return "MultiSurface";
    case 13: return "Curve";
    case 14: return "Surface";
    case 15: return "PolyhedralSurface";
    case 16: return "TIN";
    case 17: return "Triangle";
    case 100: return "None";
    case 101: return "LinearRing";
        // more
        //case 1008:
        //case 1009:
        // ...
    }
}

