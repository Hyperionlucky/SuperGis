#ifndef GEOOGLWIDGET_H
#define GEOOGLWIDGET_H
#pragma once

#include "geomap.h"
#include "OpenGLConfig/renderer.h"
#include "layerview.h"
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <vector>
#include <glm/glm.hpp>

struct FeatureVAO {
    FeatureVAO(int nFID, VertexArray* vaoIn = nullptr) : FID(nFID), VAO(vaoIn) {}
    ~FeatureVAO() { delete VAO; }
    void setVAO(VertexArray* vaoIn) {
        if (VAO)
            delete VAO;
        VAO = vaoIn;
    }
    int FID;
    VertexArray* VAO;
};

struct LayerVAO {
    LayerVAO(int nLID) : LID(nLID) {}
    ~LayerVAO() {
        for (auto& fVAO : fVAOs)
            delete fVAO;
    }
    void addFeatureVAO(FeatureVAO* fVAO) { fVAOs.push_back(fVAO); }
    void reserveFeatureVAO(int num) { fVAOs.reserve(num); }
    int LID;
    std::vector<FeatureVAO*> fVAOs;		// 必须用指针，否则会发生无谓的OpenGL调用
};

struct FeatureVBO {
    FeatureVBO(int nFID, VertexBuffer* vbo = nullptr) : FID(nFID), VBO(vbo) {}
    ~FeatureVBO() { delete VBO; }
    void setVBO(VertexBuffer* vboIn) { VBO = vboIn; }
    int FID;
    VertexBuffer* VBO;
};

struct LayerVBO
{
    LayerVBO(int nLID) :LID(nLID) {}
    ~LayerVBO() {
        for (auto& fVBO : fVBOs)
            delete fVBO;
    }
    void addFeatureVBO(FeatureVBO* fVBO) { fVBOs.push_back(fVBO); }
    void reserveFeatureVBO(int num) { fVBOs.reserve(num); }
    int LID;
    FeatureVBO* getFeatureVbo(int nFID)
    {
        for (auto& fvbo : fVBOs) {
            if (fvbo->FID == nFID) {
                return fvbo;
            }
        }
    }
    std::vector<FeatureVBO*> fVBOs;
};

struct FeatureIBO
{
    FeatureIBO(int nFID) : FID(nFID) {}
    ~FeatureIBO() {
        for (auto& ibo : IBOs)
            delete ibo;
    }
    void addIBO(IndexBuffer* ibo) { IBOs.push_back(ibo); }
    void reserveIBO(int num) { IBOs.reserve(num); }

    // 注意这里，一个对象可能有多个IBO
    // 比如一个polygon有一个外环+多个内环
    // 一个multiLineString有多个lineString
    int FID;
    std::vector<IndexBuffer*> IBOs;
};

struct LayerIBO
{
    LayerIBO(int nLID) : LID(nLID) {}
    ~LayerIBO() {
        for (auto& fIBO : fIBOs)
            delete fIBO;
    }
    void addFeatureIBO(FeatureIBO* fIBO) { fIBOs.push_back(fIBO); }
    void reserveFeatureIBO(int num) { fIBOs.reserve(num); }

    int LID;
    std::vector<FeatureIBO*> fIBOs;
};

struct FeatureColor
{
    FeatureColor(int id):FID(id){}
    ~FeatureColor(){}
    int FID;
    float r,g,b;
    bool isChanged = false;

};

class GeoOGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    GeoOGLWidget(QWidget* parent = nullptr);
    ~GeoOGLWidget() override;

    GeoPoint xy2screen(int screenX, int screenY);
    // 屏幕坐标到规范立方体坐标(-1.0f <= x,y,z <= 1.0f)
    GeoRawPoint screen2stdxy(int screenX, int screenY);
    LayerView* layerView = nullptr;
    //    KernelWidget* kernel;
    GeoLayer* geoLayer = nullptr;
    unsigned int pointCount = 0;
    void setMVP(const glm::mat4& mvp);
    void sendDataToGPU();
private:
    unsigned int* newContinuousNumber(unsigned int, unsigned int);


protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    virtual void mousePressEvent(QMouseEvent* ev) override;
    virtual void mouseMoveEvent(QMouseEvent* ev) override;
    virtual void mouseReleaseEvent(QMouseEvent* ev) override;
    virtual void wheelEvent(QWheelEvent* ev) override;
private slots:
    void SlotUpdateRGB();
    void slotReserveRGB(std::vector<GeoObject *>);
    void onSelectFeature(int,int);
    void onSelectFeature(int,std::vector<int>&);
    void slotRecoverColor();
   void slotDeleteLayer();
signals:
    void OpenIndexResult(GeoLayer*,std::vector<GeoObject*>);
    void reserveSelectFeature(std::vector<GeoObject *>);

private:
    // 绘制填充矩形
    void drawRectFillColor(const QPoint& startPoint, const QPoint& endPoint,
                           float r, float g, float b, float a = 1.0f);

    // 绘制矩形边框
    void drawRectNoFill(const QPoint& startPoint, const QPoint& endPoint,
                        float r = 0.0f, float g = 0.0f, float b = 0.0f, int lineWidth = 1);

    void updateFeatureColor(int);

    // 鼠标缩放时摄像机前后移动的距离
    float kDeltaZ = 2.0f;

    /********************* MVP *****************/
    // model
    glm::mat4 model;

    // view
    glm::vec3 camPos;		// 摄像机位置, 缩放、移动地图只需要改变摄像机位置就行
    glm::vec3 camFront;
    glm::vec3 camUp;
    glm::mat4 view;

    // project
    float zNear = 0.1f;
    float zFar = 100.0f;
    float fov = 45.0f;
    float tanFovDiv2;
    float aspectRatio;
    glm::mat4 proj;
    /***************** End MVP *****************/

    bool isRunning = true;

    int mouseLastPosX = 0;
    int mouseLastPosY = 0;

    Shader* shader = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb = nullptr;
    IndexBuffer* ib = nullptr;
    Renderer* renderer = nullptr;

    LayerVAO* layerVAO = nullptr;
    LayerVBO* layerVBO = nullptr;
    LayerIBO* layerIBO = nullptr;

    std::vector<FeatureColor*>featuresColor;

    QPoint beginPt,endPt;
    QPoint mouseCurrPos;
    bool isRectSelecting = false;
    FeatureColor* getFeatureColorByFID(int fid)
    {
        for(auto& featureColor:featuresColor)
        {
            if (featureColor->FID == fid)
                return featureColor;
        }
    }
    //    void pushSubData(VertexBuffer* vbo, const void* data, int sizeOffset, int size,
    //        FeatureIBO* fibo, int countOffset, int count);
    //  bool isLIDExist(int nLID);
};



#endif // GEOOGLWIDGET_H
