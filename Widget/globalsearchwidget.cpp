#include "globalsearchwidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QStringListModel>

GlobalSearchWidget::GlobalSearchWidget(GeoMap* mapIn, QWidget *parent)
    : map(mapIn), QLineEdit(parent)
{
    setupLayout();

    this->setFont(QFont("Microsoft YaHei", 10, QFont::Normal));
    isFileOpen = true;
    connect(this, &QLineEdit::textChanged, this,
            &GlobalSearchWidget::onTextChanged);
}
GlobalSearchWidget::~GlobalSearchWidget()
{
}

void GlobalSearchWidget::setupLayout()
{
    stringList <<"open shape File"<<"open GeoJSON"<<"Link to Postgis";
    completer = new QCompleter(stringList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    this->setCompleter(completer);

    // 按钮 清除文字
    btnClearText = new QPushButton(this);
    btnClearText->setFixedSize(15, 15);
    btnClearText->setStyleSheet("border:0px");
    btnClearText->setFocusPolicy(Qt::NoFocus);
    btnClearText->setCursor(QCursor(Qt::ArrowCursor));
    btnClearText->setStyleSheet("QPushButton{border-image: url(Icons/clear-text.png);}"
                                "QPushButton:hover{border-image: url(Icons/clear-text-hover.png);}"
                                );
    btnClearText->hide();
    connect(btnClearText, &QPushButton::clicked,
            this, [this](void) { this->clear(); });

    // 按钮 搜索
    btnSearch = new QPushButton(this);
    btnSearch->setFixedSize(18, 18);
    btnSearch->setStyleSheet("border:0px");
    btnSearch->setFocusPolicy(Qt::NoFocus);
    btnSearch->setCursor(QCursor(Qt::PointingHandCursor));
    btnSearch->hide();
    btnSearch->setStyleSheet("QPushButton{border-image: url(Icons/search.png);}"
                             "QPushButton:hover{border-image: url(Icons/search-hover.png);}"
                             "QPushButton:pressed{border-image: url(Icons/search-press.png);}"
                             );
    connect(this, &QLineEdit::returnPressed, this, &GlobalSearchWidget::onSearch);
    connect(btnSearch, &QPushButton::clicked, this, &GlobalSearchWidget::onSearch);

    QMargins textMargins = this->textMargins();
    this->setTextMargins(textMargins.left(), textMargins.top(),
                         btnSearch->width() + btnClearText->width(), textMargins.bottom());

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addStretch();
    layout->addWidget(btnClearText);
    layout->addWidget(btnSearch);
    layout->setSpacing(0);
    layout->setContentsMargins(1, 1, 1, 1);
    this->setLayout(layout);
    //this->resize(151,31);
    //  this->rect();
    // 没有焦点时透明显示
    this->setStyleSheet(
                "background-color: rgba(255, 255, 255, 0%);"
                "border:1px solid rgb(200, 200, 200);"
                );
}


void GlobalSearchWidget::onClearText()
{
    this->clear();
}

void GlobalSearchWidget::onSearch()
{
    QString input = this->text();
    if (input.isEmpty())
        return;
    if(!isFileOpen)
    {
        GeoLayer* layer = nullptr;

        // 剔除【左右中括号之间的内容（所属的图层名）】
        int leftBracket = input.indexOf("[");
        int rightBracket = input.indexOf("]");
        if (leftBracket != -1 && rightBracket != -1) {
            // 中括号中至少一个字符
            if (rightBracket - leftBracket > 1) {
                QString layerName = input.mid(leftBracket + 1, rightBracket - leftBracket - 1);
                qDebug() << layerName;
                if (!layerName.isEmpty()) {
                    //   layer = map->geolayers(layerName);
                }
            }
            input = input.left(leftBracket) + input.right(input.length() - rightBracket - 1);
            input = input.trimmed();
        }

        if (!searchResultDialog) {
            searchResultDialog = new GlobalSearchResult(map, nullptr);
            QPoint pos = this->mapToGlobal(this->pos());
            searchResultDialog->setGeometry(pos.x(), pos.y(), 800, 300);
            connect(searchResultDialog, &GlobalSearchResult::featureSelected,
                    this, &GlobalSearchWidget::onFeatureSelected);
            connect(searchResultDialog, &GlobalSearchResult::closed,
                    this, &GlobalSearchWidget::onSearchResultDialogClose);
            connect(searchResultDialog, &GlobalSearchResult::unSave,this,&GlobalSearchWidget::slotUnsaveColor);
            connect(this,&GlobalSearchWidget::OpenIndex,searchResultDialog,&GlobalSearchResult::slotOpenIndexResult);
        }
        else {
            searchResultDialog->clear();
        }


        // 指定了搜索图层
        if (layer) {
            std::vector<GeoObject*> features;
            if (searchInLayer(input, layer, features)) {
                searchResultDialog->addSearchResult(layer->getId(), features);
            }
        }

        // 未指定搜索图层，在整个地图中搜索
        else {
            int layersCount = map->geolayers.size();
            for (int iLayer = 0; iLayer < layersCount; ++iLayer) {
                layer = map->geolayers[iLayer];
                std::vector<GeoObject*> features;
                if (searchInLayer(input, layer, features)) {
                    searchResultDialog->addSearchResult(layer->getId(), features);
                }
            }
        }

        searchResultDialog->setupLayout();
        searchResultDialog->show();
    }
    else
    {
        if (input.compare(QString::fromLocal8Bit("open shape File")) == 0)
        {
            emit onReadShp();
        }
        if (input.compare(QString::fromLocal8Bit("open GeoJSON")) == 0)
        {
            emit onReadGeoJSON();
        }
        if (input.compare(QString::fromLocal8Bit("Link to Postgis")) == 0)
        {
            emit onLinkToPostGis();
        }

    }
}


void GlobalSearchWidget::onTextChanged()
{
    //  qDebug()<<"Changed";
    if (this->text().indexOf("icrystal") != -1) {
        //   qDebug()<<"Here";
    }
    else {
        //   qDebug()<<"Not Here";
    }
}

void GlobalSearchWidget::onSearchResultDialogClose()
{
    searchResultDialog = nullptr;
}

void GlobalSearchWidget::slotUpdateInputTextMeaning()
{
    if (isFileOpen)
    {
        isFileOpen = false;
        this->updateCompleterList();
    }
    else
    {
        isFileOpen = true;
        stringList <<"open shape File"<<"open GeoJSON"<<"Link to Postgis";
        QStringListModel* model = (QStringListModel*)(completer->model());
        if (!model)
            return;
        model->setStringList(stringList);
    }
}

// 有焦点时
void GlobalSearchWidget::focusInEvent(QFocusEvent* event)
{
    this->setStyleSheet(
                "background-color: rgba(255, 255, 255, 80%);"
                "border:1px solid rgb(85,170,255);"
                "border-radius:5 px;"
                );
    // 显示按钮
    btnClearText->show();
    btnSearch->show();
    QLineEdit::focusInEvent(event);
}


// 失去焦点时
void GlobalSearchWidget::focusOutEvent(QFocusEvent* event)
{
    // 没有焦点时透明显示
    this->setStyleSheet(
                "background-color: rgba(255, 255, 255, 0%);"
                "border:1px solid rgb(200, 200, 200);"
                );

    // 隐藏按钮
    btnClearText->hide();
    btnSearch->hide();
    QLineEdit::focusOutEvent(event);
}

// 更新 匹配文字
void GlobalSearchWidget::updateCompleterList()
{
    QStringListModel* model = (QStringListModel*)(completer->model());
    if (!model)
        return;

    stringList.clear();
    int layersCount = map->geolayers.size();
    GeoLayer* layer;
    GeoObject* feature;
    QString fieldName;
    for (int iLayer = 0; iLayer < layersCount; ++iLayer) {
        layer = map->geolayers[iLayer];
        int searchFieldIndex = layer->getFieldIndex("name", Qt::CaseInsensitive);
        if (searchFieldIndex == -1) {
            searchFieldIndex = layer->getFieldIndexLike("name", Qt::CaseInsensitive);
            if (searchFieldIndex == -1) {
                continue;
            }
        }
        QString layerName = layer->getLayerName();
        int featuresCount = layer->featureCount;
        for (int iFeature = 0; iFeature < featuresCount; ++iFeature) {
            feature = layer->geoObjects[iFeature];
            QString nameValue;
            feature->getField(searchFieldIndex, &nameValue);
            nameValue = nameValue + " [" + layerName + "]";
            stringList << nameValue;
        }
    }

    model->setStringList(stringList);
}


bool GlobalSearchWidget::searchInLayer(const QString& fieldValue, GeoLayer* layerIn, std::vector<GeoObject*>& featuresOut)
{
    int fieldsCount = layerIn->getNumFields();
    int featuresCount = layerIn->featureCount;

    bool ret = false;
    for (int iFeature = 0; iFeature < featuresCount; ++iFeature) {
        GeoObject* feature = layerIn->geoObjects[iFeature];
        // 遍历字段的每一列
        for (int iField = 0; iField < fieldsCount; ++iField) {
            GeoFieldDefn* fieldDefn = layerIn->getFieldDefn(iField);
            if (fieldDefn->getType() == kFieldText) {
                QString value;
                feature->getField(iField, &value);
                if (value.contains(fieldValue, Qt::CaseInsensitive)) {
                    featuresOut.push_back(feature);
                    ret = true;
                    break;
                }
            }
        } // end for iField
    } // end for iFeature

    return ret;
}

void GlobalSearchWidget :: updateSearchDialog()
{
    if (!searchResultDialog) {
        searchResultDialog = new GlobalSearchResult(map, nullptr);
        QPoint pos = this->mapToGlobal(this->pos());
        searchResultDialog->setGeometry(pos.x(), pos.y(), 800, 300);
        connect(searchResultDialog, &GlobalSearchResult::featureSelected,
                this, &GlobalSearchWidget::onFeatureSelected);
        connect(searchResultDialog, &GlobalSearchResult::closed,
                this, &GlobalSearchWidget::onSearchResultDialogClose);
        connect(searchResultDialog, &GlobalSearchResult::unSave,this,&GlobalSearchWidget::slotUnsaveColor);
        connect(this,&GlobalSearchWidget::OpenIndex,searchResultDialog,&GlobalSearchResult::slotOpenIndexResult);

    }
    else {
        searchResultDialog->clear();
    }
    searchResultDialog->setupLayout();
    searchResultDialog->show();
}
