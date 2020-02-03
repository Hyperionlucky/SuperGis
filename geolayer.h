#ifndef GEOLAYER_H
#define GEOLAYER_H
#include <QString>
#include <QDebug>
#include <vector>
#include <algorithm>
#include <QVector>
#include <QTextStream>
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "geopoint.h"
#include "geopolyline.h"
#include "geopolygon.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include "geomultipolygon.h"
#include <QMap>
#include <QXmlStreamReader>
#include "geomultilinestring.h"
#include "geobase.hpp"
#include "gridindex.h"
#include "geofielddefn.h"
using namespace std;
class GeoLayer
{
public:
    GeoLayer();
    ~GeoLayer();
    double left,top,right,bottom;
    QXmlStreamReader xmlReader;
    bool isEdited = false;
    void calculateKde();
    void convertObjToKdePts();
    std::vector<GeoPoint> kdePts;
    int kdeCalculated=0;
    double bandWidth;
    float **loc;
    float maxLoc;
    float minLoc;
    bool isSend = false;
    int featureCount;
    std::vector<GeoFieldDefn*>* fieldDefns = nullptr;
public:
    std::vector<GeoObject*> geoObjects;
    int count = 0;

    //sld
    void readSld(const char* filePath);
    void readNamedLayer();
    void readUserStyle();
    void readFeatureTypeStyle();
    void readRule();
    void readPropertyName();
    void readPolygonSymbolizer();
    void readFill();
    void readSvgParameter();
    void skipUnknownElement();


    void readShp(const char* filepath);
    void readGeojson(const char* filePath);

    void addObjects(GeoObject*);
    GeoObject* getObjectByFID(int FID);

    void readJsonFeatures(QJsonObject );
    void readJsonPolygon(QJsonObject );
    void readJsonPolyline(QJsonObject );
    void readJsonPoint(QJsonObject );
    void readJsonMultiPolygon(QJsonObject);

    GeoFieldType convertOGRFieldType(OGRFieldType type);
    void parseWkbPoint(OGRFeature* poFeature);
    void parseWkbMultiPolygon(OGRFeature* poFeature);
    void parseWkbPolygon(OGRFeature* poFeature);
    void parseWkbPolyline(OGRFeature* poFeature);
    void parseWkbMultiLineString(OGRFeature* poFeature);

    void setLayerName(QString layername){this->layerName = layername;}
    void setId(unsigned int Id){this->id = Id;}
    QString getLayerName(){return this->layerName;}
    unsigned int getId(){return this->id;}
    void setVisible(bool isVisible);
    bool getVisible();
    void setProperties(const char* propertiesIn){strncpy_s(properties, 16, propertiesIn, strlen(propertiesIn));}
    const char* getProperties(){return this->properties;}
    double getBoundaryWidth(){return this->width;}
    double getBoundaryHeight(){return this->height;}
    double getCenterX(){return this->centerX;}
    double getCenterY(){return this->centerY;}
    GeoExtent getLayerExtent();
    void setLayerExtent(GeoExtent);

    bool createGridIndex();
    void queryFeatures(float x, float y, GeoObject*& featureResult) const;
    void queryFeatures(const GeoExtent& extent, std::vector<GeoObject*>& featuresResult) const;


    //FieldDefn
    int addField(const QString& nameIn, int widthIn, GeoFieldType typeIn);
    int addField(GeoFieldDefn* fieldDefnIn);
    std::vector<GeoFieldDefn*>* getFieldDefns() const
        { return fieldDefns; }
    GeoFieldDefn* getFieldDefn(int idx) const { return (*fieldDefns)[idx]; }
    GeoFieldDefn* getFieldDefn(const QString& name) const;
    int getFieldIndex(const QString& name, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int getFieldIndexLike(const QString& name, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    size_t getNumFields() const { return fieldDefns->size(); }

private:
    QString layerName;
    unsigned int id = 0;
    unsigned indexFID = 0;
    bool isVisible;
    char properties[16] = {0};
    double width,height,centerX,centerY;
    GeoExtent boundary;
    GridIndex* gridIndex = nullptr;
    int sldFeatureCount = 0;

};

#endif // GEOLAYER_H
