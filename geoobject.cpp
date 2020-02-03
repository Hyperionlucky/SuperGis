#include "geoobject.h"

GeoObject::GeoObject()
{

}
GeoObject::~GeoObject()
{
    if(!fieldDefns)
        return;
    else
    {
        int count = fieldDefns->size();
        for (int i = 0; i < count; ++i) {
            GeoFieldType fieldType = (*fieldDefns)[i]->getType();
            switch (fieldType) {
            case kFieldInt:
                delete (int*)fieldValues[i];
                break;
            case kFieldDouble:
                delete (double*)fieldValues[i];
                break;
            case kFieldText:
                delete (QString*)fieldValues[i];
                break;
            case kFieldUnknown:
                break;
            default:
                break;
            }
        }
    }
}
void GeoObject::initNewFieldValue()
{
    size_t numFieldDefns = fieldDefns->size();
    size_t numFieldValues = fieldValues.size();
    for (size_t i = numFieldValues; i < numFieldDefns; ++i) {
        switch ((*fieldDefns)[i]->getType()) {
        default:
            break;
        case kFieldInt:
            fieldValues.push_back(new int(0));
            break;
        case kFieldDouble:
            fieldValues.push_back(new double(0.0));
            break;
        case kFieldText:
            fieldValues.push_back(new QString());
            break;
        }
    }
}
QString GeoObject::getFieldName(int idx) const
{
    return (*fieldDefns)[idx]->getName();
}

GeoFieldType GeoObject::getFieldType(int idx) const
{
    return (*fieldDefns)[idx]->getType();
}

GeoFieldType GeoObject::getFieldType(const QString& name) const
{
    int idx = getFieldIndexByName(name);
    if (idx != -1)
        return getFieldType(idx);
    else
        return kFieldUnknown;
}

bool GeoObject::checkFieldName(const QString& name) const
{
    return getFieldIndexByName(name) != -1;
}

// 字段是否存在
bool GeoObject::isFieldExist(const QString& fieldName, Qt::CaseSensitivity cs /*= Qt::CaseSensitive*/) const
{
    size_t count = (*fieldDefns).size();
    for (size_t i = 0; i < count; ++i) {
        if ((*fieldDefns)[i]->getName().compare(fieldName, cs) == 0) {
            return true;
        }
    }
    return false;
}

// 字段是否存在（模糊匹配）
bool GeoObject::isFieldExistLike(const QString& fieldName, Qt::CaseSensitivity cs /*= Qt::CaseSensitive*/) const
{
    size_t count = (*fieldDefns).size();
    for (size_t i = 0; i < count; ++i) {
        if ((*fieldDefns)[i]->getName().contains(fieldName, cs)) {
            return true;
        }
    }
    return false;
}

int GeoObject::getFieldIndexByName(const QString& name) const
{
    size_t count = (*fieldDefns).size();
    for (unsigned int i = 0; i < count; ++i) {
        if ((*fieldDefns)[i]->getName() == name) {
            return i;
        }
    }
    return -1;
}
