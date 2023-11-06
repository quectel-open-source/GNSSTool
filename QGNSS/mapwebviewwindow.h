#ifndef MAPWEBVIEWWINDOW_H
#define MAPWEBVIEWWINDOW_H

#include <QWidget>
#include "QWebEngineView"

#define OSM_URL "file:///./OnlineMap/OSM.html"
#define userKeyHTML_URL "file:///./OnlineMap/user.html"
#define userKeyHTML "./OnlineMap/user.html"
#define TianDiMapTemplate "./OnlineMap/TianDiMap.html"

namespace Ui {
class MapWebViewWindow;
}


class MapWebViewWindow : public QWidget
{
    Q_OBJECT

public:
    enum MapType{
        AutoSelect = 0,
        Baidu,
        Google,
        OSM,
        TianDiMap,
    };
    Q_ENUM(MapType)

    explicit MapWebViewWindow(QWidget *parent = nullptr);
    ~MapWebViewWindow();

    void switchOnlineMap(MapType mapType = AutoSelect);
    void addMark(const QString &cmd);
    void clearOverlays();
public slots:
    void applySettings();

private:
    Ui::MapWebViewWindow *ui;
    QWebEngineView  *map_webView;
    MapType mapType;

};

#endif // MAPWEBVIEWWINDOW_H
