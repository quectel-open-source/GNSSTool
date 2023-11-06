#include "msg_dialog.h"
#include "ui_msg_dialog.h"
#include "DataParser/rtcm3parser.h"
#include "QDebug"
#include "QTimer"

Msg_Dialog::Msg_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Msg_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    this->setWindowTitle("Messages View");
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    this->setAttribute(Qt::WA_QuitOnClose, false);
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it) {
         itemMap.insert((*it)->text(0).split(' ')[0],*it);
         (*it)->text(0);
        ++it;
    }
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(200);
//    ui->tableWidget->setRowCount(1000);
//    ui->tableWidget->setColumnCount(4);
    normalHeader<<"Parameter"<<"Value"<<"Unit"<<"Description";
//    ui->tableWidget->setHorizontalHeaderLabels(normalHeader);
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    respHeader<<"  ID  "<<" Result "<<""<<"";

    //=================================================================
    //Heading
    ui->widgetHeading->yAxis->setLabel("Heading");
    connect(ui->widgetHeading,SIGNAL(mouseMove(QMouseEvent *)),this,SLOT(accMoveMouseEventH(QMouseEvent *)));
    connect(ui->widgetHeading, SIGNAL(mouseRelease()), this, SLOT(mouseReleaseSlotsAcc()));
    QFont font = ui->widgetHeading->font();
    font.setPointSize(10);
    font.setBold(true);
    font.setFamily("Arial");
    ui->widgetHeading->xAxis->setVisible(PLOTFALG);
    ui->widgetHeading->yAxis->setLabelFont(font);
    ui->widgetHeading->yAxis->setLabelFont(font);
    tracerHeading = new QCPItemTracer(ui->widgetHeading);
    tracerHeading->setInterpolating(false);
    tracerHeading->setStyle(QCPItemTracer::tsCircle);
    tracerHeading->setPen(QPen(Qt::red));
    tracerHeading->setBrush(Qt::red);
    tracerHeading->setSize(6);
    //
    ui->widgetPitch->yAxis->setLabel("Pitch");
    connect(ui->widgetPitch,SIGNAL(mouseMove(QMouseEvent *)),this,SLOT(accMoveMouseEventP(QMouseEvent *)));
    connect(ui->widgetPitch, SIGNAL(mouseRelease()), this, SLOT(mouseReleaseSlotsAcc()));
    font = ui->widgetPitch->font();
    font.setPointSize(10);
    font.setBold(true);
    font.setFamily("Arial");
    ui->widgetPitch->xAxis->setVisible(PLOTFALG);
    ui->widgetPitch->yAxis->setLabelFont(font);
    ui->widgetPitch->yAxis->setLabelFont(font);
    tracerPitch = new QCPItemTracer(ui->widgetPitch);
    tracerPitch->setInterpolating(false);
    tracerPitch->setStyle(QCPItemTracer::tsCircle);
    tracerPitch->setPen(QPen(Qt::red));
    tracerPitch->setBrush(Qt::red);
    tracerPitch->setSize(6);
    //
    ui->widgetRoll->yAxis->setLabel("Roll");
    connect(ui->widgetRoll,SIGNAL(mouseMove(QMouseEvent *)),this,SLOT(accMoveMouseEventR(QMouseEvent *)));
    connect(ui->widgetRoll, SIGNAL(mouseRelease()), this, SLOT(mouseReleaseSlotsAcc()));
    font = ui->widgetRoll->font();
    font.setPointSize(10);
    font.setBold(true);
    font.setFamily("Arial");
    ui->widgetRoll->xAxis->setVisible(PLOTFALG);
    ui->widgetRoll->yAxis->setLabelFont(font);
    ui->widgetRoll->yAxis->setLabelFont(font);
    tracerRoll = new QCPItemTracer(ui->widgetRoll);
    tracerRoll->setInterpolating(false);
    tracerRoll->setStyle(QCPItemTracer::tsCircle);
    tracerRoll->setPen(QPen(Qt::red));
    tracerRoll->setBrush(Qt::red);
    tracerRoll->setSize(6);

    timerAcc = new QTimer(this);
    timerAcc->start();
    connect(timerAcc, SIGNAL(timeout()), this, SLOT(timeToRePlotAcc()));
    plotColor.append("#00AAFF");
    plotColor.append("#FF0000");
    plotColor.append("#00AA00");

//    pAccMenu = new QMenu(this);
//    acc_Clear = new QAction(tr("Clear"), this);
//    acc_Reset = new QAction(tr("Refresh"), this);
//    acc_Datacomparison = new QAction(tr("Comparison"), this);
//    acc_Datacomparison->setCheckable(true);
//    acc_Datacomparison->setChecked(true);

    showPlotAcc();

    xAcc    .clear();
    xUTC    .clear();
    yHeading.clear();
    yPitch  .clear();
    yRoll   .clear();
    TimeQMap.clear();
}

Msg_Dialog::~Msg_Dialog()
{
    delete ui;
}

void Msg_Dialog::update_type999()
{
    itemMap["999"]->setHidden(!st_HAP._n);
    update_type999_sub1();
    update_type999_sub2();
    update_type999_sub19();
    update_type999_sub21();
}

void Msg_Dialog::update_type999_sub1()
{
    itemMap["1"]->setHidden(!st_HAP.rss._n);
    itemMap["1"]->setText(1,QString("%1").arg(st_HAP.rss._n));
    if(ui->treeWidget->currentItem()->text(0).split(' ')[0] == "1")
    {
        model->setItem(0,0,new QStandardItem("TOW"));
        model->setItem(0,1,new QStandardItem(QString("%1").arg(st_HAP.rss.TOW)));
        model->setItem(0,2,new QStandardItem("ms"));

        model->setItem(1,0,new QStandardItem("GPS Ext. Week Number"));
        model->setItem(1,1,new QStandardItem(QString("%1").arg(st_HAP.rss.weekNum)));
        model->setItem(1,3,new QStandardItem("Best time converted to GPS system time"));

        model->setItem(2,0,new QStandardItem("Leap Seconds"));
        model->setItem(2,1,new QStandardItem(QString("%1").arg(st_HAP.rss.LeapSeconds)));
        model->setItem(2,2,new QStandardItem("s"));

        model->setItem(3,0,new QStandardItem("Safety info"));
        model->setItem(3,1,new QStandardItem(QString("%1").arg(st_HAP.rss.safelyInfo)));
        model->setItem(3,3,new QStandardItem("0 = not available"));

        model->setItem(4,0,new QStandardItem("ProtocolVersionFlags"));
        model->setItem(4,1,new QStandardItem(QString("%1").arg(st_HAP.rss.protocolVersionFlags)));
        model->setItem(4,3,new QStandardItem("0, 1 or 2"));

        model->setItem(5,0,new QStandardItem("Firmware Version"));
        model->setItem(5,1,new QStandardItem(QString("0x%1").arg(st_HAP.rss.FirmwareVersion, 0, 16)));
        model->setItem(5,3,new QStandardItem("0xffffff = N/A"));

        model->setItem(6,0,new QStandardItem("PPS Status"));
        model->setItem(6,1,new QStandardItem(QString("%1").arg(st_HAP.rss.ppsStatus)));
        model->setItem(6,3,new QStandardItem("0=ok  1=not available"));

        model->setItem(7,0,new QStandardItem("Time Validity"));
        model->setItem(7,1,new QStandardItem(QString("%1").arg(st_HAP.rss.TimeValidity)));

        model->setItem(8,0,new QStandardItem("Constellation Alarm Mask"));
        model->setItem(8,1,new QStandardItem(QString("0x%1").arg(st_HAP.rss.constellationAlarmMask, 0, 16)));
        model->setItem(8,3,new QStandardItem("0=ok 1=excluded"));

        model->setItem(9,0,new QStandardItem("GNSS Constellation Mask"));
        model->setItem(9,1,new QStandardItem(QString("0x%1").arg(st_HAP.rss.GNSS_ConstellationMask, 0, 16)));

        model->setItem(10,0,new QStandardItem("Multi-Frequency Constellation Mask"));
        model->setItem(10,1,new QStandardItem(QString("0x%1").arg(st_HAP.rss.Multi_FrequencyConstellationMask, 0, 16)));

        model->setItem(11,0,new QStandardItem("NCO clock drift"));
        model->setItem(11,1,new QStandardItem(QString("%1").arg(st_HAP.rss.NCO_clockDrift)));
    }
}

void Msg_Dialog::update_type999_sub2()
{
    itemMap["2"]->setHidden(!st_HAP.rcc._n);
    itemMap["2"]->setText(1,QString("%1").arg(st_HAP.rcc._n));
    if(ui->treeWidget->currentItem()->text(0).split(' ')[0] == "2")
    {

        model->setItem(0,0,new QStandardItem("Version"));
        model->setItem(0,1,new QStandardItem(QString(g_customData.lg96taa_version)));

        model->setItem(1,0,new QStandardItem("Response ID"));
        model->setItem(1,1,new QStandardItem(QString("0x%1").arg(st_HAP.rcc.ResponseID, 0, 16)));
//        model->setItem(0,2,new QStandardItem(""));

        model->setItem(2,0,new QStandardItem("Config. Page Number"));
        model->setItem(2,1,new QStandardItem(QString("%1").arg(st_HAP.rcc.ConfigPageNumber)));
//        model->setItem(1,3,new QStandardItem("Best time converted to GPS system time"));

        model->setItem(3,0,new QStandardItem("Continue"));
        model->setItem(3,1,new QStandardItem(QString("%1").arg(st_HAP.rcc.ContinueOnNextMessage)));
//        model->setItem(2,2,new QStandardItem("s"));
    }


}

void Msg_Dialog::update_type999_sub19()
{
    itemMap["19"]->setHidden(!g_customData._n_resp);
    itemMap["19"]->setText(1,QString("%1").arg(g_customData._n_resp));
    if(ui->treeWidget->currentItem()->text(0).split(' ')[0] == "19"){
        for (int i = 0;i<1000;i++) {
            if(g_customData.resp[i][0]){
                model->setItem(i,0,new QStandardItem(QString("  %1").arg(g_customData.resp[i][0])));
                model->setItem(i,1,new QStandardItem(resp_Str[g_customData.resp[i][1]]));
            }
            else
                break;
        }
    }
}

void Msg_Dialog::update_type999_sub21()
{
    itemMap["21"]->setHidden(!st_HAP.epvt._n);
    itemMap["21"]->setText(1,QString("%1").arg(st_HAP.epvt._n));
    if(ui->treeWidget->currentItem()->text(0).split(' ')[0] == "21")
    {
        model->setItem(0,0,new QStandardItem("Reference Station ID"));
        model->setItem(0,1,new QStandardItem(QString("0x%1").arg(st_HAP.epvt.ReferenceStationID, 0, 16)));
//        model->setItem(0,2,new QStandardItem(""));
        model->setItem(0,3,new QStandardItem("Invalid = 0x3FF"));

        model->setItem(1,0,new QStandardItem("ITRF Realization Year"));
        model->setItem(1,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.ReservedForITRF_RealizationYear)));
//        model->setItem(1,2,new QStandardItem(""));
//        model->setItem(1,3,new QStandardItem(""));

        model->setItem(2,0,new QStandardItem("GPS Quality Indicator"));
        model->setItem(2,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.GPS_Quality)));
//        model->setItem(2,2,new QStandardItem(""));
//        model->setItem(2,3,new QStandardItem(""));

        model->setItem(3,0,new QStandardItem("Data status"));
        model->setItem(3,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.DataStatus)));
//        model->setItem(3,2,new QStandardItem(""));
        model->setItem(3,3,new QStandardItem("0 = Data valid,1 = Navigation receiver warning"));

        model->setItem(4,0,new QStandardItem("Fix frequency mode"));
        model->setItem(4,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.FixFrequencyMode)));
//        model->setItem(4,2,new QStandardItem(""));
//        model->setItem(4,3,new QStandardItem(""));

        model->setItem(5,0,new QStandardItem("Fix integrity"));
        model->setItem(5,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.FixIntegrity)));
//        model->setItem(5,2,new QStandardItem(""));
//        model->setItem(5,3,new QStandardItem(""));

        model->setItem(6,0,new QStandardItem("RFU"));
        model->setItem(6,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.RFU)));
//        model->setItem(6,2,new QStandardItem(""));
//        model->setItem(6,3,new QStandardItem(""));

        model->setItem(7,0,new QStandardItem("Number of satellites in use"));
        model->setItem(7,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.satellitesInUse)));
//        model->setItem(7,2,new QStandardItem(""));
//        model->setItem(7,3,new QStandardItem(""));

        model->setItem(8,0,new QStandardItem("Number of satellites in view"));
        model->setItem(8,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.satellitesInView)));
//        model->setItem(8,2,new QStandardItem(""));
//        model->setItem(8,3,new QStandardItem(""));

        model->setItem(9,0,new QStandardItem("HDOP"));
        model->setItem(9,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.HDOP)));
//        model->setItem(9,2,new QStandardItem(""));
//        model->setItem(9,3,new QStandardItem(""));

        model->setItem(10,0,new QStandardItem("VDOP"));
        model->setItem(10,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.VDOP)));
//        model->setItem(10,2,new QStandardItem(""));
//        model->setItem(10,3,new QStandardItem(""));

        model->setItem(11,0,new QStandardItem("PDOP"));
        model->setItem(11,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.PDOP)));
//        model->setItem(11,2,new QStandardItem(""));
//        model->setItem(11,3,new QStandardItem(""));

        model->setItem(12,0,new QStandardItem("Geoidal separation, meters"));
        model->setItem(12,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.GeoidalSeparation)));
//        model->setItem(12,2,new QStandardItem(""));
//        model->setItem(12,3,new QStandardItem(""));

        model->setItem(13,0,new QStandardItem("Age of Differentials"));
        model->setItem(13,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.AgeOfDifferentials)));
//        model->setItem(13,2,new QStandardItem(""));
//        model->setItem(13,3,new QStandardItem(""));

        model->setItem(14,0,new QStandardItem("Differential Reference Station ID"));
        model->setItem(14,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.DifferentialReferenceStationID)));
//        model->setItem(14,2,new QStandardItem(""));
//        model->setItem(14,3,new QStandardItem(""));

        model->setItem(15,0,new QStandardItem("Time ID"));
        model->setItem(15,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.TimeID)));
//        model->setItem(15,2,new QStandardItem(""));
//        model->setItem(15,3,new QStandardItem(""));

        model->setItem(16,0,new QStandardItem("Time Validity"));
        model->setItem(16,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.TimeValidity)));
//        model->setItem(16,2,new QStandardItem(""));
//        model->setItem(16,3,new QStandardItem(""));

        model->setItem(17,0,new QStandardItem("GNSS Epoch Time"));
        model->setItem(17,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.GNSS_EpochTime)));
        model->setItem(17,2,new QStandardItem("ms"));
        model->setItem(17,3,new QStandardItem("Not valid=0x3FFFFFFF"));

        model->setItem(18,0,new QStandardItem("Extended Week Number"));
        model->setItem(18,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.ExtendedWeekNumber)));
//        model->setItem(18,2,new QStandardItem(""));
//        model->setItem(18,3,new QStandardItem(""));

        model->setItem(19,0,new QStandardItem("Leap Seconds, GPS-UTC"));
        model->setItem(19,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.LeapSeconds)));
//        model->setItem(19,2,new QStandardItem(""));
//        model->setItem(19,3,new QStandardItem(""));

        model->setItem(20,0,new QStandardItem("Latitude"));
        model->setItem(20,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.Latitude)));
        model->setItem(20,2,new QStandardItem("0.001 [arcsec]"));
//        model->setItem(20,3,new QStandardItem(""));

        model->setItem(21,0,new QStandardItem("Longitude"));
        model->setItem(21,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.Longitude)));
        model->setItem(21,2,new QStandardItem("0.001 [arcsec]"));
//        model->setItem(21,3,new QStandardItem(""));

        model->setItem(22,0,new QStandardItem("Height"));
        model->setItem(22,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.Height)));
        model->setItem(22,2,new QStandardItem("0.1 [m]"));
//        model->setItem(22,3,new QStandardItem(""));

        model->setItem(23,0,new QStandardItem("Velocity Horizontal"));
        model->setItem(23,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.VelocityHorizontal)));
        model->setItem(23,2,new QStandardItem("0.01 [m/s]"));
//        model->setItem(23,3,new QStandardItem(""));

        model->setItem(24,0,new QStandardItem("Velocity Vertical"));
        model->setItem(24,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.VelocityVertical)));
        model->setItem(24,2,new QStandardItem("0.01 [m/s]"));
//        model->setItem(24,3,new QStandardItem(""));

        model->setItem(25,0,new QStandardItem("Course Angle"));
        model->setItem(25,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.CourseAngle)));
        model->setItem(25,2,new QStandardItem("0.1 [deg]"));
//        model->setItem(25,3,new QStandardItem(""));

        model->setItem(26,0,new QStandardItem("Protection Level Horizontal"));
        model->setItem(26,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.ProtectionLevelH)));
        model->setItem(26,2,new QStandardItem("0.01 [m](N/A = 0xFFFF)"));
//        model->setItem(26,3,new QStandardItem(""));

        model->setItem(27,0,new QStandardItem("Protection Level Vertical"));
        model->setItem(27,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.ProtectionLevelV)));
        model->setItem(27,2,new QStandardItem("0.01 [m](N/A = 0xFFFF)"));
//        model->setItem(27,3,new QStandardItem(""));

        model->setItem(28,0,new QStandardItem("Protection Level Angle"));
        model->setItem(28,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.ProtectionLevelA)));
        model->setItem(28,2,new QStandardItem("0.01 [deg](N/A = 0xFFFF)"));
//        model->setItem(28,3,new QStandardItem(""));

        model->setItem(29,0,new QStandardItem("Receiver clock bias"));
        model->setItem(29,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.ReceiverClockBias)));
        model->setItem(29,2,new QStandardItem("[mm]"));
//        model->setItem(29,3,new QStandardItem(""));

        model->setItem(30,0,new QStandardItem("Receiver Clock Drift"));
        model->setItem(30,1,new QStandardItem(QString("%1").arg(st_HAP.epvt.ReceiverClockDrift)));
        model->setItem(30,2,new QStandardItem("[cm/s]"));
//        model->setItem(30,3,new QStandardItem(""));
    }
}

void Msg_Dialog::update_type1005()
{
    itemMap["1005"]->setHidden(!rtcm.nmsg3[5]);
    itemMap["1005"]->setText(1,QString("%1").arg(rtcm.nmsg3[5]));
    if(rtcm.nmsg3[5]&&ui->treeWidget->currentItem()->text(0) == "1005")
    {
        model->setItem(0,1,new QStandardItem(QString("%1").arg(rtcm.sta.pos[0],8,'f')));
        model->setItem(1,1,new QStandardItem(QString("%1").arg(rtcm.sta.pos[1],8,'f')));
        model->setItem(2,1,new QStandardItem(QString("%1").arg(rtcm.sta.pos[2],8,'f')));
        model->setItem(3,1,new QStandardItem(QString("%1").arg(rtcm.sta.hgt,8,'f')));
    }
}

void Msg_Dialog::update_type1006()
{
    itemMap["1006"]->setHidden(!rtcm.nmsg3[6]);
    itemMap["1006"]->setText(1,QString("%1").arg(rtcm.nmsg3[6]));
    if(rtcm.nmsg3[6]&&ui->treeWidget->currentItem()->text(0) == "1006")
    {
        model->setItem(0,1,new QStandardItem(QString("%1").arg(rtcm.sta.pos[0],8,'f')));
        model->setItem(1,1,new QStandardItem(QString("%1").arg(rtcm.sta.pos[1],8,'f')));
        model->setItem(2,1,new QStandardItem(QString("%1").arg(rtcm.sta.pos[2],8,'f')));
        model->setItem(3,1,new QStandardItem(QString("%1").arg(rtcm.sta.hgt,8,'f')));
    }
}

void Msg_Dialog::update_type1029()
{
    itemMap["1029"]->setHidden(!rtcm.nmsg3[29]);
    itemMap["1029"]->setText(1,QString("%1").arg(rtcm.nmsg3[29]));
    if(rtcm.nmsg3[29]&&ui->treeWidget->currentItem()->text(0) == "1029")
    {
        model->setItem(0,1,new QStandardItem(QString("%1").arg(rtcm.msg)));
    }
}

void Msg_Dialog::textDataUpdateAcc()
{
    if(yHeading.count()>0){
        ui->labelHeadingRT->setText(QString::number(yHeading[yHeading.count()-1],'f',6));
        ui->labelPitchRT  ->setText(QString::number(yPitch[yPitch.count()-1],'f',6));
        ui->labelRollRT   ->setText(QString::number(yRoll[yRoll.count()-1],'f',6));
    }else if (yHeading.count()==0){
        ui->labelHeadingRT->setText("0.000000");
        ui->labelPitchRT  ->setText("0.000000");
        ui->labelRollRT   ->setText("0.000000");
    }
}

void Msg_Dialog::showPlotAcc()
{
    QPen pen;
    pen.setWidth(1);
    pen.setStyle(Qt::PenStyle::SolidLine);
    pen.setColor("#00AAFF");
    ui->widgetHeading->addGraph(ui->widgetHeading->xAxis, ui->widgetHeading->yAxis);
    ui->widgetHeading->graph(0)->setName("Heading  ");
    ui->widgetHeading->graph(0)->setPen(pen);
    ui->widgetHeading->graph()->setAntialiased(true);
    ui->widgetHeading->setNoAntialiasingOnDrag(false);
    ui->widgetPitch->addGraph(ui->widgetPitch->xAxis, ui->widgetPitch->yAxis);
    ui->widgetPitch->graph(0)->setName("Pitch  ");
    pen.setColor("#FF0000");
    ui->widgetPitch->graph(0)->setPen(pen);
    ui->widgetPitch->graph()->setAntialiased(true);
    ui->widgetPitch->setNoAntialiasingOnDrag(false);
    ui->widgetRoll->addGraph(ui->widgetRoll->xAxis, ui->widgetRoll->yAxis);
    ui->widgetRoll->graph(0)->setName("Roll  ");
    pen.setColor("#00AA00");
    ui->widgetRoll->graph(0)->setPen(pen);
    ui->widgetRoll->graph()->setAntialiased(true);
    ui->widgetRoll->setNoAntialiasingOnDrag(false);
}

void Msg_Dialog::clearAllData()
{
    ui->widgetHeading->graph(0)->data().data()->clear();
    ui->widgetPitch  ->graph(0)->data().data()->clear();
    ui->widgetRoll   ->graph(0)->data().data()->clear();
    ui->widgetHeading->replot(QCustomPlot::rpQueuedReplot);
    ui->widgetPitch  ->replot(QCustomPlot::rpQueuedReplot);
    ui->widgetRoll   ->replot(QCustomPlot::rpQueuedReplot);
    yHeading.clear();
    yPitch  .clear();
    yRoll   .clear();
    xAcc    .clear();
    xUTC    .clear();
    TimeQMap.clear();
//    textDataUpdateAcc();
    ui->labelHeadingRT->setText("0.000000");
    ui->labelPitchRT  ->setText("0.000000");
    ui->labelRollRT   ->setText("0.000000");
}

void Msg_Dialog::getParseDataLC02H(NMEA_t msg)
{
    xUTC.append(msg.LC02H.dUTC);
    yHeading.append(msg.LC02H.dHeading);
    yPitch.append(msg.LC02H.dPitch);
    yRoll.append(msg.LC02H.dRoll);
    xAcc.append(msg.LC02H.nIndex);
    TimeQMap.insert(msg.LC02H.nIndex,msg.LC02H.dUTC);

    qDebug()<<QString::number(msg.LC02H.nIndex);
    qDebug()<<QString::number(msg.LC02H.dUTC,'f',3);
    qDebug()<<QString::number(msg.LC02H.dRoll,'f',6);
}

void Msg_Dialog::timeToRePlotAcc()
{
    auto maxHeading = std::max_element(std::begin(yHeading), std::end(yHeading));
    auto maxPitch   = std::max_element(std::begin(yPitch  ), std::end(yPitch  ));
    auto maxRoll    = std::max_element(std::begin(yRoll   ), std::end(yRoll   ));
    auto minHeading = std::min_element(std::begin(yHeading), std::end(yHeading));
    auto minPitch   = std::min_element(std::begin(yPitch  ), std::end(yPitch  ));
    auto minRoll    = std::min_element(std::begin(yRoll   ), std::end(yRoll   ));
    QCustomPlot* customPlot = ui->widgetHeading;
    customPlot->graph(0)->setData(xAcc,yHeading);
    customPlot->rescaleAxes();
    customPlot->yAxis->setRange(*minHeading-1,*maxHeading+1);
    if(xAcc.count()>PLOTCOUNT){
        customPlot->xAxis->setRange(xAcc[xAcc.count()-PLOTCOUNT-1],xAcc[xAcc.count()-1]);
    }else{
        customPlot->xAxis->setRange(xAcc[0],xAcc[0]+PLOTCOUNT);
    }
    customPlot->replot(QCustomPlot::rpQueuedReplot);
    //
    customPlot = ui->widgetPitch;
    customPlot->graph(0)->setData(xAcc,yPitch);
    customPlot->rescaleAxes();
    customPlot->yAxis->setRange(*minPitch-1,*maxPitch+1);
    if(xAcc.count()>PLOTCOUNT){
        customPlot->xAxis->setRange(xAcc[xAcc.count()-PLOTCOUNT-1],xAcc[xAcc.count()-1]);
    }else{
        customPlot->xAxis->setRange(xAcc[0],xAcc[0]+PLOTCOUNT);
    }
    customPlot->replot(QCustomPlot::rpQueuedReplot);
    //
    customPlot = ui->widgetRoll;
    customPlot->graph(0)->setData(xAcc,yRoll);
    customPlot->rescaleAxes();
    customPlot->yAxis->setRange(*minRoll,*maxRoll);
    if(xAcc.count()>PLOTCOUNT){
        customPlot->xAxis->setRange(xAcc[xAcc.count()-PLOTCOUNT],xAcc[xAcc.count()-1]);
    }else{
        customPlot->xAxis->setRange(xAcc[0],xAcc[0]+PLOTCOUNT);
    }
    customPlot->replot(QCustomPlot::rpQueuedReplot);

    textDataUpdateAcc();

}

void Msg_Dialog::accMoveMouseEventH(QMouseEvent *event)
{
    if(flagAcc){
        if((ui->widgetHeading->graph(0)->dataCount()>0)/*&&((flagAcc))*//*&&acc_Datacomparison->isChecked()*/){
            tracerHeading->setVisible(true);
            QCPGraph *mGraph = ui->widgetHeading->graph(0);
            tracerHeading->setPen(QPen(Qt::DashLine));
            tracerHeading->setStyle(QCPItemTracer::tsCrosshair);
            double x = ui->widgetHeading->xAxis->pixelToCoord(event->pos().x());
            mGraph = ui->widgetHeading->graph(0);
            tracerHeading->setGraph(mGraph);
            tracerHeading->setGraphKey(x);
            double traceX = tracerHeading->position->key();
            QString temps=QString("<tr><%1</tr> ").arg("");
            int countGraph = ui->widgetHeading->graphCount();
            for (int i = 0; i < countGraph; ++i)
            {
                if(ui->widgetHeading->graph(i)->visible()){
                    QCPDataContainer<QCPGraphData>::const_iterator coorPoint = ui->widgetHeading->graph(i)->data().data()->findBegin(traceX,false);
                    temps.append( QString("<tr><td><font color=%1>&nbsp;%2&nbsp;</font></td><td><font color=%1>,</font></td>"
                                          "<td><font color=%1>UTC:&nbsp;%3 </font></td><td><font color=%1>,</font></td>"
                                          "<td><font color=%1>V:&nbsp;%4</font></td></tr>")
                                  .arg(plotColor[0].name())
                            .arg(ui->widgetHeading->graph(i)->name())
                            .arg(QString::number(TimeQMap.value(traceX),'f',3))
                            .arg(QString::number(coorPoint->value,'f',6)));
                }
            }
            QToolTip::showText(event->globalPos(), QString("<h4>%1</h4> <table>%2</table> ").arg("Comparison").arg(temps), this, this->rect());
            ui->widgetHeading->replot(QCustomPlot::rpQueuedReplot);
        }else{
            tracerHeading->setVisible(false);
            ui->widgetHeading->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void Msg_Dialog::accMoveMouseEventP(QMouseEvent *event)
{
    if(flagAcc){
        if((ui->widgetPitch->graph(0)->dataCount()>0)/*&&((flagAcc))*//*&&acc_Datacomparison->isChecked()*/){
            tracerPitch->setVisible(true);
            QCPGraph *mGraph = ui->widgetPitch->graph(0);
            tracerPitch->setPen(QPen(Qt::DashLine));
            tracerPitch->setStyle(QCPItemTracer::tsCrosshair);
            double x = ui->widgetPitch->xAxis->pixelToCoord(event->pos().x());
            mGraph = ui->widgetPitch->graph(0);
            tracerPitch->setGraph(mGraph);
            tracerPitch->setGraphKey(x);
            double traceX = tracerPitch->position->key();
            QString temps=QString("<tr><%1</tr> ").arg("");
            int countGraph = ui->widgetPitch->graphCount();
            for (int i = 0; i < countGraph; ++i)
            {
                if(ui->widgetPitch->graph(i)->visible()){
                    QCPDataContainer<QCPGraphData>::const_iterator coorPoint = ui->widgetPitch->graph(i)->data().data()->findBegin(traceX,false);
                    temps.append( QString("<tr><td><font color=%1>&nbsp;%2&nbsp;</font></td><td><font color=%1>,</font></td>"
                                          "<td><font color=%1>UTC:&nbsp;%3 </font></td><td><font color=%1>,</font></td>"
                                          "<td><font color=%1>V:&nbsp;%4</font></td></tr>")
                                  .arg(plotColor[1].name())
                            .arg(ui->widgetPitch->graph(i)->name())
                            .arg(QString::number(TimeQMap.value(traceX),'f',3))
                            .arg(QString::number(coorPoint->value,'f',6)));
                }
            }
            QToolTip::showText(event->globalPos(), QString("<h4>%1</h4> <table>%2</table> ").arg("Comparison").arg(temps), this, this->rect());
            ui->widgetPitch->replot(QCustomPlot::rpQueuedReplot);
        }else{
            tracerPitch->setVisible(false);
            ui->widgetPitch->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void Msg_Dialog::accMoveMouseEventR(QMouseEvent *event)
{
    if(flagAcc){
        if((ui->widgetRoll->graph(0)->dataCount()>0)/*&&((flagAcc))*//*&&acc_Datacomparison->isChecked()*/){
            tracerRoll->setVisible(true);
            QCPGraph *mGraph = ui->widgetRoll->graph(0);
            tracerRoll->setPen(QPen(Qt::DashLine));
            tracerRoll->setStyle(QCPItemTracer::tsCrosshair);
            double x = ui->widgetRoll->xAxis->pixelToCoord(event->pos().x());
            mGraph = ui->widgetRoll->graph(0);
            tracerRoll->setGraph(mGraph);
            tracerRoll->setGraphKey(x);
            double traceX = tracerRoll->position->key();
            QString temps=QString("<tr><%1</tr> ").arg("");
            int countGraph = ui->widgetRoll->graphCount();
            for (int i = 0; i < countGraph; ++i)
            {
                if(ui->widgetRoll->graph(i)->visible()){
                    QCPDataContainer<QCPGraphData>::const_iterator coorPoint = ui->widgetRoll->graph(i)->data().data()->findBegin(traceX,false);
                    temps.append( QString("<tr><td><font color=%1>&nbsp;%2&nbsp;</font></td><td><font color=%1>,</font></td>"
                                          "<td><font color=%1>UTC:&nbsp;%3 </font></td><td><font color=%1>,</font></td>"
                                          "<td><font color=%1>V:&nbsp;%4</font></td></tr>")
                                  .arg(plotColor[i].name())
                                  .arg(ui->widgetRoll->graph(i)->name())
                                  .arg(QString::number(TimeQMap.value(traceX),'f',3))
                                  .arg(QString::number(coorPoint->value,'f',6)));
                }
            }
            QToolTip::showText(event->globalPos(), QString("<h4>%1</h4> <table>%2</table> ").arg("Comparison").arg(temps), this, this->rect());
            ui->widgetRoll->replot(QCustomPlot::rpQueuedReplot);
        }else{
            tracerRoll->setVisible(false);
            ui->widgetRoll->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}
//void Msg_Dialog::mouseReleaseSlotsAcc()
//{

//}

void Msg_Dialog::timerUpdate()
{
    update_type999();
    update_type1005();
    update_type1006();
    update_type1029();
}

void Msg_Dialog::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
//     ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>(current->text(1))[0]);
ui->stackedWidget->setCurrentIndex(0);
    Q_UNUSED(previous)
    QTreeWidgetItem *c = current;
    QString curPath = c->text(0);
    while(c->parent()){
        curPath=c->parent()->text(0)+" - "+curPath;
        c=c->parent();
    }
    ui->label_curPath->setText(curPath);
    model->clear();
    model->setHorizontalHeaderLabels(normalHeader);
//    ui->tableWidget->setColumnCount(4);
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    if(current->text(0) == "1005"){
        model->setItem(0,0,new QStandardItem("X"));
        model->setItem(1,0,new QStandardItem("Y"));
        model->setItem(2,0,new QStandardItem("Z"));
        model->setItem(3,0,new QStandardItem("Antenna Height"));
        model->setItem(0,2,new QStandardItem("m"));
        model->setItem(1,2,new QStandardItem("m"));
        model->setItem(2,2,new QStandardItem("m"));
        model->setItem(3,2,new QStandardItem("m"));
    }
    else if(current->text(0) == "1006"){
        model->setItem(0,0,new QStandardItem("X"));
        model->setItem(1,0,new QStandardItem("Y"));
        model->setItem(2,0,new QStandardItem("Z"));
        model->setItem(3,0,new QStandardItem("Antenna Height"));
        model->setItem(0,2,new QStandardItem("m"));
        model->setItem(1,2,new QStandardItem("m"));
        model->setItem(2,2,new QStandardItem("m"));
        model->setItem(3,2,new QStandardItem("m"));
    }
    else if(current->text(0) == "1029"){
        model->setItem(0,0,new QStandardItem("message"));
//        model->setItem(1,0,new QStandardItem("Y"));
//        model->setItem(2,0,new QStandardItem("Z"));
//        model->setItem(3,0,new QStandardItem("Antenna Height"));
//        model->setItem(0,2,new QStandardItem("m"));
//        model->setItem(1,2,new QStandardItem("m"));
//        model->setItem(2,2,new QStandardItem("m"));
//        model->setItem(3,2,new QStandardItem("m"));
    }
    else if(current->text(0) == "DataDisplay" || current->text(0) == "PQTMTAR"){
             ui->stackedWidget->setCurrentIndex(1);

    }
//    else if(current->text(0).split(' ')[0] == "2"){
//        ui->tableWidget->setItem(0,0,new QStandardItem("Version"));
//    }
//    else if(current->text(0).split(' ')[0] == "19"){
//        model->setHorizontalHeaderLabels(respHeader);
//        ui->tableWidget->setColumnCount(2);
//    }
//    else if(current->text(0).split(' ')[0] == "1"){
//    }
}

void Msg_Dialog::on_checkBoxHeading_clicked()
{
    if(ui->checkBoxHeading->isChecked()){
        ui->widgetHeading->graph(0)->setVisible(true);
    }else{
        ui->widgetHeading->graph(0)->setVisible(false);
    }
    ui->widgetHeading->replot(QCustomPlot::rpQueuedReplot);
}

void Msg_Dialog::on_checkBoxPitch_clicked()
{
    if(ui->checkBoxPitch->isChecked()){
        ui->widgetPitch->graph(0)->setVisible(true);
    }else{
        ui->widgetPitch->graph(0)->setVisible(false);
    }
    ui->widgetPitch->replot(QCustomPlot::rpQueuedReplot);
}

void Msg_Dialog::on_checkBoxRoll_clicked()
{
    if(ui->checkBoxRoll->isChecked()){
        ui->widgetRoll->graph(0)->setVisible(true);
    }else{
        ui->widgetRoll->graph(0)->setVisible(false);
    }
    ui->widgetRoll->replot(QCustomPlot::rpQueuedReplot);
}

void Msg_Dialog::on_pushButtonReset_clicked()
{
    yHeading.clear();
    yPitch  .clear();
    yRoll   .clear();
    xAcc    .clear();
    TimeQMap.clear();
    xUTC    .clear();
    textDataUpdateAcc();
}

void Msg_Dialog::on_pushButtonStartPause_clicked()
{
    if(!flagAcc){
//        ui->pushButtonPC->setText("Running");
        timerAcc->stop();
        flagAcc =true;
        //
        QList<QCPAxis*> axes;
        axes << ui->widgetHeading->yAxis<< ui->widgetHeading->xAxis;
        ui->widgetHeading->axisRect()->setRangeZoomAxes(axes);
        ui->widgetHeading->axisRect()->setRangeDragAxes(axes);
        ui->widgetHeading->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom /*| QCP::iSelectPlottables*/);
        ui->widgetHeading->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));
        ui->widgetHeading->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));
        ui->widgetHeading->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
        //
        axes << ui->widgetPitch->yAxis<< ui->widgetPitch->xAxis;
        ui->widgetPitch->axisRect()->setRangeZoomAxes(axes);
        ui->widgetPitch->axisRect()->setRangeDragAxes(axes);
        ui->widgetPitch->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom /*| QCP::iSelectPlottables*/);
        ui->widgetPitch->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));
        ui->widgetPitch->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));
        ui->widgetPitch->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
        //
        axes << ui->widgetRoll->yAxis<< ui->widgetRoll->xAxis;
        ui->widgetRoll->axisRect()->setRangeZoomAxes(axes);
        ui->widgetRoll->axisRect()->setRangeDragAxes(axes);
        ui->widgetRoll->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom /*| QCP::iSelectPlottables*/);
        ui->widgetRoll->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));
        ui->widgetRoll->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));
        ui->widgetRoll->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);

    }else{
//        ui->pushButtonPC->setText("Pause");
        timerAcc->start();
        flagAcc = false;
        tracerHeading->setVisible(false);
        tracerPitch  ->setVisible(false);
        tracerRoll   ->setVisible(false);
        ui->widgetHeading->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        ui->widgetPitch->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        ui->widgetRoll->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    }
}

