#ifndef GEOFIELDDEFN_H
#define GEOFIELDDEFN_H


#include <QString>

enum GeoFieldType {
    kFieldInt = 0,
    kFieldDouble,
    kFieldText,
    kFieldUnknown
};

/*****************************************************/
/*                                                   */
/*         GeoFieldDefn                              */
/*            属性表中每个字段的定义                  */
/*            名称、宽度、类型                        */
/*                                                   */
/*****************************************************/

class GeoFieldDefn {
public:
    GeoFieldDefn(QString nameIn, int widthIn, GeoFieldType typeIn)
        : width(widthIn), type(typeIn), name(nameIn) {}
    GeoFieldDefn() = default;
    ~GeoFieldDefn() = default;

    bool isSame(const GeoFieldDefn& rhs)
        { return name == rhs.name; }
    bool operator==(const GeoFieldDefn& rhs)
        { return name == rhs.name; }

    GeoFieldType getType() const { return type; }
    QString getName() const { return name; }
    int getWidth() const { return width; }

    void setType(GeoFieldType typeIn) { this->type = typeIn; }
    void setWidth(int widthIn) { this->width = widthIn; }
    void setName(QString nameIn) { name = nameIn; }

private:
    int width = 0;						// 字段宽度
    GeoFieldType type = kFieldUnknown;	// 字段类型，int，double，text ...
    QString name;
};


#endif // GEOFIELDDEFN_H
