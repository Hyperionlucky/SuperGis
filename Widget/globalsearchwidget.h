#ifndef GLOBALSEARCHWIDGET_H
#define GLOBALSEARCHWIDGET_H

#include <QLineEdit>
#include <QCompleter>
#include <QStringList>
#include <QPushButton>

#include "geolayer.h"
#include "Dialog/globalsearchresult.h"

class GlobalSearchWidget : public QLineEdit
{
    Q_OBJECT

    public:
        GlobalSearchWidget(GeoMap* mapIn, QWidget *parent);
        ~GlobalSearchWidget();

    public:
        void updateCompleterList();

    signals:
        void featureSelected(int nLID, int nFID);
        void onReadShp();
        void onReadGeoJSON();
        void onLinkToPostGis();
        void unSaveColor();
        void OpenIndex(GeoLayer*,std::vector<GeoObject*>);
    public slots:
        void onClearText();
        void onSearch();
        void onTextChanged();
        void onSearchResultDialogClose();
        void onFeatureSelected(int nLID, int nFID)
            { emit featureSelected(nLID, nFID); }
        void slotUpdateInputTextMeaning();
        void slotUnsaveColor()
        {
            emit unSaveColor();
        }
        void slotOpenIndex(GeoLayer* geolayer,std::vector<GeoObject*>objs)
        {
            updateSearchDialog();
            emit OpenIndex(geolayer,objs);
        }
        /* Override Event */
    protected:
        virtual void focusInEvent(QFocusEvent* event) override;
        virtual void focusOutEvent(QFocusEvent* event) override;

    private:
        void setupLayout();
        bool searchInLayer(const QString& value, GeoLayer* layerIn, std::vector<GeoObject*>& featuresOut);
        void updateSearchDialog();
        GeoMap* map;

        QStringList stringList;
        QCompleter* completer;
        QPushButton* btnSearch;
        QPushButton* btnClearText;

        GlobalSearchResult* searchResultDialog = nullptr;
        bool isFileOpen;
};

#endif // GLOBALSEARCHWIDGET_H
