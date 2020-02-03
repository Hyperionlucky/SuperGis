#ifndef KERNELDENSITYTOOLWIDGET_H
#define KERNELDENSITYTOOLWIDGET_H
#include <QOpenGLWidget>
#include "geolayer.h"
#include <QMouseEvent>

class KernelDensityToolWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    KernelDensityToolWidget(GeoLayer*, QWidget* );
    ~KernelDensityToolWidget();

    void initializeGL();
    void paintGL();
    void resizeGL(int width,int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    GeoLayer* layer = nullptr;
    GeoPoint xy2screen(GeoPoint pt);
 //   GeoPoint2 xy2screen2(GeoPoint2 pt);
    float mouseX, mouseY, offsetX = 0.0, offsetY = 0.0, newOffsetX = 0.0, newOffsetY = 0.0;
    float scale=0.9;

};

#endif // KERNELDENSITYTOOLWIDGET_H
