#ifndef KERNELCACULATEDIALOG_H
#define KERNELCACULATEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include "geolayer.h"
#include "Widget/kerneldensitytoolwidget.h"

class KernelCaculateDialog : public QDialog
{
    Q_OBJECT
public:
    KernelCaculateDialog(GeoLayer* layer,QWidget* parent);
    ~KernelCaculateDialog();
    GeoLayer* layer ;

private:
    void setupLayout();

    QVBoxLayout* mainLayout = nullptr;
    KernelDensityToolWidget* kdeWidget = nullptr;

};

#endif // KERNELCACULATEDIALOG_H
