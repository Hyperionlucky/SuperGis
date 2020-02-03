#include "Dialog/kernelcaculatedialog.h"

KernelCaculateDialog::KernelCaculateDialog(GeoLayer* layerIn, QWidget *parent)
    : QDialog(parent), layer(layerIn)
{
    // 最大化和最小化按钮
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag |= Qt::WindowMinimizeButtonHint;
    windowFlag |= Qt::WindowMaximizeButtonHint;
    windowFlag |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(windowFlag);
    this->setWindowTitle(tr("Attribute Table"));
    this->setAttribute(Qt::WA_DeleteOnClose, true);		// 关闭即销毁

    setupLayout();
}

KernelCaculateDialog::~KernelCaculateDialog()
{

}

void KernelCaculateDialog::setupLayout()
{
    layer->calculateKde();
    kdeWidget = new KernelDensityToolWidget(layer,this);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(5);
    mainLayout->addWidget(kdeWidget);
}
