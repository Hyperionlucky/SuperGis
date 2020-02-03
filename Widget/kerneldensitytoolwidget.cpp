#include "kerneldensitytoolwidget.h"

KernelDensityToolWidget::KernelDensityToolWidget(GeoLayer *layerIn,QWidget* parent)
    :QOpenGLWidget (parent),layer(layerIn)
{

}

KernelDensityToolWidget :: ~KernelDensityToolWidget()
{

}

void KernelDensityToolWidget :: initializeGL(){
    glClearColor(0.16f,0.16f,0.16f,0.0f);
}

void KernelDensityToolWidget :: resizeGL(int width,int height){
    glViewport(0,0,width,height);
}

void KernelDensityToolWidget :: paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(offsetX,offsetY,0);
    glScalef(scale,scale,0);

    //task 3
    if(this->layer!=nullptr)
    {
        if(this->layer->kdeCalculated==0)
        {
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            for(int i=0;i<layer->kdePts.size();i++)
            {
                 glColor3d(0,0,0.8);
                 glVertex2f(xy2screen(layer->kdePts[i]).x,xy2screen(layer->kdePts[i]).y);
             }
             glEnd();
        }
        if(this->layer->kdeCalculated==1)
        {
              for(int i=0;i<1000;i++)
              {
                  for(int j=0;j<1000;j++)
                  {
                      float a=(this->layer->loc[i][j]-this->layer->minLoc)/(this->layer->maxLoc-this->layer->minLoc);
                      glColor3f(0.16f,0.16f,0.16f+a);
                      double x,y;
                      glBegin(GL_POLYGON);
                      x = -1.0 + 0.002*i;
                      y = -1.0 + 0.002*j;
                      glVertex2f(x, y);
                      glVertex2f(x + 0.002, y);
                      glVertex2f(x + 0.002, y+0.002);
                      glVertex2f(x, y + 0.002);
                      glEnd();
                  }
              }
        }
    }
    glFlush();
}

GeoPoint KernelDensityToolWidget::xy2screen(GeoPoint pt)
{
    // transfer point to screen coordinate
    float width = this->layer->getBoundaryWidth();
    float height = this->layer->getBoundaryHeight();
    GeoPoint screen;
    screen.x = (pt.x-this->layer->left)/width*2-1;
    screen.y = (pt.y-this->layer->bottom)/height*2-1;
    return screen;
}

void KernelDensityToolWidget::mousePressEvent(QMouseEvent *event){
    if (event->buttons() == Qt::LeftButton)
    {
        // 切换光标样式
        setCursor(Qt::OpenHandCursor);
        mouseX = event->x();
        mouseY = event->y();
    }
    if (event->buttons() == Qt::RightButton)
    {
        offsetX = 0;
        offsetY = 0;
        update();
    }
}

void KernelDensityToolWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        // 计算图像偏移量
        newOffsetX = (event->x() - mouseX) / this->width();
        newOffsetY = (mouseY - event->y()) / this->height();
        offsetX += newOffsetX;
        offsetY += newOffsetY;
        update();
    }
}

void KernelDensityToolWidget::mouseReleaseEvent(QMouseEvent *event)
{
    unsetCursor();
    offsetX += newOffsetX;
    offsetY += newOffsetY;
    newOffsetX = 0;
    newOffsetY = 0;
}

void KernelDensityToolWidget::wheelEvent(QWheelEvent *event)
{
    scale -= 0.01 * event->delta();
    // 限制缩小倍数
    if (scale < 0.9f)
    {
        scale = 0.9f;
    }
    update();
}

