#include "globalsearchresult.h"
#include <QStringList>
#include <QHeaderView>

GlobalSearchResult::GlobalSearchResult(GeoMap* mapIn, QWidget *parent)
    : QDialog(parent), map(mapIn)
{
    this->setWindowTitle(tr("Search Result"));
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setStyleSheet("background-color:white;");
}

GlobalSearchResult::~GlobalSearchResult()
{
}

void GlobalSearchResult::clear()
{
    results.clear();

    if (scrollAreaLayout)
        clearLayout(scrollAreaLayout);
}

void GlobalSearchResult::clearLayout(QLayout* layout)
{
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->setParent(nullptr);
        }
        // 递归删除子布局
        else if (child->layout()) {
            clearLayout(child->layout());
        }
        delete child;
    }
}

void GlobalSearchResult::setupLayout()
{
    if (!scrollArea) {
        scrollArea = new QScrollArea(this);
        mainLayout = new QVBoxLayout(this);
        mainLayout->setMargin(0);
        mainLayout->addWidget(scrollArea);
        scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        scrollArea->setWidgetResizable(true);
        scrollAreaCenterWidget = new QWidget(scrollArea);
        scrollAreaLayout = new QVBoxLayout(this);
        scrollAreaCenterWidget->setLayout(scrollAreaLayout);
        scrollArea->setWidget(scrollAreaCenterWidget);
    }

    // 在多少个图层搜索到，就显示多少个QTableWidget
    // 放在一个QScrollArea中
    size_t resultsListCount = results.size();
    for (size_t i = 0; i < resultsListCount; ++i) {
        const SearchResult& result = results[i];
        int nLID = result.LID;
        int featuresCount = result.features.size();
        GeoLayer* layer = map->geolayers[nLID];
        int fieldsCount = layer->getNumFields();

        // 创建QTableWidget
        QTableWidget* resultList = new QTableWidget(this);
        resultList->setUserData(Qt::UserRole, (new CustomData(nLID)));
        resultList->setAlternatingRowColors(true);
        resultList->setColumnCount(fieldsCount + 1);
        resultList->setRowCount(featuresCount);
        resultList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        resultList->setSelectionMode(QAbstractItemView::SingleSelection);
        resultList->setSelectionBehavior(QAbstractItemView::SelectRows);
        resultList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        connect(resultList, &QTableWidget::itemDoubleClicked,
                this, &GlobalSearchResult::onDoubleClicked);
        scrollAreaLayout->addWidget(resultList);

        // 设置表头
        QStringList header;
        header << "FID";
        for (int i = 0; i < fieldsCount; ++i) {
            header << layer->getFieldDefn(i)->getName();
        }
        resultList->setHorizontalHeaderLabels(header);

        // 填充数据
        for (int iFeature = 0; iFeature < featuresCount; ++iFeature) {
            GeoObject* feature = result.features[iFeature];

            // 第一列（FID）
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignRight);
            item->setText(QString::number(feature->getId()));
            resultList->setItem(iFeature, 0, item);

            // 遍历字段的每一列
            for (int iCol = 0; iCol < fieldsCount; ++iCol) {
                QTableWidgetItem* item = new QTableWidgetItem();
                GeoFieldDefn* fieldDefn = layer->getFieldDefn(iCol);

                switch (fieldDefn->getType()) {
                default:
                    break;
                case kFieldInt: {
                    item->setTextAlignment(Qt::AlignRight);
                    int value;
                    feature->getField(iCol, &value);
                    item->setText(QString::number(value));
                    break;
                }
                case kFieldDouble: {
                    item->setTextAlignment(Qt::AlignRight);
                    double value;
                    feature->getField(iCol, &value);
                    item->setText(QString::number(value));
                    break;
                }
                case kFieldText: {
                    item->setTextAlignment(Qt::AlignLeft);
                    QString value;
                    feature->getField(iCol, &value);
                    item->setText(value);
                    break;
                }
                } // end switch

                resultList->setItem(iFeature, iCol + 1, item);
            } // end for iCol
        } // end for iFeature
    } // end for iResults
}

void GlobalSearchResult::closeEvent(QCloseEvent* event)
{
    emit unSave();
    emit closed();
}

// 双击数据区一行
void GlobalSearchResult::onDoubleClicked(QTableWidgetItem* item)
{
    CustomData* pLID = (CustomData*)(item->tableWidget()->userData(Qt::UserRole));
    int nLID = pLID->LID;
    int nFID = item->tableWidget()->item(item->row(), 0)->text().toInt();

    emit featureSelected(nLID, nFID);
}
void GlobalSearchResult::slotOpenIndexResult(GeoLayer* layer,std::vector<GeoObject*>objects)
{
    if (!layer)
        return;
    addSearchResult(layer->getId(), objects);
  //  QPoint pos = this->mapToGlobal(this->pos());
    this->setGeometry(200,200, 800, 300);
    this->setupLayout();
    this->show();
 //   objects.clear();
}
