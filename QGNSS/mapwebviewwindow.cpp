#include "mapwebviewwindow.h"
#include "ui_mapwebviewwindow.h"
#include "common.h"

#include <QSettings>
#include <QIcon>
#include <QFile>

MapWebViewWindow::MapWebViewWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapWebViewWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Online Map");
    setWindowIcon(QIcon(":/images/onLineMap.png"));
    map_webView = new QWebEngineView();
    ui->gridLayout->addWidget(map_webView);
    switchOnlineMap();
}

MapWebViewWindow::~MapWebViewWindow()
{
    delete ui;
    delete map_webView;
}

void MapWebViewWindow::switchOnlineMap(MapWebViewWindow::MapType mapType)
{
    switch (mapType) {
    case TianDiMap:
    {
        QFile HTMLtemplate(TianDiMapTemplate);
        HTMLtemplate.open(QIODevice::ReadOnly);
        QString map = HTMLtemplate.readAll();
        HTMLtemplate.close();
        QSettings settings(SETTINGS_FILE_NAME, QSettings::IniFormat);
        QString mapKey = settings.value(TianDiMapKey,QVariant("26293a33a1dc6f87720e5a47b918f300")).toString();
        map = QString(map).arg(mapKey);
        QFile userHTML(userKeyHTML);
        userHTML.open(QIODevice::WriteOnly);
        userHTML.write(map.toLatin1());
        userHTML.close();
        map_webView->load(QUrl(userKeyHTML_URL));
        break;
    }
    case OSM:
        map_webView->load(QUrl(OSM_URL));
        break;
    default:
        QLocale locale = QLocale::system();
        qDebug()<< "locale:" << locale.country();
        if(   locale.country() == QLocale::China
            || locale.country() == QLocale::Taiwan
            || locale.country() == QLocale::HongKong)
        {
            this->mapType = MapType::TianDiMap;
        }
        else
        {
            this->mapType = MapType::OSM;
        }
        switchOnlineMap(this->mapType);
        break;
    }
}

void MapWebViewWindow::clearOverlays()
{
    /*clear map*/
    map_webView->page()->runJavaScript("remove_overlay()");
}

void MapWebViewWindow::applySettings()
{
    if(mapType == MapType::TianDiMap)
        switchOnlineMap(mapType);
}

void MapWebViewWindow::addMark(const QString &cmd)
{
    map_webView->page()->runJavaScript(cmd);
}
