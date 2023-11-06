#ifndef SKYVIEWWIDGET_H
#define SKYVIEWWIDGET_H

#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QPoint>
#include <QDoubleSpinBox>
#include <QtCore/QtMath>
#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QTranslator>
#include <QMutex>
#include <QLabel>
#include <QMessageBox>
#include <QtWidgets/QGridLayout>
#include <QtCore/QTimer>
#include <QPainter>

#include "DataParser/nmea_parser.h"

#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
#define SATSIZE     20                  // satellite circle size in skyplot

namespace Ui {
class SkyViewWidget;
}

class SkyViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SkyViewWidget(QWidget *parent = nullptr);
    ~SkyViewWidget();

    void update_skymap_view();
    void setGsv_t(GSV_t *value);

    void setSol(Solution_t *value);

private:
    void init_skymap_widget(void);

    void resizeEvent ( QResizeEvent * event );
    /* draw skymap */

private slots:
    /* draw skymap */

    void DrawText1(QPainter *c, int x, int y, const QString &s,const QColor &color, int align);
    void DrawSky(QPainter *c, int w, int h, int x0, int y0);
    void DrawSat(QPainter *c, QPainter *c2, int w, int h, int x0, int y0, int index, int freq);
    void DrawPlot(QLabel *plot,QLabel *plot2, int type, int freq);

private:
    Ui::SkyViewWidget *ui;

    GSV_t *gsv_t;
    Solution_t *sol;

};

#endif // SKYVIEWWIDGET_H
