#pragma once
#include "geolayer.h"
#include "geomath.h"
#include <iostream>
#include <QXmlStreamReader>
#include <fstream>
#include <QFile>
#include <QMessageBox>

using namespace std;

GeoLayer :: GeoLayer()
{
    this->isVisible = true;
}
GeoLayer :: ~GeoLayer()
{
    size_t size = geoObjects.size();
    for(unsigned i = 0; i < size; i++)
    {
        delete geoObjects[i];
    }
    if (fieldDefns) {
        for (auto& fieldDefn : *fieldDefns) {
            delete fieldDefn;
        }
        delete fieldDefns;
    }
    if (gridIndex)
        delete gridIndex;
    if(kdeCalculated==1)
    {
        for(int i=0;i<1000;i++)
        {
            delete[] loc[i];
            loc[i]=nullptr;
        }
        delete[] loc;
        loc=nullptr;
    }

}

void GeoLayer :: calculateKde(){

    convertObjToKdePts();
    //bandWidth
    vector<double> Distance;
    float Avex = 0,Avey = 0;
    //calculate the average center of the input points
    for(unsigned int i = 0; i < this->kdePts.size(); i++)
    {
        Avex += kdePts[i].x;
        Avey += kdePts[i].y;
    }
    Avex /= this->kdePts.size();
    Avey /= this->kdePts.size();
    //calculate the distance from the (weighted) average center of all points
    for (size_t i = 0; i<this->kdePts.size(); i++)
    {
        double Dis = sqrt(pow(kdePts[i].x-Avex, 2.0) + pow(kdePts[i].y-Avey, 2.0));
        Distance.push_back(Dis);
    }
    sort(Distance.begin(),Distance.end());
    //calculate the (weighted) median of these distances
    double Dm = Distance.at(Distance.size()/2);
    //calculate (weighted) standard distance
    float SDx = 0,SDy = 0;
    for (size_t i = 0; i < this->kdePts.size(); i++)
    {
        SDx += pow(kdePts[i].x-Avex,2);
        SDy += pow(kdePts[i].y-Avey,2);
    }
    float SD = sqrt(SDx/this->kdePts.size()+SDy/this->kdePts.size());
    if ( SD > (sqrt(1/log(2))*Dm))
        this->bandWidth = 0.9*(sqrt(1/log(2))*Dm)*pow(this->kdePts.size(),-0.2);
    else
        this->bandWidth = 0.9*SD*pow(this->kdePts.size(),-0.2);
    //generate raster
    loc = new float*[1000];
    for (int i=0;i<1000;i++)
    {
        loc[i] = new float[1000];
    }
    //calculateKde
    float temp;
    double dis2;
    float width = (this->right-this->left)/1000;
    float height = (this->top-this->bottom)/1000;
    for (int i = 0; i<1000;i++)
    {
        float x,y;
        x = this->left+width*i+width/2;
        for (int j = 0; j<1000; j++)
        {
            temp = 0;
            y = this->bottom+height*j+height/2;
            for (size_t m = 0; m < this->kdePts.size(); m++)
            {
                dis2 = pow(x-this->kdePts[m].x,2)+pow(y-this->kdePts[m].y,2);
                if (dis2<(this->bandWidth*this->bandWidth))
                {
                    temp += 3/3.1415926*pow((1-dis2/pow(this->bandWidth,2)),2);
                }
            }
            temp = temp/this->kdePts.size()/pow(this->bandWidth,2);
            loc[i][j] = temp;
            if (i == 0&&j == 0)
            {
                maxLoc = minLoc = temp;
            }
            else if (temp>maxLoc)
                maxLoc = temp;
            else if (temp<minLoc)
                minLoc=temp;
        }
    }
    this->kdeCalculated=1;

}

void GeoLayer :: addObjects(GeoObject* geoObject)
{
    geoObjects.push_back(geoObject);
    geoObjects.back()->setId(indexFID++);
    setProperties(geoObject->getType());
}


void GeoLayer :: readGeojson(const char* filePath)
{
    QFile loadFile(filePath);
    //判断是否能够打开json文件
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "could't open projects json";
        return;
    }
    //全部读取后关闭
    QByteArray allData = loadFile.readAll();
    loadFile.close();
    //将读出的内容转换成QJsonDocuments类

    this->fieldDefns = new std::vector<GeoFieldDefn*>;
    GeoFieldDefn* geoFieldDefn0 = new GeoFieldDefn();
    geoFieldDefn0 ->setName("level");
    geoFieldDefn0 ->setWidth(20);
    geoFieldDefn0 ->setType(kFieldText);
    this->addField(geoFieldDefn0);
    GeoFieldDefn* geoFieldDefn1 = new GeoFieldDefn();
    geoFieldDefn1->setName("name");
    geoFieldDefn1 ->setWidth(20);
    geoFieldDefn1 ->setType(kFieldText);
    this->addField(geoFieldDefn1);

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!";
        return;
    }

    //将QJsonDocument类转换为一个具体的对象QJsonObject
    QJsonObject rootObj = jsonDoc.object();
    //通过对对象QJsonObject进行操作，可以直接获取第一级的一对：键-值
    if(rootObj.contains(QStringLiteral("features")))
    {

        readJsonFeatures(rootObj);
    }
}

void GeoLayer :: readJsonFeatures(QJsonObject rootObj)
{
    //一级键值为包含不同类型要素（二级键）的数组
    QJsonValue arrayValue = rootObj.value(QStringLiteral("features"));
    if(arrayValue.isArray())
    {
        QJsonArray array = arrayValue.toArray();
        for(int i = 0; i < array.size(); i++)
        {
            QJsonValue featureValue = array.at(i);
            QJsonObject featureObj = featureValue.toObject();
            //从二级键值的feature中进入三级键（点线面要素的区别存在于三级键）
            if (featureObj.contains(QStringLiteral("geometry")))
            {
                QJsonObject subObject = featureObj.value(QStringLiteral("geometry")).toObject();
                //Point
                readJsonPoint(subObject);
                //Polyline
                readJsonPolyline(subObject);
                //Polygon
                readJsonPolygon(subObject);
                //MultiPolygon
                readJsonMultiPolygon(subObject);
            }
            //Field Table ------Only be used to read "china.json"
            if (featureObj.contains(QStringLiteral("properties")))
            {

                QJsonObject subObject = featureObj.value(QStringLiteral("properties")).toObject();
                this->geoObjects[i]->fieldDefns = this->fieldDefns;
                this->geoObjects[i]->initNewFieldValue();
                this->geoObjects[i]->setField(0, subObject["level"].toString());
                this->geoObjects[i]->setField(1, subObject["name"].toString());

            }
        }
        this->featureCount = this->geoObjects.size();
        boundary = this->geoObjects[0]->getExtent();
        for (int i = 0; i < featureCount; i++)
        {
            GeoExtent extent = this->geoObjects[i]->getExtent();
            boundary.merge(extent);
        }
        this->setLayerExtent(boundary);
    }
}

void GeoLayer :: readJsonPoint(QJsonObject subObject)
{
    if(subObject["type"].toString() == "Point")
    {
        QJsonArray pointArray = subObject.value(QStringLiteral("coordinates")).toArray();

        GeoPoint* pt = new GeoPoint();
        pt->x = pointArray.at(0).toDouble();
        pt->y = pointArray.at(1).toDouble();
        //     points.push_back(pt);
        geoObjects.push_back(pt);
    }
    else return;
}

void GeoLayer :: readJsonPolyline(QJsonObject subObject)
{
    if(subObject["type"].toString() == "LineString")
    {

        GeoPolyline* polyline = new GeoPolyline();
        QJsonArray polylineArray = subObject.value(QStringLiteral("coordinates")).toArray();
        for(int i = 0;i < polylineArray.size();i++)
        {
            QJsonValue ptVal = polylineArray.at(i);
            QJsonArray coordinatesArray = ptVal.toArray();
            for(int j = 0; j < coordinatesArray.size(); j++)
            {
                GeoPoint pt;
                pt.x = coordinatesArray.at(0).toDouble();
                pt.y = coordinatesArray.at(1).toDouble();
                polyline->addPoint(pt.x,pt.y);
            }
        }
        this->addObjects(polyline);
    }
    else return;
}

void GeoLayer :: readJsonPolygon(QJsonObject subObject)
{
    if(subObject["type"].toString() == "Polygon")
    {
        GeoPolygon* polygon = new GeoPolygon();
        QJsonArray polylineArray = subObject.value(QStringLiteral("coordinates")).toArray();
        for(int i = 0;i < polylineArray.size();i++)
        {
            GeoPolyline* polyline = new GeoPolyline();
            QJsonValue ptVal = polylineArray.at(i);
            QJsonArray coordinatesArray = ptVal.toArray();
            for(int j = 0;j < coordinatesArray.size();j++)
            {
                QJsonArray elementsArray = QJsonValue(coordinatesArray.at(j)).toArray();
                GeoPoint pt;
                pt.x = elementsArray.at(0).toDouble();
                pt.y = elementsArray.at(1).toDouble();
                polyline->addPoint(pt.x,pt.y);

            }
            polygon->addPolyline(polyline);
        }
        this->addObjects(polygon);
    }
    else return;
}

void GeoLayer :: readJsonMultiPolygon(QJsonObject subObject)
{
    if(subObject["type"].toString() == "MultiPolygon")
    {
        GeoMultiPolygon* multiPolygon = new GeoMultiPolygon();
        //GeoPolygon* polygon = new GeoPolygon();
        QJsonArray polygonArray = subObject.value(QStringLiteral("coordinates")).toArray();
        multiPolygon->reserve(polygonArray.size());
        for (int pCount = 0; pCount < polygonArray.size(); pCount++)
        {
            GeoPolygon* polygon = new GeoPolygon();
            QJsonArray polylineArray = polygonArray.at(pCount).toArray();
            for(int i = 0;i < polylineArray.size();i++)
            {
                GeoPolyline* polyline = new GeoPolyline();
                QJsonValue ptVal = polylineArray.at(i);
                QJsonArray coordinatesArray = ptVal.toArray();
                polyline->reserve(coordinatesArray.size());
                for(int j = 0;j < coordinatesArray.size();j++)
                {
                    QJsonArray elementsArray = QJsonValue(coordinatesArray.at(j)).toArray();
                    GeoPoint pt;
                    pt.x = elementsArray.at(0).toDouble();
                    pt.y = elementsArray.at(1).toDouble();
                    polyline->addPoint(pt.x,pt.y);

                }
                polygon->addPolyline(polyline);
            }
            multiPolygon->addPolygon(polygon);
        }
        this->addObjects(multiPolygon);
    }
    else return;
}


void GeoLayer :: setVisible(bool isVisible){
    this->isVisible = isVisible;
}
bool GeoLayer :: getVisible(){
    return this->isVisible;
}
void GeoLayer :: readShp(const char* filePath)
{
    //注册所有驱动
    clock_t start = clock();
    GDALAllRegister();
    //建立GDALDataset对象
    GDALDataset *poDS = nullptr;
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	// 支持中文路径
    CPLSetConfigOption("SHAPE_ENCODING","");  //解决中文乱码问题
    //读取shp文件
    poDS = (GDALDataset*) GDALOpenEx(filePath,GDAL_OF_VECTOR,nullptr,nullptr,nullptr);

    if( poDS == nullptr )
    {
        qDebug()<<"Open failed.\n%s";
        return;
    }

    OGRLayer  *poLayer = nullptr;
    poLayer = poDS->GetLayer(0); //
    this->layerName = poLayer->GetName();
    //获取该要素的范围
    OGREnvelope evlop ;
    poLayer->GetExtent(&evlop);
    boundary = GeoExtent(evlop.MinX,evlop.MaxX,evlop.MinY,evlop.MaxY);
    this->setLayerExtent(boundary);

    OGRFeature* poFeature = nullptr;
    this->featureCount = poLayer->GetFeatureCount();
    poFeature = poLayer->GetNextFeature();
    //读取属性表表头
    int fieldCount = poFeature->GetFieldCount();
    OGRFeatureDefn* poFDefn = poFeature->GetDefnRef();
    OGRFieldDefn* poFieldDefn = nullptr;
    this->fieldDefns = new std::vector<GeoFieldDefn*>;
    for(int i = 0; i < fieldCount; ++i)
    {
        poFieldDefn = poFDefn->GetFieldDefn(i);
        GeoFieldDefn* geoFieldDefn = new GeoFieldDefn();

        geoFieldDefn->setName(poFieldDefn->GetNameRef());
        geoFieldDefn->setWidth(poFieldDefn->GetWidth());
        geoFieldDefn->setType(convertOGRFieldType(poFieldDefn->GetType()));

        // 暂时只读取int、text、double类型的属性字段
        switch (poFieldDefn->GetType()) {
        default:
            delete geoFieldDefn;
            break;
        case OFTInteger:
            this->addField(geoFieldDefn);
            break;
        case OFTReal:
            this->addField(geoFieldDefn);
            break;
        case OFTString:
            this->addField(geoFieldDefn);
            break;
        }
    }
    clock_t end0 = clock();
    qDebug()<<"Time of creating table column :"<<(end0-start)/double(CLK_TCK)<<" s"<<endl;
    // int index = 0;
    poLayer->ResetReading();
    while (poFeature = poLayer->GetNextFeature())
    {
        //  clock_t startEveryFeature = clock();
        OGRwkbGeometryType poGeoType =wkbFlatten(poFeature->GetGeometryRef()->getGeometryType());
        switch (poGeoType) {
        default:
            break;
        case wkbPoint:
        {
            parseWkbPoint(poFeature);
            break;
        }
        case wkbPolygon:
        {   parseWkbPolygon(poFeature);
            break;
        }
        case wkbLineString:
        {
            parseWkbPolyline(poFeature);
            break;
        }
        case wkbMultiPoint:
            break;
        case wkbMultiPolygon:
        {
            parseWkbMultiPolygon(poFeature);
            break;
        }
        case wkbMultiLineString:
        {
            parseWkbMultiLineString(poFeature);
            break;
        }
        }
    }
    OGRFeature::DestroyFeature(poFeature);
    GDALClose(poDS);
    clock_t end = clock();
    qDebug()<<" Time of reading shp:" << (end - start) / double(CLK_TCK) << "s" <<endl;
} // end for

void GeoLayer :: parseWkbMultiPolygon(OGRFeature* poFeature)
{

    OGRMultiPolygon* multiPolygon = (OGRMultiPolygon*)poFeature->GetGeometryRef();
    //multipolygon
    GeoMultiPolygon* geomultipolygons = new GeoMultiPolygon();
    geomultipolygons->fieldDefns = this->fieldDefns;
    geomultipolygons->initNewFieldValue();
    OGRPolygon* poPolygon;
    geomultipolygons->reserve(multiPolygon->getNumGeometries());
    for(int ipolygon = 0; ipolygon < multiPolygon->getNumGeometries(); ipolygon ++)
    {
        poPolygon = (OGRPolygon*)(multiPolygon->getGeometryRef(ipolygon));
        OGRPoint ptTemp;
        //int numberOfInnerRings = poPolygon->getNumInteriorRings();
        OGRLinearRing* poExteriorRing = poPolygon->getExteriorRing();
        int numberOfExteriorRingVertices = poExteriorRing->getNumPoints();
        GeoPolygon* geoPolygon = new GeoPolygon();
        GeoPolyline* geoPolyline = new GeoPolyline();
        geoPolyline->reserve(numberOfExteriorRingVertices);
        for (int k = 0; k < numberOfExteriorRingVertices; ++k)
        {
            poExteriorRing->getPoint(k, &ptTemp);
            float x,y;
            x = ptTemp.getX();
            y = ptTemp.getY();
            geoPolyline->addPoint(x, y);
        }
        geoPolygon->addPolyline(geoPolyline);
        geomultipolygons->addPolygon(geoPolygon);
    }

    int fCount = poFeature->GetFieldCount();
    OGRFeatureDefn* poFeatureDefn = poFeature->GetDefnRef();
    OGRFieldDefn* poFieldDefn = nullptr;
    for (int i = 0, j = 0; i < fCount; ++i) {
        poFieldDefn = poFeatureDefn->GetFieldDefn(i);
        // 暂时只读取int、text、float类型的属性字段
        switch (poFieldDefn->GetType()) {
        default:
            break;
        case OFTInteger:
            geomultipolygons->setField(j++, poFeature->GetFieldAsInteger(i));
            break;
        case OFTReal:
            geomultipolygons->setField(j++, poFeature->GetFieldAsDouble(i));
            break;
        case OFTString:
            if (poFeature->IsFieldNull(i))
                geomultipolygons->setField(j++, QString(""));
            else
                geomultipolygons->setField(j++, QString(poFeature->GetFieldAsString(i)));
            break;
        }
    }
    this->addObjects(geomultipolygons);

}
void GeoLayer::parseWkbPolygon(OGRFeature* poFeature)
{
    OGRPolygon* poPolygon = (OGRPolygon *)poFeature->GetGeometryRef();
    OGRPoint ptTemp;
    //int numberOfInnerRings = poPolygon->getNumInteriorRings();
    OGRLinearRing* poExteriorRing = poPolygon->getExteriorRing();
    int numberOfExteriorRingVertices = poExteriorRing->getNumPoints();
    GeoPolygon* geoPolygon = new GeoPolygon();
    geoPolygon->fieldDefns = this->fieldDefns;
    geoPolygon->initNewFieldValue();

    GeoPolyline* geoPolyline= new GeoPolyline();
    geoPolyline->reserve(numberOfExteriorRingVertices);
    for (int k = 0; k < numberOfExteriorRingVertices; ++k) {
        poExteriorRing->getPoint(k, &ptTemp);
        geoPolyline->addPoint(ptTemp.getX(), ptTemp.getY());

    }
    geoPolygon->addPolyline(geoPolyline);

    int fCount = poFeature->GetFieldCount();
    OGRFeatureDefn* poFeatureDefn = poFeature->GetDefnRef();
    OGRFieldDefn* poFieldDefn = nullptr;
    for (int i = 0, j = 0; i < fCount; ++i) {
        poFieldDefn = poFeatureDefn->GetFieldDefn(i);
        // 暂时只读取int、text、float类型的属性字段
        switch (poFieldDefn->GetType()) {
        default:
            break;
        case OFTInteger:
            geoPolygon->setField(j++, poFeature->GetFieldAsInteger(i));
            break;
        case OFTReal:
            geoPolygon->setField(j++, poFeature->GetFieldAsDouble(i));
            break;
        case OFTString:
            if (poFeature->IsFieldNull(i))
                geoPolygon->setField(j++, QString(""));
            else
                geoPolygon->setField(j++, QString(poFeature->GetFieldAsString(i)));
            break;
        }
    }
    this->addObjects(geoPolygon);
    return;
}
void GeoLayer::parseWkbPoint(OGRFeature* poFeature)
{
    OGRPoint* poPoint = (OGRPoint* )poFeature->GetGeometryRef();
    GeoPoint* point = new GeoPoint(poPoint->getX(),poPoint->getY());
    point->fieldDefns = this->fieldDefns;
    point->initNewFieldValue();
    int fCount = poFeature->GetFieldCount();
    OGRFeatureDefn* poFeatureDefn = poFeature->GetDefnRef();
    OGRFieldDefn* poFieldDefn = nullptr;
    for (int i = 0, j = 0; i < fCount; ++i) {
        poFieldDefn = poFeatureDefn->GetFieldDefn(i);
        // 暂时只读取int、text、float类型的属性字段
        switch (poFieldDefn->GetType()) {
        default:
            break;
        case OFTInteger:
            point->setField(j++, poFeature->GetFieldAsInteger(i));
            break;
        case OFTReal:
            point->setField(j++, poFeature->GetFieldAsDouble(i));
            break;
        case OFTString:
            if (poFeature->IsFieldNull(i))
                point->setField(j++, QString(""));
            else
                point->setField(j++, QString(poFeature->GetFieldAsString(i)));
            break;
        }
    }
    this->addObjects(point);
}
void GeoLayer::parseWkbPolyline(OGRFeature* poFeature)
{
    OGRLineString* poLineString = (OGRLineString *)poFeature->GetGeometryRef();
    GeoPolyline* polyline = new GeoPolyline();
    OGRPoint pt;
    int numOfPolylines = poLineString->getNumPoints();
    for (int i = 0; i < numOfPolylines; i++)
    {

        poLineString->getPoint(i,&pt);
        polyline->addPoint(pt.getX(),pt.getY());
    }

    polyline->fieldDefns = this->fieldDefns;
    polyline->initNewFieldValue();
    int fCount = poFeature->GetFieldCount();
    OGRFeatureDefn* poFeatureDefn = poFeature->GetDefnRef();
    OGRFieldDefn* poFieldDefn = nullptr;
    for (int i = 0, j = 0; i < fCount; ++i) {
        poFieldDefn = poFeatureDefn->GetFieldDefn(i);
        // 暂时只读取int、text、float类型的属性字段
        switch (poFieldDefn->GetType()) {
        default:
            break;
        case OFTInteger:
            polyline->setField(j++, poFeature->GetFieldAsInteger(i));
            break;
        case OFTReal:
            polyline->setField(j++, poFeature->GetFieldAsDouble(i));
            break;
        case OFTString:
            if (poFeature->IsFieldNull(i))
                polyline->setField(j++, QString(""));
            else
                polyline->setField(j++, QString(poFeature->GetFieldAsString(i)));
            break;
        }
    }
    this->addObjects(polyline);
}

void GeoLayer :: parseWkbMultiLineString(OGRFeature* poFeature)
{
    OGRMultiLineString* poMultiLineString = (OGRMultiLineString *)(poFeature->GetGeometryRef());
    OGRLineString* poLineString;
    GeoMultiLineString* multiLineString = new GeoMultiLineString();
    int numOfLineString = poMultiLineString->getNumGeometries();
    multiLineString->reserve(numOfLineString);
    for (int i = 0; i < numOfLineString; i++)
    {
        poLineString = (OGRLineString* )poMultiLineString->getGeometryRef(i);
        GeoPolyline* geoPolyline= new GeoPolyline();
        OGRPoint pt;
        int numOfPolylines = poLineString->getNumPoints();
        for (int j = 0; j < numOfPolylines; j++)
        {
            poLineString->getPoint(j,&pt);
            geoPolyline->addPoint(pt.getX(),pt.getY());
        }
        multiLineString->addPolyline(geoPolyline);
    }

    multiLineString->fieldDefns = this->fieldDefns;
    multiLineString->initNewFieldValue();
    int fCount = poFeature->GetFieldCount();
    OGRFeatureDefn* poFeatureDefn = poFeature->GetDefnRef();
    OGRFieldDefn* poFieldDefn = nullptr;
    for (int i = 0, j = 0; i < fCount; ++i) {
        poFieldDefn = poFeatureDefn->GetFieldDefn(i);
        // 暂时只读取int、text、float类型的属性字段
        switch (poFieldDefn->GetType()) {
        default:
            break;
        case OFTInteger:
            multiLineString->setField(j++, poFeature->GetFieldAsInteger(i));
            break;
        case OFTReal:
            multiLineString->setField(j++, poFeature->GetFieldAsDouble(i));
            break;
        case OFTString:
            if (poFeature->IsFieldNull(i))
                multiLineString->setField(j++, QString(""));
            else
                multiLineString->setField(j++, QString(poFeature->GetFieldAsString(i)));
            break;
        }
    }
    this->addObjects(multiLineString);
}

void GeoLayer::readSld(const char*filePath)
{
    QFile *file =new QFile(filePath);
    //  file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!file->open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::information(NULL, QString("title"), QString("open error!"));
        return   ;
    }

    xmlReader.setDevice(file);

    while(!xmlReader.atEnd())
    {
        if(xmlReader.isStartElement())
        {
            //   qDebug()<<xmlReader.name();
            QString startElementName = xmlReader.name().toString();
            if(startElementName.compare("NamedLayer") == 0)
            {
                xmlReader.readNext();
                readNamedLayer();
            }
            else{
                xmlReader.readNext();
            }
        }
        else {
            xmlReader.readNext();
        }
    }
    qDebug()<< sldFeatureCount;

    file->close();
    if(xmlReader.hasError()){
        return ;
    }else if(file->error() != QFile::NoError){
        return ;
    }
    return ;
}
void GeoLayer::readNamedLayer()
{
    xmlReader.readNext();
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isEndElement())
        {
            xmlReader.readNext();
            break;
        }
        if(xmlReader.isStartElement())
        {
            //     qDebug()<<xmlReader.name();
            QString startElementName = xmlReader.name().toString();
            if(startElementName.compare("UserStyle") == 0)
            {
                readUserStyle();
            }
            else skipUnknownElement();
        } else
            xmlReader.readNext();
    }

}
void GeoLayer::readUserStyle()
{
    xmlReader.readNext();
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isEndElement())
        {
            xmlReader.readNext();
            break;
        }
        if(xmlReader.isStartElement())
        {
            //      qDebug()<<xmlReader.name();
            QString startElementName = xmlReader.name().toString();
            if(startElementName.compare("FeatureTypeStyle") == 0)
            {
                readFeatureTypeStyle();
            }
            else skipUnknownElement();
        } else
            xmlReader.readNext();
    }
}

void GeoLayer::readFeatureTypeStyle()
{

    xmlReader.readNext();
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isEndElement())
        {
            xmlReader.readNext();
            break;
        }
        if(xmlReader.isStartElement())
        {
            //      qDebug()<<xmlReader.name();
            QString startElementName = xmlReader.name().toString();
            if(startElementName.compare("Rule") == 0)
            {
                //     qDebug()<<xmlReader.readElementText();
                readRule();
            }
            else skipUnknownElement();
        } else
            xmlReader.readNext();
    }
}

void GeoLayer::readRule()
{
    xmlReader.readNext();
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isEndElement())
        {
            xmlReader.readNext();
            break;
        }
        if(xmlReader.isStartElement())
        {
            //       qDebug()<<xmlReader.name();
            QString startElementName = xmlReader.name().toString();
            if(startElementName.compare("Name") == 0)
            {
                readPropertyName();

            }
            else if(startElementName.compare("PolygonSymbolizer") == 0)
            {
                readPolygonSymbolizer();
            }
            else
                skipUnknownElement();
        } else
        {
            xmlReader.readNext();
        }
    }
}
void GeoLayer::readPropertyName()
{
    QString propertiesName = xmlReader.readElementText();
    // qDebug()<<propertiesName<<endl;
    bool flags = false;
    for(int i = 0; i < featureCount; i++)
    {
        GeoPolygon* polygon = (GeoPolygon*)geoObjects[i];
        int fieldNumber = polygon->getNumFields();
        for(int j = 0; j < fieldNumber; j++)
        {
            if (polygon->getFieldType(j) == kFieldText)
            {
                //   QString currentField = polygon->getFieldName(j);
                QString fieldName;
                polygon->getField(j,&fieldName);
                //    qDebug()<<fieldName<<endl;
                if(propertiesName.compare(fieldName) == 0)
                {
                    count = i;
                    flags = true;
                    break;
                }

            }
        }
        if (flags)
            break;
    }
    if(xmlReader.isEndElement()){
        xmlReader.readNext();
    }
}
void GeoLayer::readPolygonSymbolizer()
{
    xmlReader.readNext();
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isEndElement())
        {
            xmlReader.readNext();
            break;
        }
        if(xmlReader.isStartElement())
        {
            //     qDebug()<<xmlReader.name();
            QString startElementName = xmlReader.name().toString();
            if(startElementName.compare("Fill") == 0)
                readFill();
            else skipUnknownElement();

        } else
            xmlReader.readNext();
    }
}

void GeoLayer::readFill()
{
    xmlReader.readNext();
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isEndElement())
        {
            xmlReader.readNext();
            break;
        }
        if(xmlReader.isStartElement())
        {
            //    qDebug()<<xmlReader.name();
            QString startElementName = xmlReader.name().toString();
            if(startElementName.compare("SvgParameter") == 0)
            {
                readSvgParameter();
                sldFeatureCount ++;
                //       qDebug()<<count<<endl;
            }
            else skipUnknownElement();

        } else
            xmlReader.readNext();
    }
}
void GeoLayer::readSvgParameter()
{
    QString name = xmlReader.attributes().value("name").toString();
    if(name.compare("fill") == 0){
        QString fillColor = xmlReader.readElementText();
        if (strcmp(this->geoObjects[count]->getType(),"POLYGON") == 0)

        {
            GeoPolygon* polygon = (GeoPolygon*)(this->geoObjects[count]);
            polygon->fillColor = (QColor)fillColor;
            //       qDebug()<<polygon->fillColor.redF()  <<polygon->fillColor.greenF() <<polygon->fillColor.blueF();
        }
        else if (strcmp(this->geoObjects[count]->getType(),"MULTIPOLYGON") == 0)
        {
            GeoMultiPolygon* multiPolygon = (GeoMultiPolygon*)(this->geoObjects[count]);
            multiPolygon->fillColor = (QColor)fillColor;
        }
    }
    if(xmlReader.isEndElement()){
        xmlReader.readNext();
    }
}

void GeoLayer::skipUnknownElement(){
    xmlReader.readNext();
    //没有结束
    while (!xmlReader.atEnd()) {
        //结束
        if(xmlReader.isEndElement()){
            xmlReader.readNext();
            break;
        }
        //继续跳过读取。
        if(xmlReader.isStartElement()){
            skipUnknownElement();
        }else {
            xmlReader.readNext();
        }
    }
}


bool GeoLayer::createGridIndex()
{
    clock_t start = clock();
    // 图层范围
    GeoExtent layerExtent = this->getLayerExtent();

    // 网格大小取空间要素外包络矩形平均大小的3倍
    double gridWidth = 0;
    double gridHeight = 0;
    int featuresCount = geoObjects.size();
    for (int i = 0; i < featuresCount; ++i) {
        GeoExtent extent = geoObjects[i]->getExtent();
        gridWidth += extent.width();
        gridHeight += extent.height();
    }
    gridWidth = (gridWidth / featuresCount) * 3;
    gridHeight = (gridHeight / featuresCount) * 3;
    // 调整网格大小使其能够均分
    gridWidth = layerExtent.width() / floor(layerExtent.width() / gridWidth + 0.5);
    gridHeight = layerExtent.height() / floor(layerExtent.height() / gridHeight + 0.5);

    // 网格行列数
    int row = floor(layerExtent.height() / gridHeight + 0.5);
    int col = floor(layerExtent.width() / gridWidth + 0.5);

    if (gridIndex)
        delete gridIndex;
    gridIndex = new GridIndex();
    gridIndex->reserve(row * col);

    // 添加网格
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < col; ++j) {
            Grid* grid = new Grid(i * col + j);
            gridIndex->addGrid(grid);
            GeoExtent gridExtent(layerExtent.minX + j * gridWidth, layerExtent.minX + (j + 1) * gridWidth,
                                 layerExtent.minY + i * gridHeight, layerExtent.minY + (i + 1) * gridHeight);
            grid->setExtent(gridExtent);
            // 遍历每个feature
            for (int k = 0; k < featuresCount; ++k) {
                GeoObject* feature = geoObjects[k];
                // 先简单判断一下feature的边界是否和该网格相交
                // 如果不相交，则该feature就不会和该网格相交
                if (!gridExtent.isIntersect(feature->getExtent())) {
                    continue;
                }
                const char* geomType = feature->getType();
                if(strcmp(geomType,"POINT") == 0)
                {
                    GeoPoint* point = (GeoPoint*) feature;
                    if (gridExtent.contain(point->x, point->y)) {
                        grid->addFeature(feature);
                    }
                }
                if(strcmp(geomType,"MULTIPOINT") == 0)
                {
                    return false;
                }
                if (strcmp(geomType,"POLYLINE") == 0)
                {
                    GeoPolyline* lineString = (GeoPolyline*)feature;
                    if (isLineStringRectIntersect(lineString, gridExtent)) {
                        grid->addFeature(feature);
                    }
                }
                if (strcmp(geomType,"MULTIPOLYLINE") == 0)
                {
                    return false;
                }
                if (strcmp(geomType,"POLYGON") == 0)
                {
                    GeoPolygon* polygon = (GeoPolygon*) feature;
                    if (isPolygonRectIntersect(polygon, gridExtent)) {
                        grid->addFeature(feature);
                    }
                }
                if (strcmp(geomType,"MULTIPOLYGON") == 0)
                {
                    GeoMultiPolygon* multiPolygon = (GeoMultiPolygon*) feature;
                    int polygonsCount = multiPolygon->polygons.size();
                    for (int i = 0; i < polygonsCount; ++i) {
                        //  GeoExtent extent = multiPolygon->polygons[i]->getExtent();
                        if (isPolygonRectIntersect(multiPolygon->polygons[i], gridExtent)) {
                            grid->addFeature(feature);
                        }
                    }
                }
            } // end for k
        } // end for j
    } // end for i

    clock_t end = clock();
    qDebug()<<"Time of creating gridIndex is "<<(end-start)/ double CLK_TCK<<endl;
    return true;
}

GeoExtent GeoLayer::getLayerExtent(){
    return boundary;
}

void GeoLayer :: setLayerExtent(GeoExtent extent)
{
    boundary = extent;
    this->centerX = boundary.centerX();
    this->centerY = boundary.centerY();
    this->width   = boundary.width();
    this->height  = boundary.height();
    this->left = boundary.minX;
    this->right = boundary.maxX;
    this->bottom = boundary.minY;
    this->top = boundary.maxY;
}
void GeoLayer::queryFeatures(float x, float y, GeoObject*& featureResult) const
{
    if (gridIndex) {
        gridIndex->queryFeatures(x, y, featureResult);
    }
}

void GeoLayer::queryFeatures(const GeoExtent& extent, std::vector<GeoObject*>& featuresResult) const
{
    if (gridIndex) {
        gridIndex->queryFeatures(extent, featuresResult);
    }
}

GeoFieldDefn* GeoLayer::getFieldDefn(const QString& name) const
{
    size_t fieldsCount = fieldDefns->size();
    for (unsigned int i = 0; i < fieldsCount; ++i) {
        if ((*fieldDefns)[i]->getName() == name)
            return (*fieldDefns)[i];
    }
    return nullptr;
}

int GeoLayer::getFieldIndex(const QString& name, Qt::CaseSensitivity cs /*= Qt::CaseSensitive*/) const
{
    size_t fieldCount = fieldDefns->size();
    for (unsigned int iField = 0; iField < fieldCount; ++iField) {
        if ((*fieldDefns)[iField]->getName().compare(name, cs) == 0)
            return iField;
    }
    return -1;
}

int GeoLayer::getFieldIndexLike(const QString& name, Qt::CaseSensitivity cs /*= Qt::CaseSensitive*/) const
{
    qDebug() << name;
    size_t fieldCount = fieldDefns->size();
    for (unsigned int iField = 0; iField < fieldCount; ++iField) {
        qDebug() << (*fieldDefns)[iField]->getName();

        if ((*fieldDefns)[iField]->getName().contains(name, cs))
            return iField;
    }
    return -1;
}

int GeoLayer::addField(GeoFieldDefn* fieldDefnIn)
{
    int index = getFieldIndex(fieldDefnIn->getName());
    if (index != -1) // 已经存在
        return index;
    else {
        fieldDefns->push_back(fieldDefnIn);
        for (auto& feature : geoObjects) {
            feature->initNewFieldValue();
        }
        return fieldDefns->size() - 1;
    }
}



int GeoLayer::addField(const QString& nameIn, int widthIn, GeoFieldType typeIn)
{
    return addField(new GeoFieldDefn(nameIn, widthIn, typeIn));
}

GeoFieldType GeoLayer::convertOGRFieldType(OGRFieldType type)
{
    switch (type) {
    default:			return kFieldUnknown;
    case OFTInteger:	return kFieldInt;
    case OFTReal:		return kFieldDouble;
    case OFTString:		return kFieldText;
    }
}

GeoObject* GeoLayer :: getObjectByFID(int FID)
{
    for(int i = 0; i < featureCount; i++)
    {
        if (int(geoObjects[i]->getId()) == FID)
        {
            return geoObjects[i];
        }

    }
    return nullptr;
}

void GeoLayer ::convertObjToKdePts()
{
    if (strcmp(this->getProperties(),"POINT") != 0)
        return ;
    kdePts.reserve(featureCount);
    for(int i = 0; i < featureCount; i++)
    {
        GeoPoint* pt = (GeoPoint *)geoObjects[i];
        kdePts.push_back(GeoPoint(pt->x,pt->y));
    }
}
