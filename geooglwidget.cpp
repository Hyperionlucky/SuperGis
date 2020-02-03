#include "OpenGLConfig/vertexbufferlayout.h"

#include "geooglwidget.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGLConfig/earcut.hpp"
#include <iostream>
#include <array>
#include <QDebug>
#include <ctime>



GeoOGLWidget::GeoOGLWidget(QWidget* parent) :
    QOpenGLWidget(parent)
{
    geoLayer = nullptr;
    tanFovDiv2 = tan(glm::radians(fov / 2.0f));
    //    layerView = new LayerView();
    //  kernel = new KernelWidget();
    //connect(this,SIGNAL(KDEAnalyze(float ,float **,float ,float  )),kernel,SLOT(KDEAnalyze(float ,float **,float ,float )));
    connect(this,SIGNAL(reserveSelectFeature(std::vector<GeoObject *>)),this,SLOT(slotReserveRGB(std::vector<GeoObject *>)));
}

GeoOGLWidget::~GeoOGLWidget()
{
    // very important
    makeCurrent();
    isRunning = false;

    if (renderer)
        delete renderer;
    if(shader)
        delete shader;
    if(layerVBO)
        delete layerVBO;
    if(layerVAO)
        delete layerVAO;
    if(layerIBO)
        delete layerIBO;
    for(const auto& featureColor:featuresColor)
        delete featureColor;
}

void GeoOGLWidget::initializeGL()
{
    if (glewInit() != GLEW_NO_ERROR) {
        std::cout << "Glew init failed" << std::endl;
        return;
    }

    shader = new Shader("OpenGLConfig/vertexshader.glsl", "OpenGLConfig/fragmentshader.glsl");
    renderer = new Renderer;
    renderer->Clear();
}

void GeoOGLWidget::resizeGL(int w, int h) {
    if(!geoLayer)
        return;
    GLCall(glViewport(0, 0, w, h));
    GLCall(glMatrixMode(GL_PROJECTION));

    if (h == 0)
        h = 1;

    //the ratio of the width and height of the window
    aspectRatio = float(w) / float(h);

    //Perspective projection
    float layerBoundaryRatio = geoLayer->getBoundaryWidth()/geoLayer->getBoundaryHeight();
    float camPosZ = 0.0f;
    if (layerBoundaryRatio < this->aspectRatio)
        camPosZ = geoLayer->getBoundaryHeight() / (2 * tanFovDiv2);
    else
        camPosZ = geoLayer->getBoundaryWidth()  / (2 * this->aspectRatio * tanFovDiv2);

    proj = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
    view = glm::lookAt(camPos, camPos + camFront, camUp);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    glm::mat4 mvp = proj * view * model;

    shader->Bind();
    shader->SetUniformMat4f("u_MVP", mvp);
    shader->Unbind();
}

void GeoOGLWidget::paintGL()
{
    renderer->Clear();

    if(!geoLayer)
        return;

    if (!isRunning)
        return;
    shader->Bind();

    if (geoLayer->getVisible()) {
        const auto& featureVAOs = (layerVAO)->fVAOs;
        const auto& featureIBOs = (layerIBO)->fIBOs;
        auto featureVaoIter = featureVAOs.begin();
        auto featureIboIter = featureIBOs.begin();

        while (featureVaoIter != featureVAOs.end() && featureIboIter != featureIBOs.end()) {
            for (auto& ibo : (*featureIboIter)->IBOs) {
                renderer->Draw(*(*featureVaoIter)->VAO, *ibo, *shader);
                //return;
            }

            ++featureVaoIter;
            ++featureIboIter;
        }
    }
    if(isRectSelecting)
    {
        shader->Unbind();
        drawRectNoFill(beginPt, mouseCurrPos, 1.0f, 0.5f, 0.0f, 5);
        drawRectFillColor(beginPt, mouseCurrPos, 1.0f, 0.5f, 0.0f, 0.5f);
    }
}

void GeoOGLWidget::setMVP(const glm::mat4& mvp)
{
    shader->Bind();
    shader->SetUniformMat4f("u_MVP", mvp);
    shader->Unbind();
}



GeoPoint GeoOGLWidget::xy2screen(int screenX, int screenY)
{
    // 鼠标点在规范立方体中的坐标
    // 即 x:[-1, 1]  y:[-1, 1]  z:[-1, 1]
    float stdX = (2.0f * screenX / this->width()) - 1;
    float stdY = (2.0f * (this->height() - screenY) / this->height()) - 1;

    float geoX = stdX * camPos.z * aspectRatio * tanFovDiv2 + camPos.x;
    float geoY = stdY * camPos.z * tanFovDiv2 + camPos.y;

    return { geoX, geoY };
}
GeoRawPoint GeoOGLWidget::screen2stdxy(int screenX, int screenY)
{
    float stdX = (2.0f * screenX / this->width()) - 1;
    float stdY = (2.0f * (this->height() - screenY) / this->height()) - 1;
    return { stdX, stdY };
}
void GeoOGLWidget::mousePressEvent(QMouseEvent* ev)
{
    beginPt = endPt = ev->pos();
}

/***************************************/
/*            Mouse Move               */
/***************************************/
void GeoOGLWidget :: mouseMoveEvent(QMouseEvent* ev)
{
    if(!geoLayer)
        return;
    mouseCurrPos = ev->pos();

    // 鼠标是否已经移到视图区域外
    //	if (!this->geometry().contains(ev->pos()))
    //		return;
    // 是否已经加载地图

    makeCurrent();
    if (geoLayer->isEdited) {
        // 是否达到框选的最低限度
        if ((mouseCurrPos - beginPt).manhattanLength() < 6)
            return;
        isRectSelecting = true;
        update();
    }
    else{
        // 窗口宽高比
        aspectRatio = float(this->width()) / float(this->height());

        // 摄像机偏移值
        float xOffset = 2 * aspectRatio * tanFovDiv2 * (mouseCurrPos.x() - endPt.x()) * camPos.z / this->width();
        float yOffset = tanFovDiv2 * (endPt.y() - mouseCurrPos.y()) * camPos.z * 2 / this->height();

        // view： camera position and direction
        camPos -= glm::vec3(xOffset, yOffset, 0);
        glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);

        glm::mat4 mvp = proj * view * model;
        setMVP(mvp);
        ;
        update();


    }
}

void GeoOGLWidget :: mouseReleaseEvent(QMouseEvent* ev)
{
    if (!geoLayer)
        return;
    isRectSelecting = false;
    if(geoLayer->isEdited)
    {

        endPt = ev->pos();
        std::vector<GeoObject*> objs ;
        if ((endPt - beginPt).manhattanLength() < 6)
        {
            GeoObject* obj = nullptr;
            GeoPoint geoPt = xy2screen(endPt.x(), endPt.y());
            geoLayer->queryFeatures(geoPt.x, geoPt.y, obj);

            if (obj) {
                //     qDebug()<<obj->getId()  <<" " <<obj->getType();
                update();
            }
            if (obj != nullptr)
            {
                objs.push_back(obj);
                // emit OpenIndexResult(geoLayer,objs);
                emit reserveSelectFeature(objs);
                emit OpenIndexResult(geoLayer,objs);
            }
        }
        else {


            GeoPoint geoPt1 = xy2screen(beginPt.x(),beginPt.y());
            GeoPoint geoPt2 = xy2screen(endPt.x(), endPt.y());
            geoLayer->queryFeatures(GeoExtent(geoPt1.x,geoPt2.x,geoPt2.y,geoPt1.y),objs);
            emit reserveSelectFeature(objs);
            emit OpenIndexResult(geoLayer,objs);

            size_t size = objs.size();
            for(unsigned i = 0; i < size; i++)
            {
                if(objs[i])
                {
                    qDebug()<<objs[i]->getId()  <<" " <<objs[i]->getType();
                    update();
                }
            }
        }
        update();
    }

    QOpenGLWidget::mouseReleaseEvent(ev);
}

void GeoOGLWidget :: wheelEvent(QWheelEvent* ev)
{
    GeoPoint geoXY = xy2screen(ev->x(), ev->y());

    //	if (!this->geometry().contains(ev->pos()))
    //		return;
    if (!geoLayer)
        return;

    makeCurrent();

    float xOffset = (geoXY.x - camPos.x) * kDeltaZ / camPos.z;
    float yOffset = (geoXY.y - camPos.y) * kDeltaZ / camPos.z;

    if (ev->delta() > 0) {
        if (camPos.z - 2 > zNear) {
            camPos.z -= kDeltaZ;
            camPos.x += xOffset;
            camPos.y -= yOffset;
        }
        else
            return;
    }
    else {
        if (camPos.z + 2 < zFar) {
            camPos.z += kDeltaZ;
            camPos.x -= xOffset;
            camPos.y += yOffset;
        }
        else
            return;
    }

    view = glm::lookAt(camPos, camPos + camFront, camUp);
    glm::mat4 mvp = proj * view * model;
    setMVP(mvp);

    update();
}
void GeoOGLWidget :: sendDataToGPU()
{
    if (!geoLayer)
        return;

    makeCurrent();

    // 是否已经发送给GPU
    //……
    clock_t start = clock();
    layerVAO = new LayerVAO(0);
    layerVBO = new LayerVBO(0);
    layerIBO = new LayerIBO(0);

    int count = geoLayer->geoObjects.size();
    for(int i = 0; i < count; i++)
    {

        const auto& feature = geoLayer->geoObjects[i];

        unsigned int nFID = feature->getId();


        float r = rand()%10/(float)(10);
        float g = rand()%10/(float)(10);
        float b = rand()%10/(float)(10);

        FeatureColor* featureColor = new FeatureColor(nFID);
        featureColor->r = r;
        featureColor->b = b;
        featureColor->g = g;
        featuresColor.push_back(featureColor);

        VertexArray* vao = new VertexArray();

        VertexBuffer* vbo = nullptr;

        FeatureVAO* featureVAO = new FeatureVAO(nFID, vao);
        FeatureVBO* featureVBO = new FeatureVBO(nFID);
        FeatureIBO* featureIBO = new FeatureIBO(nFID);

        layerVAO->addFeatureVAO(featureVAO);
        layerVBO->addFeatureVBO(featureVBO);
        layerIBO->addFeatureIBO(featureIBO);

        const char* geomType = feature->getType();
        if(strcmp(geomType,"POINT") == 0)
        {
            GeoPoint* point = (GeoPoint* )feature;
            // VBO
            float vertices[5] = { point->x, point->y, r, g, b };
            vbo = new VertexBuffer(vertices, 5 * sizeof(float));
            featureVBO->setVBO(vbo);
            // IBO
            unsigned int indices = 0;
            IndexBuffer* ibo = new IndexBuffer(&indices, 1, GL_POINTS);
            featureIBO->addIBO(ibo);

        }
        else if(strcmp(geomType,"POLYGON") == 0)
        {
            GeoPolygon* geoPolygon = (GeoPolygon* )feature;
            size_t polygonPointsCount = geoPolygon->getNumPoints();
            //int interiorRingsCount = geoPolygon->getInteriorRingsCount();

            using Point = std::array<double, 2>;
            std::vector<std::vector<Point>> polygon;

            float* vertices = new float[polygonPointsCount * 5 * sizeof(float)];

            // exteriorRing
            int exteriorRingSize = geoPolygon->polylines.size();
            int index = 0;
            for(int j = 0; j < exteriorRingSize; j++)
            {
                GeoPolyline* geoExteriorPolyline = geoPolygon->polylines[j];
                int exteriorRingPointsCount = geoExteriorPolyline->points.size();
                std::vector<Point> exteriorRing;
                exteriorRing.reserve(exteriorRingPointsCount);
                GeoPoint rawPoint;
                for (int i = 0; i < exteriorRingPointsCount; ++i)
                {
                    rawPoint = geoExteriorPolyline->points[i];
                    exteriorRing.push_back({ rawPoint.x, rawPoint.y });
                    vertices[index] = rawPoint.x;
                    vertices[index + 1] = rawPoint.y;
                    vertices[index + 2] = r;
                    vertices[index + 3] = g;
                    vertices[index + 4] = b;
                    index += 5;
                }
                polygon.emplace_back(exteriorRing);
            }
            // 内环
            /*
            for (int j = 0; j < interiorRingsCount; ++j) {
                const auto& geoInteriorRing = geoPolygon->getInteriorRing(j);
                int interiorRingPointsCount = geoInteriorRing->getNumPoints();
                std::vector<Point> interiorRing;
                interiorRing.reserve(interiorRingPointsCount);
                for (int k = 0; k < interiorRingPointsCount; ++k) {
                    geoInteriorRing->getRawPoint(k, &rawPoint);
                    exteriorRing.push_back({ rawPoint.x, rawPoint.y });
                    vertices[index] = rawPoint.x;
                    vertices[index + 1] = rawPoint.y;
                    vertices[index + 2] = r;
                    vertices[index + 3] = g;
                    vertices[index + 4] = b;
                    index += 5;
                }
                polygon.emplace_back(interiorRing);
            }
*/

            // Triangulation
            std::vector<unsigned int> indices = mapbox::earcut<unsigned int>(polygon);
            std::reverse(indices.begin(), indices.end());

            // VBO
            vbo = new VertexBuffer(vertices, polygonPointsCount * 5 * sizeof(float));
            featureVBO->setVBO(vbo);

            // IBO
            IndexBuffer* ibo = new IndexBuffer(&indices[0], indices.size(), GL_TRIANGLES);
            featureIBO->addIBO(ibo);
            delete[] vertices;
        }
        else if(strcmp(geomType,"POLYLINE") == 0)
        {
            GeoPolyline* lineString = (GeoPolyline* )feature;
            int pointsCount = lineString->points.size();
            float* vertices = new float[pointsCount * 5];
            for (int j = 0; j < pointsCount; ++j)
            {
                vertices[j * 5] = lineString->points[j].x;
                vertices[j * 5 + 1] = lineString->points[j].y;
                vertices[j * 5 + 2] = r;
                vertices[j * 5 + 3] = g;
                vertices[j * 5 + 4] = b;
            }

            vbo = new VertexBuffer(vertices, 5 * sizeof(float) * pointsCount);
            featureVBO->setVBO(vbo);

            unsigned int* indices = newContinuousNumber(0, pointsCount);
            IndexBuffer* ibo = new IndexBuffer(indices, pointsCount, GL_LINE_STRIP);
            featureIBO->addIBO(ibo);
            delete[] vertices;
            delete[] indices;
        }
        else if(strcmp(geomType,"MULTIPOINT") == 0) return;
        else if(strcmp(geomType,"MULTIPOLYGON") == 0)
        {
            GeoMultiPolygon* multiPolygon = (GeoMultiPolygon *)feature;
            int pointsCount = multiPolygon->getNumPoints();
            int sizeOffset = 0;
            int countOffset = 0;
            //     int subDataSize = 0;

            int polygonCount = multiPolygon->polygons.size();

            // VBO，这里只是分配空间，没有发送数据
            float* vertices = new float[pointsCount * 5 * sizeof(float)];
            vbo = new VertexBuffer(vertices, pointsCount * 5 * sizeof(float));

            // 构造VAO VBO IBO
            for (int i = 0; i < polygonCount; ++i)
            {
                GeoPolygon* geoPolygon = multiPolygon->polygons[i];
                size_t polygonPointsCount = geoPolygon->getNumPoints();
                //    int interiorRingsCount = geoPolygon->polylines.size();

                using Point = std::array<double, 2>;
                std::vector<std::vector<Point>> polygon;

                //    float* vertices = new float[polygonPointsCount * 5 * sizeof(float)];

                // 外环
                int exteriorRingSize = geoPolygon->polylines.size();
                for(int j = 0; j < exteriorRingSize; j++)
                {
                    GeoPolyline* geoExteriorPolyline = geoPolygon->polylines[j];
                    int exteriorRingPointsCount = geoExteriorPolyline->points.size();
                    std::vector<Point> exteriorRing;
                    exteriorRing.reserve(exteriorRingPointsCount);
                    GeoPoint rawPoint;
                    int index = 0;
                    for (int i = 0; i < exteriorRingPointsCount; ++i)
                    {
                        rawPoint = geoExteriorPolyline->points[i];
                        exteriorRing.push_back({ rawPoint.x, rawPoint.y });
                        vertices[index] = rawPoint.x;
                        vertices[index + 1] = rawPoint.y;
                        vertices[index + 2] = r;
                        vertices[index + 3] = g;
                        vertices[index + 4] = b;
                        index += 5;
                    }
                    polygon.emplace_back(exteriorRing);
                }

                // 三角剖分
                std::vector<unsigned int> indices = mapbox::earcut<unsigned int>(polygon);
                //					std::reverse(indices.begin(), indices.end());

                if (countOffset > 0)
                    std::for_each(indices.begin(), indices.end(), [&countOffset](auto& value) {value += countOffset; });

                vbo->addSubData(vertices, sizeOffset, polygonPointsCount * 5 * sizeof(float));

                featureVBO->setVBO(vbo);
                IndexBuffer* ibo = new IndexBuffer(&indices[0], indices.size(), GL_TRIANGLES);
                featureIBO->addIBO(ibo);
                sizeOffset += polygonPointsCount * 5 * sizeof(float);
                countOffset += polygonPointsCount;

            }
            delete[] vertices;
        }
        else if(strcmp(geomType,"MULTILINESTRING") == 0)
        {
            GeoMultiLineString* multiLineString = (GeoMultiLineString *)feature;
            size_t pointsCount = multiLineString->getNumPoints();
            float* vertices = new float[pointsCount * 5];
            size_t numOfline = multiLineString->polylines.size();
            int index = 0;
            for(unsigned int j = 0; j < numOfline; j++)
            {
                GeoPolyline* geoPolyline = multiLineString->polylines[j];
                size_t ptSize = geoPolyline->points.size();
                for (size_t k = 0; k < ptSize; k++)
                {
                    vertices[index] = geoPolyline->points[k].x;
                    vertices[index + 1] = geoPolyline->points[k].y;
                    vertices[index + 2] = r;
                    vertices[index + 3] = g;
                    vertices[index + 4] = b;
                    index += 5;
                }
            }
            vbo = new VertexBuffer(vertices, 5 * sizeof(float) * pointsCount);
            featureVBO->setVBO(vbo);
            unsigned int* indices = newContinuousNumber(0, pointsCount);
            IndexBuffer* ibo = new IndexBuffer(indices, pointsCount, GL_LINE_STRIP);
            featureIBO->addIBO(ibo);
            delete[] vertices;
            delete[] indices;
        }
        // 告诉OpenGL数据是如何布局的
        VertexBufferLayout layout;
        layout.Push<float>(2);	// x, y
        layout.Push<float>(3);	// r, g, b
        vao->addBuffer(*vbo, layout);

    }
    clock_t end = clock();
    qDebug()<<" Time of sending data to GPU:" << (end - start) / double(CLK_TCK) << "s" <<endl;
    //initialize mvp
    makeCurrent();

    /* MVP: model、view、project */
    // project
    zNear = 0.1f;
    zFar = 100.0f;
    fov = 45.0f;		// 视角，角度制
    tanFovDiv2 = tan(glm::radians(fov / 2.0f));
    aspectRatio = float(this->width()) / float(this->height());
    proj = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);

    // view： camera position and direction
    float layerBoundaryRatio = geoLayer->getBoundaryWidth()/geoLayer->getBoundaryHeight();
    float camPosZ = 0.0f;
    if (layerBoundaryRatio < this->aspectRatio)
        camPosZ = geoLayer->getBoundaryHeight() / (2 * tanFovDiv2);
    else
        camPosZ = geoLayer->getBoundaryWidth()  / (2 * this->aspectRatio * tanFovDiv2);
    camPos = glm::vec3(geoLayer->getCenterX(), geoLayer->getCenterY(), camPosZ);
    camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    camUp = glm::vec3(0.0f, 1.0f,  0.0f);
    view = glm::lookAt(camPos, camPos + camFront, camUp);

    // model
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    // MVP
    glm::mat4 mvp = proj * view * model;
    setMVP(mvp);

    update();
    // 重置摄像机位置
    //   resetCameraStatusSlot();
    geoLayer->isSend = true;
}

void GeoOGLWidget::SlotUpdateRGB()
{
    makeCurrent();
    size_t count = geoLayer->geoObjects.size();
    for(unsigned int i = 0; i < count; i++)
    {
        //  unsigned int renderId = layerVBO->fVBOs[i]->VBO->getRendererID();
        const auto& vbo = layerVBO->getFeatureVbo(i)->VBO;
        vbo->Bind();
        float* newVbo =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
        const auto& feature = geoLayer->geoObjects[i];
        FeatureColor* featureColor = featuresColor[i];
        if(strcmp(feature->getType(),"POLYGON") == 0)
        {
            GeoPolygon* polygon = (GeoPolygon *)feature;
            featureColor->r = float(polygon->fillColor.redF());
            featureColor->g = float(polygon->fillColor.greenF());
            featureColor->b = float(polygon->fillColor.blueF());
            int size = vbo->getVerticeCount();
            for(int j = 0; j < size; j += 5)
            {
                newVbo[j+2] = float(polygon->fillColor.redF());
                newVbo[j+3] = float(polygon->fillColor.greenF());
                newVbo[j+4] = float(polygon->fillColor.blueF());

            }
        }
        if (strcmp(feature->getType(),"MULTIPOLYGON") == 0)
        {
            GeoMultiPolygon* multiPolygon = (GeoMultiPolygon *)feature;
            featureColor->r = float(multiPolygon->fillColor.redF());
            featureColor->g = float(multiPolygon->fillColor.greenF());
            featureColor->b = float(multiPolygon->fillColor.blueF());
            int size = vbo->getVerticeCount();
            for(int j = 0; j < size; j+=5)
            {
                newVbo[j+2] = float(multiPolygon->fillColor.redF());
                newVbo[j+3] = float(multiPolygon->fillColor.greenF());
                newVbo[j+4] = float(multiPolygon->fillColor.blueF());
            }
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);

    }
    update();
}

void GeoOGLWidget :: drawRectNoFill(const QPoint& startPoint, const QPoint& endPoint, float r /*= 0.0f*/, float g /*= 0.0f*/, float b /*= 0.0f*/, int lineWidth /*= 1*/)
{
    GeoRawPoint leftTop = screen2stdxy(startPoint.x(), startPoint.y());
    GeoRawPoint rightBottom = screen2stdxy(endPoint.x(), endPoint.y());
    glBegin(GL_LINE_STRIP);
    glColor3f(r, g, b);
    glLineWidth(lineWidth);
    glVertex2d(leftTop.x, leftTop.y);
    glVertex2d(leftTop.x, rightBottom.y);
    glVertex2d(rightBottom.x, rightBottom.y);
    glVertex2d(rightBottom.x, leftTop.y);
    glVertex2d(leftTop.x, leftTop.y);
    glEnd();
}

// 绘制填充矩形
void GeoOGLWidget::  drawRectFillColor(const QPoint& startPoint, const QPoint& endPoint, float r, float g, float b, float a)
{
    GeoRawPoint leftTop = screen2stdxy(startPoint.x(), startPoint.y());
    GeoRawPoint rightBottom = screen2stdxy(endPoint.x(), endPoint.y());

    // 如果设置了透明度，就启用混合
    bool usingBlend = fabs(a - 1.0f) < 0.001f ? false : true;
    if (usingBlend) {
        glEnable(GL_BLEND);	// 启用混合
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// 混合函数
    }

    glColor4f(r, g, b, a);
    glRectd(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y);	// 绘制矩形内部

    if (usingBlend) {
        glDisable(GL_BLEND);// 关闭混合
    }
}

void GeoOGLWidget :: slotReserveRGB(std::vector<GeoObject*> objs)
{
    slotRecoverColor();
    int count = objs.size();
    for(int i = 0; i < count; i++)
    {
        unsigned int renderId = objs[i]->getId();
        updateFeatureColor(renderId);
    }
    update();
}

void GeoOGLWidget :: updateFeatureColor(int FID)
{
    FeatureColor* featureColor = this->getFeatureColorByFID(FID);
    const auto& vbo = layerVBO->getFeatureVbo(FID)->VBO;
    vbo->Bind();
    float* newVbo =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
    int size = vbo->getVerticeCount();
    for (int j = 0; j < size; j += 5)
    {
        newVbo[j+2] = 1- newVbo[j+2];
        newVbo[j+3] = 1- newVbo[j+3];
        newVbo[j+4] = 1- newVbo[j+4];
    }

    featureColor->isChanged = true;
    //   qDebug()<<featureColor->FID<<endl;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    update();
}

void GeoOGLWidget :: onSelectFeature(int nLID,int nFID)
{
    FeatureColor* featureColor = this->getFeatureColorByFID(nFID);
    const auto& vbo = layerVBO->getFeatureVbo(nFID)->VBO;
    vbo->Bind();
    float* newVbo =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
    int size = vbo->getVerticeCount();
    for (int j = 0; j < size; j += 5)
    {
        newVbo[j+2] = float(175/255);
        newVbo[j+3] = float(32/255);
        newVbo[j+4] = float(49/255);
    }
    featureColor->isChanged = true;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    update();
}

void GeoOGLWidget :: slotRecoverColor()
{
    makeCurrent();
    size_t featureCount = geoLayer->geoObjects.size();
    for (unsigned int nFlags = 0; nFlags < featureCount; nFlags++)
    {
        FeatureColor* fColor = featuresColor[nFlags];
        if (fColor->isChanged)
        {
            const auto& vbo = layerVBO->getFeatureVbo(nFlags)->VBO;
            vbo->Bind();
            float* newVbo =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
            int size = vbo->getVerticeCount();
            for(int j = 0; j < size; j += 5)
            {
                newVbo[j+2] = fColor->r;
                newVbo[j+3] = fColor->g;
                newVbo[j+4] = fColor->b;
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
            fColor->isChanged = false;
        }
    }
    update();
}

void GeoOGLWidget :: onSelectFeature(int nLID, std::vector<int>& nFIDS)
{
    slotRecoverColor();
    for (auto nFID : nFIDS)
    {
        this->onSelectFeature(nLID,nFID);
    }
}

void GeoOGLWidget :: slotDeleteLayer()
{
    if (!this->geoLayer)
        return;
    else
        if(layerVAO||layerVBO||layerIBO)

        {
            delete layerVBO;
            delete layerVAO;
            delete layerIBO;
            layerVAO = nullptr;
            layerVBO = nullptr;
            layerIBO = nullptr;
            geoLayer = nullptr;

    //        delete this->geoLayer;
            update();
        }
}

unsigned int* GeoOGLWidget::newContinuousNumber(unsigned int start, unsigned int count)
{
    if (count < 1)
            return nullptr;
        else {
            unsigned int* ret = new unsigned int[count];
            for (unsigned int i = 0; i < count; ++i) {
                ret[i] = start + i;
            }
            return ret;
        }
}
