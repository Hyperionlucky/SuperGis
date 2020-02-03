#ifndef GLOBALSEARCHRESULT_H
#define GLOBALSEARCHRESULT_H

#include <QDialog>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>

#include <map>
#include <vector>

#include "geomap.h"

class GlobalSearchResult : public QDialog
{
    Q_OBJECT

    public:
        GlobalSearchResult(GeoMap* mapIn, QWidget *parent);
        ~GlobalSearchResult();

    signals:
        void closed();
        void unSave();
        void featureSelected(int nLID, int nFID);

    public slots:
        // 双击数据区
        void onDoubleClicked(QTableWidgetItem* item);
        void slotOpenIndexResult(GeoLayer*,std::vector<GeoObject*>geoObjects);

    public:
        void clear();
        void setupLayout();
        void addSearchResult(int nLID, const std::vector<GeoObject*>& features)
            {this->clear();
            results.emplace_back(nLID, features); }

    protected:
        virtual void closeEvent(QCloseEvent* event) override;

    private:
        struct SearchResult {
            SearchResult(int nLID, const std::vector<GeoObject*>& features)
                : LID(nLID), features(features) {}
            int LID;
            std::vector<GeoObject*> features;
        };

        struct CustomData : QObjectUserData {
            CustomData(int nLID) :LID(nLID) {}
            int LID;
        };

    private:
        void clearLayout(QLayout* layout);

        std::vector<SearchResult> results;

        GeoMap* map;

        QVBoxLayout* mainLayout = nullptr;
        QVBoxLayout* scrollAreaLayout = nullptr;
        QScrollArea* scrollArea = nullptr;
        QWidget* scrollAreaCenterWidget = nullptr;
};

#endif // GLOBALSEARCHRESULT_H
