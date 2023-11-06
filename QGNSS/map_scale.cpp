#include "map_scale.h"
#include <QPainter>

Map_Scale::Map_Scale(QWidget *parent) : QWidget(parent)
{

}

void Map_Scale::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

//    painter.setPen(Qt::NoPen);
//    painter.setBrush(Qt::red);
//    painter.drawRect(rect());

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 8));
    QString label;
    if      (mTickSize<0.01  ) label=QString("%1 mm").arg(mTickSize*1000.0,0,'f',0);
    else if (mTickSize<1.0   ) label=QString("%1 cm").arg(mTickSize*100.0,0,'f',0);
    else if (mTickSize<1000.0) label=QString("%1 m" ).arg(mTickSize,0,'f',0);
    else                label=QString("%1 km").arg(mTickSize/1000.0,0,'f',0);
    painter.drawText(rect(), Qt::AlignCenter, label);
    QVector<QPoint> point;
    int h = 15;
    point<<QPoint(10,this->height()-h);
    point<<QPoint(10,this->height()-10);
    point<<QPoint(10,this->height()-10);
    point<<QPoint(this->width()-10,this->height()-10);
    point<<QPoint(this->width()-10,this->height()-10);
    point<<QPoint(this->width()-10,this->height()-h);
    painter.drawLines(point);
}

void Map_Scale::setTickSize(double tickSize)
{
    mTickSize = tickSize;
    update();
}
