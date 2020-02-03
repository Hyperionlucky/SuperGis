#ifndef GEOOBJECT_H
#define GEOOBJECT_H

#include "geobase.hpp"
#include "geofielddefn.h"
#include <vector>
#include <QString>
//#include <vld.h>
class GeoObject
{
public:
    GeoObject() ;
    virtual ~GeoObject() ;
public:
    virtual void setId(unsigned int id) = 0;
    virtual unsigned int getId() = 0;
    virtual void setType(const char*type) = 0;
    virtual const char*getType() = 0;
    virtual GeoExtent getExtent() = 0;
    void initNewFieldValue();
    std::vector<GeoFieldDefn*>* fieldDefns = nullptr;
    template<typename T>
    bool getField(QString name, T* outValue) const {
        return getField(getFieldIndexByName(name), outValue);
    }

    template<typename T>
    bool getField(int idx, T* outValue) const {
        if (checkFieldIndex(idx)) {
            *outValue = *(T*)fieldValues[idx];
            return true;
        }
        else {
            return false;
        }
    }

    template<typename T>
    void setField(int idx, T valueIn) {
        if (checkFieldIndex(idx)) {
            initNewFieldValue();
            *(T*)(fieldValues[idx]) = valueIn;
        }
    }

    template<typename T>
    void setField(QString name, T valueIn) {
        setField(getFieldIndexByName(name), valueIn);
    }
    int getNumFields() const { return (*fieldDefns).size(); }
    QString getFieldName(int idx) const;
    GeoFieldType getFieldType(int idx) const;
    GeoFieldType getFieldType(const QString& name) const;
    // 字段是否存在
    bool isFieldExist(const QString& fieldName, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    // 字段是否存在（模糊匹配）
    bool isFieldExistLike(const QString& fieldName, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int getFieldIndexByName(const QString& name) const;
protected:

    std::vector<void*> fieldValues;
private:
    // 检查下标索引的合法性
    bool checkFieldIndex(int idx) const
    { return idx > -1 && idx < (*fieldDefns).size(); }
    bool checkFieldName(const QString& name) const;


};

#endif // GEOOBJECT_H
