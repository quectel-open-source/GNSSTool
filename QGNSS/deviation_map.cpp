#include "deviation_map.h"
#include "ui_deviation_map.h"
#include "mainwindow.h"
#include "RTKLib/rtklib.h"

Deviation_Map::Deviation_Map(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::Deviation_Map)
{
    ui->setupUi(this);
    this->setWindowTitle("Deviation Map");
    this->setWindowIcon(QIcon(":/images/position.png"));
    QMetaEnum metaEnum = QMetaEnum::fromType<RefPointType>();
    //    ui->comboBox_RPoint->addItem(metaEnum.valueToKey(RefPointType::NoPoint), RefPointType::NoPoint);
    ui->comboBox_RPoint->addItem(metaEnum.valueToKey(RefPointType::Average), RefPointType::Average);
    ui->comboBox_RPoint->addItem(metaEnum.valueToKey(RefPointType::User), RefPointType::User);

    fTimer = new QTimer(this);
    fTimer->setInterval(600);
    connect(fTimer, SIGNAL(timeout()), this, SLOT(updateChart()));
    fTimer->start();

    QRegExp lon_rx("[\\+\\-]?((?:[0-9]|[1-9][0-9]|1[0-7][0-9])\\.([0-9]{0,10})|[0-9]|[1-9][0-9]|1[0-7][0-9]|180)");
    ui->lineEdit_long->setValidator(new QRegExpValidator(lon_rx, ui->lineEdit_long));
    QRegExp lat_rx("[\\+\\-]?((?:[0-9]|[1-8][0-9])\\.([0-9]{0,10})|[0-9]|[1-8][0-9]|90)");
    ui->lineEdit_lat->setValidator(new QRegExpValidator(lat_rx, ui->lineEdit_lat));
    QRegExp alt_rx("[\\+\\-]?((?:[0-9]{0,10})\\.([0-9]{0,10})|[0-9]{0,10})");
    ui->lineEdit_alt->setValidator(new QRegExpValidator(alt_rx, ui->lineEdit_alt));

    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent *)), this, SLOT(mouseWheel(QWheelEvent *)));
    // normalGraph
    normalGraph = ui->customPlot->addGraph();
    normalGraph->setPen(QPen(QColor(Qt::blue)));
    normalGraph->setLineStyle(QCPGraph::lsNone);
    normalGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    normalGraph->setName("Normal Point");

    // rtkGraph
    rtkGraph = ui->customPlot->addGraph();
    rtkGraph->setPen(QPen(QColor(Qt::green)));
    rtkGraph->setLineStyle(QCPGraph::lsNone);
    rtkGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    rtkGraph->setName("RTK Point");

    // avgGraph
    avgGraph = ui->customPlot->addGraph();
    avgGraph->setPen(QPen(Qt::red, 1));
    avgGraph->setLineStyle(QCPGraph::lsNone);
    avgGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusCircle, 10));
    avgGraph->setName("avg Point");

    // set ranges appropriate to show data
    ui->customPlot->xAxis->setRange(-10, 10);
    ui->customPlot->yAxis->setRange(-10, 10);

    ui->customPlot->xAxis->ticker()->setTickCount(10);
    ui->customPlot->xAxis->setTickLabels(false);
    ui->customPlot->xAxis->setBasePen(QPen(Qt::NoPen));
    ui->customPlot->xAxis->setSubTickPen(QPen(Qt::NoPen));
    ui->customPlot->xAxis->setTickPen(QPen(Qt::NoPen));

    ui->customPlot->yAxis->ticker()->setTickCount(10);
    ui->customPlot->yAxis->setTickLabels(false);
    ui->customPlot->yAxis->setBasePen(QPen(Qt::NoPen));
    ui->customPlot->yAxis->setSubTickPen(QPen(Qt::NoPen));
    ui->customPlot->yAxis->setTickPen(QPen(Qt::NoPen));
    ui->customPlot->axisRect(0)->setAutoMargins(false); // 去除边框

    ui->widget_dis_scale->setTickSize(getMapScale());

    // CEP Circle
    cep50Circle = new QCPItemEllipse(ui->customPlot);
    cep50Circle->setPen(QPen(Qt::black, 1, Qt::DashLine));
    cep95Circle = new QCPItemEllipse(ui->customPlot);
    cep95Circle->setPen(QPen(Qt::black, 1, Qt::DashLine));
    cep50Lable = new QCPItemText(ui->customPlot);
    cep50Lable->setFont(QFont("Arial", 7));
    cep95Lable = new QCPItemText(ui->customPlot);
    cep95Lable->setFont(QFont("Arial", 7));
    setCEP_CircleVisible(false);

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectPlottables
    clearMap();
}

Deviation_Map::~Deviation_Map()
{
    delete ui;
    delete fTimer;
}

void Deviation_Map::mouseWheel(QWheelEvent *event)
{
    Q_UNUSED(event)
    qDebug() << getMapScale();
    ui->widget_dis_scale->setTickSize(getMapScale());
}

/* lla2enu -------------------------
 * lla2enu
 * args   : double *origin      I   geodetic position {lat,lon,0} (deg,m)
 *          double *pos        I   geodetic position {lat,lon,0} (deg,m)
 *          double *enu        O   vector in local tangental coordinate {e,n,u}
 * return : none
 *-----------------------------------------------------------------------------*/
void lla2enu(const double *origin, const double *pos, double *enu)
{
    double P[3] = {pos[0] * D2R, pos[1] * D2R, 0};
    double ori[3] = {origin[0] * D2R, origin[1] * D2R, 0};
    double ecefs[3] = {0, 0, 0};
    double ecefp[3] = {0, 0, 0};
    pos2ecef(P, ecefs);
    pos2ecef(ori, ecefp);
    ecefs[0] = ecefs[0] - ecefp[0];
    ecefs[1] = ecefs[1] - ecefp[1];
    ecefs[2] = ecefs[2] - ecefp[2];
    ecef2enu(ori, ecefs, enu);
}

void Deviation_Map::updateChart()
{
    double enu[3] = {0, 0, 0};
    if (mainForm->NMEA_Data.Solution_Data.savedData_L.size() == 0)
    {
        clearMap();
        return;
    }
    // Priority use GGA position
    if (mainForm->NMEA_Data.Solution_Data.savedData_L.size() > 3)
    {
        while (mainForm->NMEA_Data.Solution_Data.savedData_L.size() > en_list.size())
        {
            double org[3] = {mainForm->NMEA_Data.Solution_Data.savedData_L.at(0).Latitude, mainForm->NMEA_Data.Solution_Data.savedData_L.at(0).Longitude, 0};
            double p[3] = {mainForm->NMEA_Data.Solution_Data.savedData_L.at(en_list.size()).Latitude, mainForm->NMEA_Data.Solution_Data.savedData_L.at(en_list.size()).Longitude, 0};
            int quality = mainForm->NMEA_Data.Solution_Data.savedData_L.at(en_list.size()).GPSQuality;
            lla2enu(org, p, enu);
            QPointF en(enu[0], enu[1]);
            if (en_list.size() != 0)
            {
                //                if (fabs(en.x() - en_list.last().x()) > 0.0001 || fabs(en.y() - en_list.last().y()) > 0.0001)
                //                {
                if (quality == 4 || quality == 5)
                {
                    xRTK.append(enu[0]);
                    yRTK.append(enu[1]);
                    rtkGraph->setData(xRTK, yRTK);
                }
                else
                {
                    xNormal.append(enu[0]);
                    yNormal.append(enu[1]);
                    normalGraph->setData(xNormal, yNormal);
                }
                //                }
                en_list.append(en);
            }
            else
            {
                // add "F"
                en_list.append(en);

                xNormal.append(enu[0]);
                yNormal.append(enu[1]);
                normalGraph->setData(xNormal, yNormal);
            }
            //            last_series.clear();
            //            last_series << en;
            if (ui->pushButton_Avg->isChecked())
            {
                double sumX = 0.0;
                double sumY = 0.0;

                for (int i = 0; i < en_list.size(); ++i)
                {
                    sumX += en_list[i].x();
                    sumY += en_list[i].y();
                }

                double avgX = sumX / en_list.size();
                double avgY = sumY / en_list.size();
                avgGraph->data()->clear();
                avgGraph->addData(avgX, avgY);
            }
            else
            {
                avgGraph->data()->clear();
            }
        }
        ui->customPlot->replot();
    }
    if(ui->pushButton_Cal->isChecked())
        calculateCEP();
}

void Deviation_Map::calculateCEP()
{
    if(mainForm->NMEA_Data.Solution_Data.savedData_L.size() < 3)
        return;
    switch (CepType)
    {
    case RefPointType::User:
    {
        while (mainForm->NMEA_Data.Solution_Data.savedData_L.size() > posDelta.size())
        {
            double p[3] = {mainForm->NMEA_Data.Solution_Data.savedData_L.at(posDelta.size()).Latitude, mainForm->NMEA_Data.Solution_Data.savedData_L.at(posDelta.size()).Longitude, 0};
            double enu[3] = {0, 0, 0};
            lla2enu(ref_pos, p, enu);
            posDelta.append(sqrt(enu[0] * enu[0] + enu[1] * enu[1]));
        }
        break;
    }
    case RefPointType::Average:
    {
        if(mainForm->NMEA_Data.Solution_Data.savedData_L.size() > posDelta.size())
        {

            double sumLat = 0.0;
            double sumLon = 0.0;
            posDelta.clear();
            foreach (auto var, mainForm->NMEA_Data.Solution_Data.savedData_L)
            {
                sumLat += var.Latitude;
                sumLon += var.Longitude;
            }
            ref_pos[0] = sumLat / mainForm->NMEA_Data.Solution_Data.savedData_L.size(); // Lat itude
            ref_pos[1] = sumLon / mainForm->NMEA_Data.Solution_Data.savedData_L.size(); // long itude
            ref_pos[2] = 0;
            foreach (auto var, mainForm->NMEA_Data.Solution_Data.savedData_L)
            {
                double p[3] = {var.Latitude, var.Longitude, 0};
                double enu[3] = {0, 0, 0};
                lla2enu(ref_pos, p, enu);
                posDelta.append(sqrt(enu[0] * enu[0] + enu[1] * enu[1]));
            }

        }
        break;
    }
    default:
        break;
    }
    if (CepType != RefPointType::NoPoint)
    {
        qSort(posDelta.begin(), posDelta.end());
        double cep50 = posDelta.at(posDelta.size() / 2);
        double cep95 = posDelta.at(posDelta.size() * 0.95);
        update_CEP_info(cep50, cep95);
    }
}

void Deviation_Map::setCEP_CircleVisible(bool on)
{
    cep50Circle->setVisible(on);
    cep95Circle->setVisible(on);
    cep50Lable->setVisible(on);
    cep95Lable->setVisible(on);
}

void Deviation_Map::clearMap()
{
    xNormal.clear();
    yNormal.clear();
    normalGraph->data()->clear();
    xRTK.clear();
    yRTK.clear();
    rtkGraph->data()->clear();
    avgGraph->data()->clear();

    ui->customPlot->replot();
    en_list.clear();
    posDelta.clear();

    ref_pos[0] = 0;
    ref_pos[1] = 0;
    ref_pos[2] = 0;
    qDebug() << "Deviation_Map clear";
}

double pickClosest(double target, const QVector<double> &candidates)
{
    if (candidates.size() == 1)
        return candidates.first();
    QVector<double>::const_iterator it = std::lower_bound(candidates.constBegin(), candidates.constEnd(), target);
    if (it == candidates.constEnd())
        return *(it - 1);
    else if (it == candidates.constBegin())
        return *it;
    else
        return target - *(it - 1) < *it - target ? *(it - 1) : *it;
}

double Deviation_Map::getMapScale()
{
    auto range = ui->customPlot->xAxis->range();
    int mTickCount = ui->customPlot->xAxis->ticker()->tickCount();
    double exactStep = range.size() / double(mTickCount + 1e-10);
    const double mag = qPow(10.0, qFloor(qLn(exactStep) / qLn(10.0)));
    double magnitude = mag;
    const double mantissa = exactStep / mag;
    return pickClosest(mantissa, QVector<double>() << 1.0 << 2.0 << 2.5 << 5.0 << 10.0) * magnitude;
}

void Deviation_Map::update_CEP_info(double cep50, double cep95)
{
    double org[3] = {mainForm->NMEA_Data.Solution_Data.savedData_L.at(0).Latitude, mainForm->NMEA_Data.Solution_Data.savedData_L.at(0).Longitude, 0};
    double enu[3] = {0, 0, 0};
    lla2enu(org, ref_pos, enu);
    QPointF en(enu[0], enu[1]);
    update_circle(cep50Circle, enu[0], enu[1], cep50);
    cep50Lable->position->setCoords(enu[0] + cep50, enu[1]);
    cep50Lable->setText(QString("CEP50 %1m").arg(cep50, 0, 'f', 2));
    update_circle(cep95Circle, enu[0], enu[1], cep95);
    cep95Lable->position->setCoords(enu[0] + cep95, enu[1]);
    cep95Lable->setText(QString("CEP95 %1m").arg(cep95, 0, 'f', 2));
    setCEP_CircleVisible(true);
}

void Deviation_Map::update_circle(QCPItemEllipse *Circle, double x, double y, double r)
{
    // 圆心C(x, y) 半径r
    // tL(x-r, y+r) bR(x+r, y-r)
    Circle->topLeft->setCoords(x - r, y + r);
    Circle->bottomRight->setCoords(x + r, y - r);
}

void Deviation_Map::on_pushButton_Fit_clicked()
{
    std::vector<double> allRange;
    bool foundRange;
    for (int i = 0; i < ui->customPlot->graphCount(); i++)
    {
        auto gragh = ui->customPlot->graph(i);
        auto range = gragh->data()->keyRange(foundRange);
        if (foundRange)
        {
            allRange.push_back(range.upper);
            allRange.push_back(range.lower);
            range = gragh->data()->valueRange(foundRange);
            allRange.push_back(range.upper);
            allRange.push_back(range.lower);
        }
    }
    if (allRange.size())
    {
        auto max = std::max_element(allRange.begin(), allRange.end());
        auto min = std::min_element(allRange.begin(), allRange.end());
        ui->customPlot->xAxis->setRange((*min) - 0.5, *max + 0.5);
        ui->customPlot->yAxis->setRange((*min) - 0.5, *max + 0.5);
        ui->customPlot->replot();
        ui->widget_dis_scale->setTickSize(getMapScale());
    }
}

void Deviation_Map::on_pushButton_Avg_clicked()
{
    if (ui->pushButton_Avg->isChecked())
    {
        double sumX = 0.0;
        double sumY = 0.0;

        for (int i = 0; i < en_list.size(); ++i)
        {
            sumX += en_list[i].x();
            sumY += en_list[i].y();
        }

        double avgX = sumX / en_list.size();
        double avgY = sumY / en_list.size();
        avgGraph->data()->clear();
        avgGraph->addData(avgX, avgY);
    }
    else
    {
        avgGraph->data()->clear();
    }
}

void Deviation_Map::on_pushButton_Cal_toggled(bool arg1)
{
    qDebug() << "Cal_toggled:" << arg1;
    if (arg1)
    {
        switch (ui->comboBox_RPoint->currentData().toInt())
        {
        case RefPointType::User:
        {
            if (ui->lineEdit_lat->text().isEmpty() || ui->lineEdit_long->text().isEmpty())
            {
                QMessageBox::warning(this, tr("Parameter error"),
                                     tr("Please enter the reference point on the Setup page."),
                                     QMessageBox::Ok,
                                     QMessageBox::Ok);
                ui->pushButton_Cal->setChecked(Qt::Unchecked);
                return;
            }
            ref_pos[0] = ui->lineEdit_lat->text().toDouble();  // Lat itude
            ref_pos[1] = ui->lineEdit_long->text().toDouble(); // long itude
            ref_pos[2] = 0;                                    // alt itude
            if (ref_pos[0] > 90.0 || ref_pos[0] < -90.0 || ref_pos[1] < -180.0 || ref_pos[1] > 180.0)
            {
                QMessageBox::warning(this, tr("Parameter error"),
                                     tr("Please enter the reference point on the Setup page."),
                                     QMessageBox::Ok,
                                     QMessageBox::Ok);
                ui->pushButton_Cal->setChecked(Qt::Unchecked);
                return;
            }
            ui->lineEdit_long->setEnabled(!arg1);
            ui->lineEdit_lat->setEnabled(!arg1);
            ui->lineEdit_alt->setEnabled(!arg1);
            CepType = RefPointType::User;
            break;
        }
        case RefPointType::Average:
        {
            CepType = RefPointType::Average;
            break;
        }
        default:
            break;
        }
    }
    else
    {
        setCEP_CircleVisible(arg1);
        CepType = RefPointType::NoPoint;
        posDelta.clear();
        ui->lineEdit_long->setEnabled(!arg1);
        ui->lineEdit_lat->setEnabled(!arg1);
        ui->lineEdit_alt->setEnabled(!arg1);
    }
}
