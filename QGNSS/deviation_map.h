#ifndef DEVIATION_MAP_H
#define DEVIATION_MAP_H

#include <QWidget>
#include "QTimer"
#include "qcustomplot.h"


namespace Ui
{
    class Deviation_Map;
}

class Deviation_Map : public QWidget
{
    Q_OBJECT


public:
    explicit Deviation_Map(QWidget *parent = nullptr);
    ~Deviation_Map();

    enum RefPointType { // pen style
        NoPoint,
        Average,
        User,
    };
    Q_ENUM(RefPointType)

protected:
//    void paintEvent(QPaintEvent *event);

private slots:
    void mouseWheel(QWheelEvent *event);

    void updateChart();

    void clearMap();

    void on_pushButton_Fit_clicked();

    void on_pushButton_Avg_clicked();

    void on_pushButton_Cal_toggled(bool checked);

private:
    void calculateCEP();
    void setCEP_CircleVisible(bool on);
    void update_CEP_info(double cep50, double cep95);
    void update_circle(QCPItemEllipse *Circle, double x, double y, double r);
    double getMapScale();
    Ui::Deviation_Map *ui;
    QTimer *fTimer;
    QList<QPointF> en_list;
    QList<double> posDelta;
    bool hasActulalLocation = false;
    double ref_pos[3];

    //rtkGraph
    QCPGraph * avgGraph;
    //normalGraph
    QCPGraph * normalGraph;
    QVector<double> xNormal, yNormal;
    //rtkGraph
    QCPGraph * rtkGraph;
    QVector<double> xRTK, yRTK;
    //CEP
    QCPItemEllipse *cep50Circle,*cep95Circle;
    QCPItemText *cep50Lable, *cep95Lable;
    RefPointType CepType  = NoPoint;
};

#endif // DEVIATION_MAP_H
