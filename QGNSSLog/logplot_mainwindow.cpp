/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         logPlot_MainWindow.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-10-14      Dallas.xu
* ***************************************************************************/
#include "logplot_mainwindow.h"
#include "ui_logplot_mainwindow.h"

logPlot_MainWindow::logPlot_MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::logPlot_MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(LOG_VERSION);
    QNMEA_NEW ();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);
    QFont font = ui->customPlot->font();
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("Arial");
    /*Time
    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("hh:mm:ss.zzz \n yyyy-MM-dd");
    ui->customPlot->xAxis->setTicker(timeTicker);*/
    ui->customPlot->xAxis->setLabelFont(font);
    ui->customPlot->yAxis->setLabelFont(font);
    ui->customPlot->yAxis2->setLabelFont(font);//-----
//    ui->customPlot->xAxis->setLabel(QStringLiteral("Indexes"));
    ui->customPlot->yAxis->setLabel(QStringLiteral("C/No(dB-Hz)"));
    ui->customPlot->yAxis2->setLabel(QStringLiteral("Status(0=No Fix/1=Fix)"));//---
    ui->customPlot->yAxis2->setVisible(true);//----
    ui->customPlot->setNoAntialiasingOnDrag(true);
    ui->customPlot->setAntialiasedElement(QCP::AntialiasedElement::aeAll,false);
    setPlotColorList();
    connect(ui->customPlot, SIGNAL(mouseRelease()), this, SLOT(mouseReleaseSlots()));
    connect(this,&logPlot_MainWindow::logDataChange,this,&logPlot_MainWindow::getLogData,Qt::ConnectionType::AutoConnection);
    connect(this,&logPlot_MainWindow::logStatusChange,this,&logPlot_MainWindow::getLogStatus,Qt::ConnectionType::AutoConnection);
    pAllQwait = new QWaiting(this);
    setAcceptDrops(true);
    connect(ui->customPlot,SIGNAL(mouseMove(QMouseEvent *)),this,SLOT(myMoveMouseEvent(QMouseEvent *)));
    tracer = new QCPItemTracer(ui->customPlot);
    tracer->setInterpolating(false);
    tracer->setStyle(QCPItemTracer::tsCircle);
    tracer->setPen(QPen(Qt::red));
    tracer->setBrush(Qt::red);
    tracer->setSize(6);
    pMenu = new QMenu(this);
    act_Clear = new QAction(tr("Clear"), this);
    act_Reset = new QAction(tr("Refresh"), this);
    act_Datacomparison = new QAction(tr("Comparison"), this);
    act_Datacomparison->setCheckable(true);
    act_Datacomparison->setChecked(true);
    FilterOptions = new  FilterOptionsDialog(this);
}

void logPlot_MainWindow::showPlot(){
    if(!x_dataList.isEmpty()){
        for(int i=0; i<plotColor.count(); ++i){
            ui->customPlot->graph(i)->data().data()->clear();
        }
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    }
    y_dataList.clear();
    x_dataList.clear();
    x_VU.clear();
    y_VU.clear();
    PositionState_x.clear();
    PositionState_y.clear();
    all_x.clear();
    AlarmValue_y.clear();
    mmaCNoList.clear();
    QFont font = ui->customPlot->font();
    font.setPointSize(12);
    font.setBold(true);
    ui->customPlot->xAxis->setLabelFont(font);
    ui->customPlot->yAxis->setLabelFont(font);
    getMaxMinAVG();
    showPlotData();
    pAllQwait->close();
    plotData();
    checkboxTickControl();
}

void logPlot_MainWindow::keyReleaseEvent(QKeyEvent *event){
    if(event->key() ==Qt::Key_F5){
        if(!x_dataList.isEmpty()){
            ui->customPlot->rescaleAxes(true);
            ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
        }
    }
    if(event->modifiers()& Qt::ControlModifier  && event->key() == Qt::Key_C ){
        OnCopyAction();
    }
    if(event->modifiers()& Qt::ControlModifier  && event->key() == Qt::Key_Delete){
        OnClearAction();
    }
}

void logPlot_MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }else {
        event->ignore();
    }
}

void logPlot_MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();
        QString filePath = urls.at(0).toLocalFile();
        QTextCodec *code = QTextCodec::codecForName("GB2312");
        QString strUnicode= code->toUnicode(filePath.toLocal8Bit().data());
        Read_log(strUnicode.toStdString().c_str());
        QFileInfo fileInfo(strUnicode);
        if(fileInfo.baseName()!=""){
            fileName_save = fileInfo.baseName();
            QString tt = " ("+fileInfo.fileName()+")";
            QString nameFile = LOG_VERSION+tt;
            this->setWindowTitle(nameFile);
        }
    }else{
        OnClearAction();
        QMessageBox::information(this,"LOG file open error","File open error, please open the correct log file!");
        tracer->setVisible(false);
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void logPlot_MainWindow::myMoveMouseEvent(QMouseEvent *event)
{
    if(!x_dataList.isEmpty()&&act_Datacomparison->isChecked()){
        tracer->setVisible(true);
        QCPGraph *mGraph = ui->customPlot->graph(0);
        tracer->setPen(QPen(Qt::DashLine));
        tracer->setStyle(QCPItemTracer::tsCrosshair);
        double x = ui->customPlot->xAxis->pixelToCoord(event->pos().x());
        mGraph = ui->customPlot->graph(58);
        tracer->setGraph(mGraph);
        tracer->setGraphKey(x);
        double traceX = tracer->position->key();
        QString temps=QString("<tr><%1</tr> ").arg("");
        int countGraph = ui->customPlot->graphCount();
        if(ui->radioButton_CNo->isChecked()){
            countGraph = ui->customPlot->graphCount()-1;
        }
        for (int i = 0; i < countGraph; ++i)
        {
            if(ui->customPlot->graph(i)->data().data()->size()>0){
                if(((traceX >= ui->customPlot->graph(i)->data().data()->begin()->key) && (traceX <= x_dataList[i][x_dataList[i].count()-1]))
                        && ui->customPlot->graph(i)->visible())
                {
                    QCPDataContainer<QCPGraphData>::const_iterator coorPoint = ui->customPlot->graph(i)->data().data()->findBegin(traceX,false);
                        temps.append( QString(" <tr><td><font color=%1>%2</font></td><td><font color=%1>,</font></td><td><font color=%1>X:%3 </font></td><td><font color=%1>,</font></td><td><font color=%1>UTC:%4 </font></td><td><font color=%1>,</font></td><td><font color=%1>Y:%5</font></td></tr>")
                                      .arg(plotColor[i].name())
                                      .arg(ui->customPlot->graph(i)->name())
                                      .arg(QString::number(traceX))
                                      .arg(QDateTime::fromMSecsSinceEpoch(TimeQMap.value(traceX) * 1000.0).toString("hh:mm:ss.zzz"))
                                      .arg(QString::number(coorPoint->value,'f',2)));
                }
            }
        }
        if(!temps.isEmpty()){
            QToolTip::showText(event->globalPos(), QString("<h4>%1</h4> <table>%2</table> ").arg("Comparison").arg(temps), this, this->rect());
        }
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    }else{
        tracer->setVisible(false);
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void logPlot_MainWindow::getLogData(QLOG_Data *logdata)
{
    if(logdata->sysdatas.size()==0){
        this->setWindowTitle(LOG_VERSION);
        OnClearAction();
        QMessageBox::information(this,"LOG file open error","File open error, please open the correct log file!");
        pAllQwait->close();

    }else{
        PositionStateMap.clear();
        TimeQMap.clear();
        frameDataList.clear();
        frameDataListALL.clear();
        memset( (void *)&frameData, 0, sizeof(frameData));
        for(int i=0; i<58; ++i){
            frameDataListALL.append(frameDataList);
        }
        for(int i=0;i< logdata->sysdatas.size();++i){
            for(int j=0;j<logdata->sysdatas[i]->signaldatas.size ();++j){
                for(int idx=0;idx<logdata->sysdatas[i]->signaldatas[j]->plotdatas.size ();++idx){
                    if((int)logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->date.yy>0){
                        frameData.dUTC        = getXDataTime(logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->date
                                                             ,logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->UTC);
                        frameData.CN0_Average = (double)logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->data->CN0_Average;
                        frameData.CN0_Max     = (int)logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->data->CN0_Max;
                        frameData.CN0_Min     = (int)logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->data->CN0_Min;
                        frameData.TotalNumSatView = (unsigned short)logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->data->TotalNumSatView;
//                        qDebug()<<"dayinyinxi : "<<frameData.TotalNumSatView;
                        frameData.TotalNumSatUsed = (unsigned short)logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->data->TotalNumSatUsed;
                        frameData.serialNumber = (int)logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->RMC_Index+1;
                        frameData.PositioningState = (int)logdata->sysdatas[i]->signaldatas[j]->plotdatas[idx]->RMC_Status!=1? 1:0;
                        PositionStateMap.insert((double)frameData.serialNumber,(double)frameData.PositioningState);
                        TimeQMap.insert((double)frameData.serialNumber,frameData.dUTC);
                        frameDataList.append(frameData);
                        memset( (void *)&frameData, 0, sizeof(frameData));
                    }
                }
                if((int)logdata->sysdatas[i]->sysid==1){
                    if(logdata->sysdatas[i]->signaldatas[j]->signalid=='0'){
                        frameDataListALL[0] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='1'){
                        frameDataListALL[1] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='2'){
                        frameDataListALL[2] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='3'){
                        frameDataListALL[3] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='4'){
                        frameDataListALL[4] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='5'){
                        frameDataListALL[5] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='6'){
                        frameDataListALL[6] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='7'){
                        frameDataListALL[7] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='8'){
                        frameDataListALL[8] = frameDataList;
                    }
                }else if((int)logdata->sysdatas[i]->sysid==2){
                    if(logdata->sysdatas[i]->signaldatas[j]->signalid=='0'){
                        frameDataListALL[9] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='1'){
                        frameDataListALL[10] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='2'){
                        frameDataListALL[11] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='3'){
                        frameDataListALL[12] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='4'){
                        frameDataListALL[13] = frameDataList;
                    }
                }else if((int)logdata->sysdatas[i]->sysid==3){
                    if(logdata->sysdatas[i]->signaldatas[j]->signalid=='0'){
                        frameDataListALL[14] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='1'){
                        frameDataListALL[15] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='2'){
                        frameDataListALL[16] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='3'){
                        frameDataListALL[17] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='4'){
                        frameDataListALL[18] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='5'){
                        frameDataListALL[19] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='6'){
                        frameDataListALL[20] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='7'){
                        frameDataListALL[21] = frameDataList;
                    }
                }else if((int)logdata->sysdatas[i]->sysid==4){
                    if(logdata->sysdatas[i]->signaldatas[j]->signalid=='0'){
                        frameDataListALL[22] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='1'){
                        frameDataListALL[23] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='2'){
                        frameDataListALL[24] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='3'){
                        frameDataListALL[25] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='4'){
                        frameDataListALL[26] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='5'){
                        frameDataListALL[27] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='6'){
                        frameDataListALL[28] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='7'){
                        frameDataListALL[29] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='8'){
                        frameDataListALL[30] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='9'){
                        frameDataListALL[31] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid == 'A'){
                        frameDataListALL[32] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid == 'B'){
                        frameDataListALL[33] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid == 'C'){
                        frameDataListALL[34] = frameDataList;
                    }
                }else if((int)logdata->sysdatas[i]->sysid==5){
                    if(logdata->sysdatas[i]->signaldatas[j]->signalid=='0'){
                        frameDataListALL[35] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='1'){
                        frameDataListALL[36] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='2'){
                        frameDataListALL[37] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='3'){
                        frameDataListALL[38] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='4'){
                        frameDataListALL[39] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='5'){
                        frameDataListALL[40] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='6'){
                        frameDataListALL[41] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='7'){
                        frameDataListALL[42] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='8'){
                        frameDataListALL[43] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='9'){
                        frameDataListALL[44] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid == 'A'){
                        frameDataListALL[45] = frameDataList;
                    }
                }else if((int)logdata->sysdatas[i]->sysid==6){
                    if(logdata->sysdatas[i]->signaldatas[j]->signalid=='0'){
                        frameDataListALL[46] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='1'){
                        frameDataListALL[47] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='2'){
                        frameDataListALL[48] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='3'){
                        frameDataListALL[49] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='4'){
                        frameDataListALL[50] = frameDataList;
                    }else if(logdata->sysdatas[i]->signaldatas[j]->signalid=='5'){
                        frameDataListALL[51] = frameDataList;
                    }
                }else if((int)logdata->sysdatas[i]->sysid==7){
                    frameDataListALL[52].append(frameDataList);
                }else if((int)logdata->sysdatas[i]->sysid==8){
                    frameDataListALL[53].append(frameDataList);
                }else if((int)logdata->sysdatas[i]->sysid==9){
                    frameDataListALL[54].append(frameDataList);
                }else if((int)logdata->sysdatas[i]->sysid==10){
                    frameDataListALL[55].append(frameDataList);
                }else if((int)logdata->sysdatas[i]->sysid==11){
                    frameDataListALL[56].append(frameDataList);
                }else if((int)logdata->sysdatas[i]->sysid==12){
                    frameDataListALL[57].append(frameDataList);
                }
                frameDataList.clear();
            }
        }
        showPlot();
    }
}

void logPlot_MainWindow::getLogStatus(async_read_status *status){
    if(status->progress>0){
        pAllQwait->show();
    }
}

void logPlot_MainWindow::get_log_data_cb(QLOG_Data *logdata,void * userd)
{
    logPlot_MainWindow *lm=(logPlot_MainWindow*)userd;
    if(!lm){
        qDebug()<<"Pointer conversion failure--Log Data";
    }else{
        emit  lm->logDataChange(logdata);
    }
}

void logPlot_MainWindow::get_log_status_cb(async_read_status *status, void *userdata)
{
    logPlot_MainWindow *lm=(logPlot_MainWindow*)userdata;
    if(!lm){
        qDebug()<<"Pointer conversion failure--Log Status";
    }else{
        emit  lm->logStatusChange(status);
    }
}

void logPlot_MainWindow::buttonClicked(QAbstractButton *butClicked)
{
    if(butClicked == (QAbstractButton*)disagreeBut){
        firstOpenHelpWrite();
    }else if(butClicked == (QAbstractButton*)agreeBut){
        infoDialog *pAbout = new infoDialog(this);
        pAbout->show();
        firstOpenHelpWrite();
    }
}

int logPlot_MainWindow::firstOpenHelpRead()
{
    int gFlag;
    QSettings setting("./config/NoviceBootConfiguration.ini", QSettings::IniFormat);
    gFlag = setting.value("gFlag").toInt();
    return gFlag;
}

QString logPlot_MainWindow::firstOpenHelpReadTwo()
{
    QString gFlag;
    QSettings setting("./config/NoviceBootConfiguration.ini", QSettings::IniFormat);
    gFlag.append(setting.value("machineName").toByteArray());
    return gFlag;
}

void logPlot_MainWindow::firstOpenHelpWrite()
{
    QString gFlag= "0";
    QString machineName  = QHostInfo::localHostName();
    QSettings setting("./config/NoviceBootConfiguration.ini", QSettings::IniFormat);
    setting.setValue("gFlag", gFlag);
    setting.setValue("machineName", machineName);
}

double logPlot_MainWindow::getXDataTime(NMEA_Base::NMEA_Data tDate, NMEA_Base::NMEA_Time dUTC){
    int y = (int)tDate.yy+ 2000;
    int M = (int)tDate.mm;
    int d = (int)tDate.dd;
    QDateTime time1 = QDateTime(QDate(1970,1,1));
    QDateTime time2 = QDateTime(QDate(y,M,d));
    qint64 diffs = time1.secsTo(time2);
    double x = dUTC.hh*3600+dUTC.mm*60+dUTC.ss+(double)dUTC.sss/1000;
    double dateTime_t = diffs + x - 28800;
    return dateTime_t;
}

logPlot_MainWindow::~logPlot_MainWindow(){
    delete ui;
    QNMEA_DELETE ();
}

void logPlot_MainWindow::showTime(){
    QDateTime dateTime;
    dateTime = QDateTime::currentDateTime();
    ui->label_Time->setText(dateTime.toString("MM/dd/yyyy hh:mm:ss"));
}

void logPlot_MainWindow::mouseReleaseSlots()
{
    act_Clear->setShortcut(Qt::ControlModifier+Qt::Key_Delete);
    act_Reset->setShortcut(Qt::Key_F5);
    pMenu->addAction(act_Clear);
    pMenu->addAction(act_Reset);
    pMenu->addAction(act_Datacomparison);
    connect(act_Clear, SIGNAL(triggered()), this, SLOT(OnClearAction()));
    connect(act_Reset, SIGNAL(triggered()), this, SLOT(OnResetAction()));
    connect(act_Datacomparison, SIGNAL(triggered()), this, SLOT(OnDatarComparisonAction()));
    pMenu->exec(cursor().pos());
}

void logPlot_MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton){
        QMenu *pMenu1 = new QMenu(this);
        QAction *act_Copy = new QAction(tr("Perfect"), this);
        act_Copy->setShortcut(Qt::ControlModifier+Qt::Key_C);
        pMenu1->addAction(act_Copy);
        connect(act_Copy, SIGNAL(triggered()), this, SLOT(OnCopyAction()));
        pMenu1->exec(cursor().pos());
        QList<QAction*> list = pMenu1->actions();
        foreach (QAction* pAction, list) delete pAction;
        delete pMenu1;
    }
}

void logPlot_MainWindow::wheelEvent(QWheelEvent *event)
{
}

void logPlot_MainWindow::OnDatarComparisonAction()
{
    if(act_Datacomparison->isChecked()){
        act_Datacomparison->setCheckable(true);
        act_Datacomparison->setChecked(true);
    }else{
        act_Datacomparison->setCheckable(true);
        act_Datacomparison->setChecked(false);
    }
}

void logPlot_MainWindow::OnClearAction(){
    if(!x_dataList.isEmpty()){
        this->setWindowTitle(LOG_VERSION);
        for(int i=0; i<plotColor.count(); ++i){
            ui->customPlot->graph(i)->data().data()->clear();
        }
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
        checkboxTickControl();
        y_dataList.clear();
        x_dataList.clear();
        x_VU.clear();
        y_VU.clear();
        PositionState_x.clear();
        PositionState_y.clear();
        all_x.clear();
        AlarmValue_y.clear();
        mmaCNoList.clear();
    }
}

void logPlot_MainWindow::OnResetAction(){
    if(!x_dataList.isEmpty()){
        ui->customPlot->rescaleAxes(true);
    }
}

QString logPlot_MainWindow::getCopyText(QCheckBox *checkBox, QLabel *signalChannel, QLabel *avgCNo, QLabel *maxCNo, QLabel *minCNo){
    QString copyText="";
    if(minCNo->text()!="-1"&&signalChannel->text()!=""&&checkBox->isChecked()){
        copyText = signalChannel->text()+":"+avgCNo->text()+" "+maxCNo->text()+" "+minCNo->text()+"\n";
    }
    return  copyText;
}

void logPlot_MainWindow::OnCopyAction(){
    if(!x_dataList.isEmpty()){
        QClipboard *clipboard = QApplication::clipboard();
        QString copyText;

        copyText += getCopyText(ui->checkBox_GPALL  ,ui->label_CNo_0  , ui->label_AVG_0 , ui->label_MAX_0 , ui->label_MIN_0 );
        copyText += getCopyText(ui->checkBox_GPSL1CA,ui->label_CNo_1  , ui->label_AVG_1 , ui->label_MAX_1 , ui->label_MIN_1 );
        copyText += getCopyText(ui->checkBox_GPSL1PY,ui->label_CNo_2  , ui->label_AVG_2 , ui->label_MAX_2 , ui->label_MIN_2 );
        copyText += getCopyText(ui->checkBox_GPSL1M ,ui->label_CNo_3  , ui->label_AVG_3 , ui->label_MAX_3 , ui->label_MIN_3 );
        copyText += getCopyText(ui->checkBox_GPSL2PY,ui->label_CNo_4  , ui->label_AVG_4 , ui->label_MAX_4 , ui->label_MIN_4 );
        copyText += getCopyText(ui->checkBox_GPSL2CM,ui->label_CNo_5  , ui->label_AVG_5 , ui->label_MAX_5 , ui->label_MIN_5 );
        copyText += getCopyText(ui->checkBox_GPSL2CL,ui->label_CNo_6  , ui->label_AVG_6 , ui->label_MAX_6 , ui->label_MIN_6 );
        copyText += getCopyText(ui->checkBox_GPSL5I ,ui->label_CNo_7  , ui->label_AVG_7 , ui->label_MAX_7 , ui->label_MIN_7 );
        copyText += getCopyText(ui->checkBox_GPSL5Q ,ui->label_CNo_8  , ui->label_AVG_8 , ui->label_MAX_8 , ui->label_MIN_8 );
        copyText += getCopyText(ui->checkBox_GLALL  ,ui->label_CNo_9  , ui->label_AVG_9 , ui->label_MAX_9 , ui->label_MIN_9 );
        copyText += getCopyText(ui->checkBox_GLG1CA ,ui->label_CNo_10 , ui->label_AVG_10, ui->label_MAX_10, ui->label_MIN_10);
        copyText += getCopyText(ui->checkBox_GLG1P  ,ui->label_CNo_11 , ui->label_AVG_11, ui->label_MAX_11, ui->label_MIN_11);
        copyText += getCopyText(ui->checkBox_GLG2CA ,ui->label_CNo_12 , ui->label_AVG_12, ui->label_MAX_12, ui->label_MIN_12);
        copyText += getCopyText(ui->checkBox_GLG2P  ,ui->label_CNo_13 , ui->label_AVG_13, ui->label_MAX_13, ui->label_MIN_13);
        copyText += getCopyText(ui->checkBox_GAALL  ,ui->label_CNo_14 , ui->label_AVG_14, ui->label_MAX_14, ui->label_MIN_14);
        copyText += getCopyText(ui->checkBox_GaE5a  ,ui->label_CNo_15 , ui->label_AVG_15, ui->label_MAX_15, ui->label_MIN_15);
        copyText += getCopyText(ui->checkBox_GaE5b  ,ui->label_CNo_16 , ui->label_AVG_16, ui->label_MAX_16, ui->label_MIN_16);
        copyText += getCopyText(ui->checkBox_GaE5ab ,ui->label_CNo_17 , ui->label_AVG_17, ui->label_MAX_17, ui->label_MIN_17);
        copyText += getCopyText(ui->checkBox_GaE6A  ,ui->label_CNo_18 , ui->label_AVG_18, ui->label_MAX_18, ui->label_MIN_18);
        copyText += getCopyText(ui->checkBox_GaE6BC ,ui->label_CNo_19 , ui->label_AVG_19, ui->label_MAX_19, ui->label_MIN_19);
        copyText += getCopyText(ui->checkBox_GaL1A  ,ui->label_CNo_20 , ui->label_AVG_20, ui->label_MAX_20, ui->label_MIN_20);
        copyText += getCopyText(ui->checkBox_GaL1BC ,ui->label_CNo_21 , ui->label_AVG_21, ui->label_MAX_21, ui->label_MIN_21);
        copyText += getCopyText(ui->checkBox_GBALL  ,ui->label_CNo_22 , ui->label_AVG_22, ui->label_MAX_22, ui->label_MIN_22);
        copyText += getCopyText(ui->checkBox_BDSB1I ,ui->label_CNo_23 , ui->label_AVG_23, ui->label_MAX_23, ui->label_MIN_23);
        copyText += getCopyText(ui->checkBox_BDSB1Q ,ui->label_CNo_24 , ui->label_AVG_24, ui->label_MAX_24, ui->label_MIN_24);
        copyText += getCopyText(ui->checkBox_BDSB1C ,ui->label_CNo_25 , ui->label_AVG_25, ui->label_MAX_25, ui->label_MIN_25);
        copyText += getCopyText(ui->checkBox_BDSB1A ,ui->label_CNo_26 , ui->label_AVG_26, ui->label_MAX_26, ui->label_MIN_26);
        copyText += getCopyText(ui->checkBox_BDSB2a ,ui->label_CNo_27 , ui->label_AVG_27, ui->label_MAX_27, ui->label_MIN_27);
        copyText += getCopyText(ui->checkBox_BDSB2b ,ui->label_CNo_28 , ui->label_AVG_28, ui->label_MAX_28, ui->label_MIN_28);
        copyText += getCopyText(ui->checkBox_BDSB2ab,ui->label_CNo_29 , ui->label_AVG_29, ui->label_MAX_29, ui->label_MIN_29);
        copyText += getCopyText(ui->checkBox_BDSB3I ,ui->label_CNo_30 , ui->label_AVG_30, ui->label_MAX_30, ui->label_MIN_30);
        copyText += getCopyText(ui->checkBox_BDSB3Q ,ui->label_CNo_31 , ui->label_AVG_31, ui->label_MAX_31, ui->label_MIN_31);
        copyText += getCopyText(ui->checkBox_BDSB3A ,ui->label_CNo_32 , ui->label_AVG_32, ui->label_MAX_32, ui->label_MIN_32);
        copyText += getCopyText(ui->checkBox_BDSB2I ,ui->label_CNo_33 , ui->label_AVG_33, ui->label_MAX_33, ui->label_MIN_33);
        copyText += getCopyText(ui->checkBox_BDSB2Q ,ui->label_CNo_34 , ui->label_AVG_34, ui->label_MAX_34, ui->label_MIN_34);
        copyText += getCopyText(ui->checkBox_GQALL  ,ui->label_CNo_35 , ui->label_AVG_35, ui->label_MAX_35, ui->label_MIN_35);
        copyText += getCopyText(ui->checkBox_GQL1CA ,ui->label_CNo_36 , ui->label_AVG_36, ui->label_MAX_36, ui->label_MIN_36);
        copyText += getCopyText(ui->checkBox_GQL1CD ,ui->label_CNo_37 , ui->label_AVG_37, ui->label_MAX_37, ui->label_MIN_37);
        copyText += getCopyText(ui->checkBox_GQL1CP ,ui->label_CNo_38 , ui->label_AVG_38, ui->label_MAX_38, ui->label_MIN_38);
        copyText += getCopyText(ui->checkBox_GQLIS  ,ui->label_CNo_39 , ui->label_AVG_39, ui->label_MAX_39, ui->label_MIN_39);
        copyText += getCopyText(ui->checkBox_GQL2CM ,ui->label_CNo_40 , ui->label_AVG_40, ui->label_MAX_40, ui->label_MIN_40);
        copyText += getCopyText(ui->checkBox_GQL2CL ,ui->label_CNo_41 , ui->label_AVG_41, ui->label_MAX_41, ui->label_MIN_41);
        copyText += getCopyText(ui->checkBox_GQL5I  ,ui->label_CNo_42 , ui->label_AVG_42, ui->label_MAX_42, ui->label_MIN_42);
        copyText += getCopyText(ui->checkBox_GQL5Q  ,ui->label_CNo_43 , ui->label_AVG_43, ui->label_MAX_43, ui->label_MIN_43);
        copyText += getCopyText(ui->checkBox_GQL6D  ,ui->label_CNo_44 , ui->label_AVG_44, ui->label_MAX_44, ui->label_MIN_44);
        copyText += getCopyText(ui->checkBox_GQL6E  ,ui->label_CNo_45 , ui->label_AVG_45, ui->label_MAX_45, ui->label_MIN_45);
        copyText += getCopyText(ui->checkBox_GIALL  ,ui->label_CNo_46 , ui->label_AVG_46, ui->label_MAX_46, ui->label_MIN_46);
        copyText += getCopyText(ui->checkBox_GIL5SPS,ui->label_CNo_47 , ui->label_AVG_47, ui->label_MAX_47, ui->label_MIN_47);
        copyText += getCopyText(ui->checkBox_GISSPS ,ui->label_CNo_48 , ui->label_AVG_48, ui->label_MAX_48, ui->label_MIN_48);
        copyText += getCopyText(ui->checkBox_GIL5RS ,ui->label_CNo_49 , ui->label_AVG_49, ui->label_MAX_49, ui->label_MIN_49);
        copyText += getCopyText(ui->checkBox_GISRS  ,ui->label_CNo_50 , ui->label_AVG_50, ui->label_MAX_50, ui->label_MIN_50);
        copyText += getCopyText(ui->checkBox_GIL1SPS,ui->label_CNo_51 , ui->label_AVG_51, ui->label_MAX_51, ui->label_MIN_51);
        copyText += getCopyText(ui->checkBox_WAAS   ,ui->label_CNo_52 , ui->label_AVG_52, ui->label_MAX_52, ui->label_MIN_52);
        copyText += getCopyText(ui->checkBox_SDCM   ,ui->label_CNo_53 , ui->label_AVG_53, ui->label_MAX_53, ui->label_MIN_53);
        copyText += getCopyText(ui->checkBox_EGNOS  ,ui->label_CNo_54 , ui->label_AVG_54, ui->label_MAX_54, ui->label_MIN_54);
        copyText += getCopyText(ui->checkBox_BDSBAS ,ui->label_CNo_55 , ui->label_AVG_55, ui->label_MAX_55, ui->label_MIN_55);
        copyText += getCopyText(ui->checkBox_MSAS   ,ui->label_CNo_56 , ui->label_AVG_56, ui->label_MAX_56, ui->label_MIN_56);
        copyText += getCopyText(ui->checkBox_GAGAN  ,ui->label_CNo_57 , ui->label_AVG_57, ui->label_MAX_57, ui->label_MIN_57);
        clipboard->setText(copyText);
    }
}

void logPlot_MainWindow::getCNoAVGMaxMin(QCheckBox *checkBox, int id, QLabel *signalChannel, QString gpsID, QLabel *avgCNo, QLabel *maxCNo, QLabel *minCNo){
    signalChannel->setText(gpsID);
    if(mmaCNoList[id].nCount!=0)
    {
        avgCNo->setText(mmaCNoList[id].dAverage>=0?QString::number(mmaCNoList[id].dAverage/mmaCNoList[id].nCount,'f',2):"-");
    }else{
        avgCNo->setText(QString::number(mmaCNoList[id].dAverage,'f',2));
        checkBox->setCheckState(Qt::Unchecked);
    }
    maxCNo->setText(mmaCNoList[id].dAverage>=0?QString::number(mmaCNoList[id].nMAX):"-");
    if(mmaCNoList[id].dAverage!=0){
        minCNo->setText(mmaCNoList[id].dAverage>0?QString::number(mmaCNoList[id].nMIN):"-");
    }else{
        minCNo->setText(QString::number(0));
    }
}

void logPlot_MainWindow::getMaxMinAVG(){
    for (int i=0; i<frameDataListALL.count(); ++i) {
        if(frameDataListALL[i].count()==0)
        {
            m_CNoMMA.nCount   = 0;
            m_CNoMMA.dAverage = 0;
            m_CNoMMA.nMAX     = 0;
            m_CNoMMA.nMIN     = 0;
        }else{
            for (int j=0; j<frameDataListALL[i].count(); ++j)
            {
                if(j==0){
                    if(ui->radioButton_CNo->isChecked()){
                        if((int)frameDataListALL[i][j].CN0_Average!=-1){
                            m_CNoMMA.nCount   = 1;
                            m_CNoMMA.dAverage = frameDataListALL[i][j].CN0_Average;
                        }else{
                            m_CNoMMA.nCount   = 0;
                            m_CNoMMA.dAverage = 0;
                        }
                        m_CNoMMA.nMAX     = frameDataListALL[i][j].CN0_Max;
                        m_CNoMMA.nMIN     = frameDataListALL[i][j].CN0_Min;

                    }else if(ui->radioButton_View->isChecked()){
                        if((frameDataListALL[i][j].TotalNumSatView>0)){
                            m_CNoMMA.nCount   = 1;
                            m_CNoMMA.dAverage = frameDataListALL[i][j].TotalNumSatView;
                        }else {
                            m_CNoMMA.nCount   = 0;
                            m_CNoMMA.dAverage = 0;
                        }
                        m_CNoMMA.nMAX     = frameDataListALL[i][j].TotalNumSatView;
                        m_CNoMMA.nMIN     = frameDataListALL[i][j].TotalNumSatView;
                    }else if(ui->radioButton_Used->isChecked()){
                        if((frameDataListALL[i][j].TotalNumSatUsed>0)){
                            m_CNoMMA.nCount   = 1;
                            m_CNoMMA.dAverage = frameDataListALL[i][j].TotalNumSatUsed;
                        }else {
                            m_CNoMMA.nCount   = 0;
                            m_CNoMMA.dAverage = 0;
                        }
                        m_CNoMMA.nMAX     = frameDataListALL[i][j].TotalNumSatUsed;
                        m_CNoMMA.nMIN     = frameDataListALL[i][j].TotalNumSatUsed;
                    }
                }else{
                    if((int)frameDataListALL[i][j].CN0_Average!=-1){
                        
                        if(ui->radioButton_CNo->isChecked()){
                            m_CNoMMA.nCount++;
                            m_CNoMMA.dAverage += frameDataListALL[i][j].CN0_Average;
                            m_CNoMMA.nMAX      = m_CNoMMA.nMAX > frameDataListALL[i][j].CN0_Max ? m_CNoMMA.nMAX : frameDataListALL[i][j].CN0_Max;
                            int min = frameDataListALL[i][j].CN0_Min!=-1?frameDataListALL[i][j].CN0_Min:100;
                            m_CNoMMA.nMIN = m_CNoMMA.nMIN!=-1?m_CNoMMA.nMIN:100;
                            m_CNoMMA.nMIN      = m_CNoMMA.nMIN < min ? m_CNoMMA.nMIN : min;
                        }else if(ui->radioButton_View->isChecked()){
                            if(frameDataListALL[i][j].TotalNumSatView>0){
                                m_CNoMMA.nCount++;
                            }
                            m_CNoMMA.dAverage += frameDataListALL[i][j].TotalNumSatView;
                            m_CNoMMA.nMAX      = m_CNoMMA.nMAX > frameDataListALL[i][j].TotalNumSatView ? m_CNoMMA.nMAX : frameDataListALL[i][j].TotalNumSatView;
                            int min = frameDataListALL[i][j].TotalNumSatView!=0?frameDataListALL[i][j].TotalNumSatView:100;
                            m_CNoMMA.nMIN = m_CNoMMA.nMIN!=0?m_CNoMMA.nMIN:100;
                            m_CNoMMA.nMIN      = m_CNoMMA.nMIN < min ? m_CNoMMA.nMIN : min;
                        }else if(ui->radioButton_Used->isChecked()){
                            if(frameDataListALL[i][j].TotalNumSatUsed>0){
                                m_CNoMMA.nCount++;
                            }
                            m_CNoMMA.dAverage += frameDataListALL[i][j].TotalNumSatUsed;
                            m_CNoMMA.nMAX      = m_CNoMMA.nMAX > frameDataListALL[i][j].TotalNumSatUsed ? m_CNoMMA.nMAX : frameDataListALL[i][j].TotalNumSatUsed;
                            int min = frameDataListALL[i][j].TotalNumSatUsed!=0?frameDataListALL[i][j].TotalNumSatUsed:100;
                            m_CNoMMA.nMIN = m_CNoMMA.nMIN!=0?m_CNoMMA.nMIN:100;
                            m_CNoMMA.nMIN      = m_CNoMMA.nMIN < min ? m_CNoMMA.nMIN : min;
                        }
                    }
                }
            }
        }
        mmaCNoList.append(m_CNoMMA);
        memset( (void *)&m_CNoMMA, 0, sizeof(m_CNoMMA));
    }
    getCNoAVGMaxMin(ui->checkBox_GPALL  , 0 , ui->label_CNo_0  , "GPS:ALL    ", ui->label_AVG_0 , ui->label_MAX_0 , ui->label_MIN_0 );
    getCNoAVGMaxMin(ui->checkBox_GPSL1CA, 1 , ui->label_CNo_1  , "GPS:L1/A   ", ui->label_AVG_1 , ui->label_MAX_1 , ui->label_MIN_1 );
    getCNoAVGMaxMin(ui->checkBox_GPSL1PY, 2 , ui->label_CNo_2  , "GPS:L1P(Y) ", ui->label_AVG_2 , ui->label_MAX_2 , ui->label_MIN_2 );
    getCNoAVGMaxMin(ui->checkBox_GPSL1M , 3 , ui->label_CNo_3  , "GPS:L1M    ", ui->label_AVG_3 , ui->label_MAX_3 , ui->label_MIN_3 );
    getCNoAVGMaxMin(ui->checkBox_GPSL2PY, 4 , ui->label_CNo_4  , "GPS:L2P(Y) ", ui->label_AVG_4 , ui->label_MAX_4 , ui->label_MIN_4 );
    getCNoAVGMaxMin(ui->checkBox_GPSL2CM, 5 , ui->label_CNo_5  , "GPS:L2C-M  ", ui->label_AVG_5 , ui->label_MAX_5 , ui->label_MIN_5 );
    getCNoAVGMaxMin(ui->checkBox_GPSL2CL, 6 , ui->label_CNo_6  , "GPS:L2C-L  ", ui->label_AVG_6 , ui->label_MAX_6 , ui->label_MIN_6 );
    getCNoAVGMaxMin(ui->checkBox_GPSL5I , 7 , ui->label_CNo_7  , "GPS:L5-I   ", ui->label_AVG_7 , ui->label_MAX_7 , ui->label_MIN_7 );
    getCNoAVGMaxMin(ui->checkBox_GPSL5Q , 8 , ui->label_CNo_8  , "GPS:L5-Q   ", ui->label_AVG_8 , ui->label_MAX_8 , ui->label_MIN_8 );
    getCNoAVGMaxMin(ui->checkBox_GLALL  , 9 , ui->label_CNo_9  , "GLO:ALL    ", ui->label_AVG_9 , ui->label_MAX_9 , ui->label_MIN_9 );
    getCNoAVGMaxMin(ui->checkBox_GLG1CA , 10, ui->label_CNo_10 , "GLO:G1C/A  ", ui->label_AVG_10, ui->label_MAX_10, ui->label_MIN_10);
    getCNoAVGMaxMin(ui->checkBox_GLG1P  , 11, ui->label_CNo_11 , "GLO:G1P    ", ui->label_AVG_11, ui->label_MAX_11, ui->label_MIN_11);
    getCNoAVGMaxMin(ui->checkBox_GLG2CA , 12, ui->label_CNo_12 , "GLO:G2C/A  ", ui->label_AVG_12, ui->label_MAX_12, ui->label_MIN_12);
    getCNoAVGMaxMin(ui->checkBox_GLG2P  , 13, ui->label_CNo_13 , "GLO:G2P    ", ui->label_AVG_13, ui->label_MAX_13, ui->label_MIN_13);
    getCNoAVGMaxMin(ui->checkBox_GAALL  , 14, ui->label_CNo_14 , "GAL:ALL    ", ui->label_AVG_14, ui->label_MAX_14, ui->label_MIN_14);
    getCNoAVGMaxMin(ui->checkBox_GaE5a  , 15, ui->label_CNo_15 , "GAL:E5a    ", ui->label_AVG_15, ui->label_MAX_15, ui->label_MIN_15);
    getCNoAVGMaxMin(ui->checkBox_GaE5b  , 16, ui->label_CNo_16 , "GAL:E5b    ", ui->label_AVG_16, ui->label_MAX_16, ui->label_MIN_16);
    getCNoAVGMaxMin(ui->checkBox_GaE5ab , 17, ui->label_CNo_17 , "GAL:E5a+b  ", ui->label_AVG_17, ui->label_MAX_17, ui->label_MIN_17);
    getCNoAVGMaxMin(ui->checkBox_GaE6A  , 18, ui->label_CNo_18 , "GAL:E6A    ", ui->label_AVG_18, ui->label_MAX_18, ui->label_MIN_18);
    getCNoAVGMaxMin(ui->checkBox_GaE6BC , 19, ui->label_CNo_19 , "GAL:E6BC   ", ui->label_AVG_19, ui->label_MAX_19, ui->label_MIN_19);
    getCNoAVGMaxMin(ui->checkBox_GaL1A  , 20, ui->label_CNo_20 , "GAL:L1A    ", ui->label_AVG_20, ui->label_MAX_20, ui->label_MIN_20);
    getCNoAVGMaxMin(ui->checkBox_GaL1BC , 21, ui->label_CNo_21 , "GAL:L1BC   ", ui->label_AVG_21, ui->label_MAX_21, ui->label_MIN_21);
    getCNoAVGMaxMin(ui->checkBox_GBALL  , 22, ui->label_CNo_22 , "BDS:ALL    ", ui->label_AVG_22, ui->label_MAX_22, ui->label_MIN_22);
    getCNoAVGMaxMin(ui->checkBox_BDSB1I , 23, ui->label_CNo_23 , "BDS:B1I    ", ui->label_AVG_23, ui->label_MAX_23, ui->label_MIN_23);
    getCNoAVGMaxMin(ui->checkBox_BDSB1Q , 24, ui->label_CNo_24 , "BDS:B1Q    ", ui->label_AVG_24, ui->label_MAX_24, ui->label_MIN_24);
    getCNoAVGMaxMin(ui->checkBox_BDSB1C , 25, ui->label_CNo_25 , "BDS:B1C    ", ui->label_AVG_25, ui->label_MAX_25, ui->label_MIN_25);
    getCNoAVGMaxMin(ui->checkBox_BDSB1A , 26, ui->label_CNo_26 , "BDS:B1A    ", ui->label_AVG_26, ui->label_MAX_26, ui->label_MIN_26);
    getCNoAVGMaxMin(ui->checkBox_BDSB2a , 27, ui->label_CNo_27 , "BDS:B2a    ", ui->label_AVG_27, ui->label_MAX_27, ui->label_MIN_27);
    getCNoAVGMaxMin(ui->checkBox_BDSB2b , 28, ui->label_CNo_28 , "BDS:B2b    ", ui->label_AVG_28, ui->label_MAX_28, ui->label_MIN_28);
    getCNoAVGMaxMin(ui->checkBox_BDSB2ab, 29, ui->label_CNo_29 , "BDS:B2a+b  ", ui->label_AVG_29, ui->label_MAX_29, ui->label_MIN_29);
    getCNoAVGMaxMin(ui->checkBox_BDSB3I , 30, ui->label_CNo_30 , "BDS:B3I    ", ui->label_AVG_30, ui->label_MAX_30, ui->label_MIN_30);
    getCNoAVGMaxMin(ui->checkBox_BDSB3Q , 31, ui->label_CNo_31 , "BDS:B3Q    ", ui->label_AVG_31, ui->label_MAX_31, ui->label_MIN_31);
    getCNoAVGMaxMin(ui->checkBox_BDSB3A , 32, ui->label_CNo_32 , "BDS:B3A    ", ui->label_AVG_32, ui->label_MAX_32, ui->label_MIN_32);
    getCNoAVGMaxMin(ui->checkBox_BDSB2I , 33, ui->label_CNo_33 , "BDS:B2I    ", ui->label_AVG_33, ui->label_MAX_33, ui->label_MIN_33);
    getCNoAVGMaxMin(ui->checkBox_BDSB2Q , 34, ui->label_CNo_34 , "BDS:B2Q    ", ui->label_AVG_34, ui->label_MAX_34, ui->label_MIN_34);
    getCNoAVGMaxMin(ui->checkBox_GQALL  , 35, ui->label_CNo_35 , "QZS:ALL    ", ui->label_AVG_35, ui->label_MAX_35, ui->label_MIN_35);
    getCNoAVGMaxMin(ui->checkBox_GQL1CA , 36, ui->label_CNo_36 , "QZS:L1C/A  ", ui->label_AVG_36, ui->label_MAX_36, ui->label_MIN_36);
    getCNoAVGMaxMin(ui->checkBox_GQL1CD , 37, ui->label_CNo_37 , "QZS:L1C(D) ", ui->label_AVG_37, ui->label_MAX_37, ui->label_MIN_37);
    getCNoAVGMaxMin(ui->checkBox_GQL1CP , 38, ui->label_CNo_38 , "QZS:L1C(P) ", ui->label_AVG_38, ui->label_MAX_38, ui->label_MIN_38);
    getCNoAVGMaxMin(ui->checkBox_GQLIS  , 39, ui->label_CNo_39 , "QZS:LIS    ", ui->label_AVG_39, ui->label_MAX_39, ui->label_MIN_39);
    getCNoAVGMaxMin(ui->checkBox_GQL2CM , 40, ui->label_CNo_40 , "QZS:L2C-M  ", ui->label_AVG_40, ui->label_MAX_40, ui->label_MIN_40);
    getCNoAVGMaxMin(ui->checkBox_GQL2CL , 41, ui->label_CNo_41 , "QZS:L2C-L  ", ui->label_AVG_41, ui->label_MAX_41, ui->label_MIN_41);
    getCNoAVGMaxMin(ui->checkBox_GQL5I  , 42, ui->label_CNo_42 , "QZS:L5-I   ", ui->label_AVG_42, ui->label_MAX_42, ui->label_MIN_42);
    getCNoAVGMaxMin(ui->checkBox_GQL5Q  , 43, ui->label_CNo_43 , "QZS:L5-Q   ", ui->label_AVG_43, ui->label_MAX_43, ui->label_MIN_43);
    getCNoAVGMaxMin(ui->checkBox_GQL6D  , 44, ui->label_CNo_44 , "QZS:L6D    ", ui->label_AVG_44, ui->label_MAX_44, ui->label_MIN_44);
    getCNoAVGMaxMin(ui->checkBox_GQL6E  , 45, ui->label_CNo_45 , "QZS:L6E    ", ui->label_AVG_45, ui->label_MAX_45, ui->label_MIN_45);
    getCNoAVGMaxMin(ui->checkBox_GIALL  , 46, ui->label_CNo_46 , "NIC:ALL    ", ui->label_AVG_46, ui->label_MAX_46, ui->label_MIN_46);
    getCNoAVGMaxMin(ui->checkBox_GIL5SPS, 47, ui->label_CNo_47 , "NIC:L5-SPS ", ui->label_AVG_47, ui->label_MAX_47, ui->label_MIN_47);
    getCNoAVGMaxMin(ui->checkBox_GISSPS , 48, ui->label_CNo_48 , "NIC:S-SPS  ", ui->label_AVG_48, ui->label_MAX_48, ui->label_MIN_48);
    getCNoAVGMaxMin(ui->checkBox_GIL5RS , 49, ui->label_CNo_49 , "NIC:L5-RS  ", ui->label_AVG_49, ui->label_MAX_49, ui->label_MIN_49);
    getCNoAVGMaxMin(ui->checkBox_GISRS  , 50, ui->label_CNo_50 , "NIC:S-RS   ", ui->label_AVG_50, ui->label_MAX_50, ui->label_MIN_50);
    getCNoAVGMaxMin(ui->checkBox_GIL1SPS, 51, ui->label_CNo_51 , "NIC:L1-SPS ", ui->label_AVG_51, ui->label_MAX_51, ui->label_MIN_51);
    getCNoAVGMaxMin(ui->checkBox_WAAS   , 52, ui->label_CNo_52 , "WAAS       ", ui->label_AVG_52, ui->label_MAX_52, ui->label_MIN_52);
    getCNoAVGMaxMin(ui->checkBox_SDCM   , 53, ui->label_CNo_53 , "SDCM       ", ui->label_AVG_53, ui->label_MAX_53, ui->label_MIN_53);
    getCNoAVGMaxMin(ui->checkBox_EGNOS  , 54, ui->label_CNo_54 , "EGNOS      ", ui->label_AVG_54, ui->label_MAX_54, ui->label_MIN_54);
    getCNoAVGMaxMin(ui->checkBox_BDSBAS , 55, ui->label_CNo_55 , "BDSBAS     ", ui->label_AVG_55, ui->label_MAX_55, ui->label_MIN_55);
    getCNoAVGMaxMin(ui->checkBox_MSAS   , 56, ui->label_CNo_56 , "MSAS       ", ui->label_AVG_56, ui->label_MAX_56, ui->label_MIN_56);
    getCNoAVGMaxMin(ui->checkBox_GAGAN  , 57, ui->label_CNo_57 , "GAGAN      ", ui->label_AVG_57, ui->label_MAX_57, ui->label_MIN_57);
}

void logPlot_MainWindow::plotCrete(int id, QColor color, QVector<double> x_data, QVector<double> y_data,QString graphName_){
    ui->customPlot->addGraph();
    ui->customPlot->graph(id)->setName(graphName_);
    ui->customPlot->graph(id)->setPen(QPen(color));
    if(plotFlag){
        ui->customPlot->graph(id)->setAdaptiveSampling(true);
        ui->customPlot->graph(id)->setLineStyle(QCPGraph::lsNone);
        ui->customPlot->graph(id)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusSquare, 1.5));
    }else{
        ui->customPlot->graph(id)->setLineStyle(QCPGraph::lsLine);
        ui->customPlot->graph(id)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
    }
    ui->customPlot->graph(id)->addData(x_data,y_data);
}

void logPlot_MainWindow::plotData(){
    for(int i=0; i<plotColor.count()-3; ++i){
        plotCrete(i , plotColor[i],x_dataList[i], y_dataList[i],graphName[i]);
    }

    ui->customPlot->addGraph();
    QPen pen;
    pen.setWidth(1);
    pen.setStyle(Qt::PenStyle::DashLine);
    pen.setColor(plotColor[58]);
    ui->customPlot->graph(58)->setName(graphName[58]);
    ui->customPlot->graph(58)->setPen(pen);
    ui->customPlot->graph(58)->addData(all_x,AlarmValue_y);
    ui->checkBox_BlackLine->setCheckState(Qt::Checked);

    ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis2);//----
    ui->customPlot->graph(59)->setName(graphName[59]);
    ui->customPlot->graph(59)->setPen(QPen(plotColor[59]));
    ui->customPlot->graph(59)->addData(PositionState_x,PositionState_y);
    ui->checkBox_Status->setCheckState(Qt::Checked);

    ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);//---
    ui->customPlot->graph(60)->setName(graphName[60]);
    ui->customPlot->graph(60)->setPen(QPen(plotColor[60]));
    ui->checkBox_GrandMean->setCheckState(Qt::Checked);
    if(plotFlag){
        ui->customPlot->graph(60)->setAdaptiveSampling(true);
        ui->customPlot->graph(60)->setLineStyle(QCPGraph::lsNone);
        ui->customPlot->graph(60)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusSquare, 1.5));

    }else{
        ui->customPlot->graph(60)->setLineStyle(QCPGraph::lsLine);
        ui->customPlot->graph(60)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
    }
    if(ui->radioButton_CNo->isChecked()){
        ui->customPlot->yAxis->setLabel(QStringLiteral("C/No(dB-Hz)"));
        ui->customPlot->graph(60)->setVisible(false);
    }else if(ui->radioButton_View->isChecked()){
        ui->customPlot->yAxis->setLabel(QStringLiteral("SVs"));
        ui->customPlot->graph(60)->addData(x_VU,y_VU);
        ui->customPlot->graph(60)->setVisible(true);
    }else if(ui->radioButton_Used->isChecked()){
        ui->customPlot->yAxis->setLabel(QStringLiteral("SUs"));
        ui->customPlot->graph(60)->addData(x_VU,y_VU);
        ui->customPlot->graph(60)->setVisible(true);
    }
    /**
     * @brief axes
     */
    QList<QCPAxis*> axes;
    axes << ui->customPlot->yAxis<< ui->customPlot->xAxis<< ui->customPlot->yAxis2;
    ui->customPlot->axisRect()->setRangeZoomAxes(axes);   
    ui->customPlot->axisRect()->setRangeDragAxes(axes);

    ui->customPlot->rescaleAxes(true);
    ui->customPlot->setNoAntialiasingOnDrag(true);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom /*| QCP::iSelectPlottables*/);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
//    ui->customPlot->xAxis->setLabel(QStringLiteral("Indexes"));
}

void logPlot_MainWindow::showPlotData(){
    QList<double> qList = PositionStateMap.keys();
    for (int v=0; v<PositionStateMap.count(); ++v) {
        PositionState_x.append(qList[v]);
        PositionState_y.append(PositionStateMap.value(qList[v]));
    }
    for (int i=0; i<frameDataListALL.count(); ++i) {
        QVector<double> yDataVector;
        QVector<double> xDataVector;
        for(int j=0; j<frameDataListALL[i].count(); ++j){
            xDataVector.append(frameDataListALL[i][j].serialNumber);
            if(ui->radioButton_CNo->isChecked()){
                yDataVector.append( frameDataListALL[i][j].CN0_Average);
            }else if(ui->radioButton_Used->isChecked()){
                yDataVector.append((double) frameDataListALL[i][j].TotalNumSatUsed);
            }else if(ui->radioButton_View->isChecked()){
                yDataVector.append((double) frameDataListALL[i][j].TotalNumSatView);
            }
        }
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        x_dataList.append(xDataVector);
        y_dataList.append(yDataVector);
    }

    QVector<double> xUTCVector;
    xUTCVector.clear();
    for(int k=0; k<x_dataList.count(); ++k){
        if(x_dataList[k].count()>0){
            xUTCVector.append(x_dataList[k]);
        }
    }
    std::sort(xUTCVector.begin(),xUTCVector.end());
    auto it= std::unique(xUTCVector.begin(),xUTCVector.end());
    xUTCVector.erase(it,xUTCVector.end());

//    QList<QMap<double,double>>qListmap;
    qMListmap.clear();
    QMap<double,double>qmap;
    double mapDouble = 0.0;
    x_VU = xUTCVector;
    all_x=xUTCVector;
    if(ui->radioButton_CNo->isChecked()){
        for (int h=0; h<all_x.count(); ++h) {
            AlarmValue_y.append(35);
        }
    }else if(ui->radioButton_Used->isChecked()){
        for (int h=0; h<all_x.count(); ++h) {
            AlarmValue_y.append(4);
        }
    }else if(ui->radioButton_View->isChecked()){
        for (int h=0; h<all_x.count(); ++h) {
            AlarmValue_y.append(4);
        }
    }

    for (int i=0; i<x_dataList.count(); ++i) {
        if(x_dataList[i].count()>0){
            for (int j=0; j<x_dataList[i].count(); ++j) {
                qmap.insert(x_dataList[i][j],y_dataList[i][j]);
            }
            qMListmap.append(qmap);
            qmap.clear();
        }
    }
    for (int n=0; n<xUTCVector.count(); ++n) {
        for (int m=0; m<qMListmap.count(); ++m) {
            if(qMListmap[m].contains(xUTCVector[n]))
            {
                mapDouble += qMListmap[m].value(xUTCVector[n]);
            }
        }
        y_VU.append(mapDouble);
        mapDouble = 0.0;
    }
    x_dataList.append(all_x);
    x_dataList.append(PositionState_x);
    x_dataList.append(x_VU);
}

void logPlot_MainWindow:: curveShowOrHide(QCheckBox *checkBox, int id, QLabel *signalChannel, QLabel *avgCNo, QLabel *maxCNo, QLabel *minCNo){
    if((!x_dataList.isEmpty())){
        if(checkBox->isChecked()){
            signalChannel->setHidden(false);
            avgCNo->setHidden(false);
            maxCNo->setHidden(false);
            minCNo->setHidden(false);
            ui->customPlot->graph(id)->setVisible(true);
        }else {
            signalChannel->setHidden(true);
            avgCNo->setHidden(true);
            maxCNo->setHidden(true);
            minCNo->setHidden(true);
            ui->customPlot->graph(id)->setVisible(false);
        }
        setStyleAllCheckBox();
    }
}

void logPlot_MainWindow::setPlotColorList(){
    plotColor.append("#5500DD");  //gpsALL
    plotColor.append("#483D8B");  //gpsL1
    plotColor.append("#0000CD");  //gpsL1PY
    plotColor.append("#1E90FF");  //gpsL1M
    plotColor.append("#6495ED");  //gpsL2PY
    plotColor.append("#00BFFF");  //gpsL2CM
    plotColor.append("#87CEFA");  //gpsL2CL
    plotColor.append("#5555FF");  //gpsL5I
    plotColor.append("#ADD8E6");  //gpsL5
    plotColor.append("#886600");  //glALL
    plotColor.append("#DDAA00");  //glG1
    plotColor.append("#FFDD55");  //glG1P
    plotColor.append("#EEEE00");  //glG2CA
    plotColor.append("#BDB76B");  //glG2P
    plotColor.append("#006400");  //gaALL
    plotColor.append("#2E8B57");  //gaE5a
    plotColor.append("#3CB371");  //gaE5b
    plotColor.append("#9ACD32");  //gaE5ab
    plotColor.append("#00FF00");  //gaE6A
    plotColor.append("#7CFC00");  //gaE6BC
    plotColor.append("#ADFF2F");  //gaL1a
    plotColor.append("#90EE90");  //gaL1BC
    plotColor.append("#880000");  //gbALL
    plotColor.append("#CC0000");  //gbLB1i
    plotColor.append("#FF3333");  //gbLB1Q
    plotColor.append("#FF8888");  //gbLB1C
    plotColor.append("#A42D00");  //gbLB1A
    plotColor.append("#FF5511");  //gbLB2a
    plotColor.append("#FFA488");  //gbLB2b
    plotColor.append("#BB5500");  //gbLB2ab
    plotColor.append("#EE7700");  //gbLB3I
    plotColor.append("#FFAA33");  //gbLB3Q
    plotColor.append("#FFBB66");  //gbLB3A
    plotColor.append("#FFDDAA");  //gbLB2I
    plotColor.append("#FFCCCC");  //gbLB2Q
    plotColor.append("#770077");  //gQALL
    plotColor.append("#7A0099");  //gQL1
    plotColor.append("#FF3EFF");  //gQL1CD
    plotColor.append("#CC00FF");  //gQL1CP
    plotColor.append("#E38EFF");  //gQLIS
    plotColor.append("#550088");  //gQL2CM
    plotColor.append("#9900FF");  //gQL2CL
    plotColor.append("#D28EFF");  //gQL5I
    plotColor.append("#E8CCFF");  //gQL5q
    plotColor.append("#9F88FF");  //gQL6D
    plotColor.append("#9999FF");  //gQL6E
    plotColor.append("#8C0044");  //giALL
    plotColor.append("#FFB7DD");  //giL5SPS
    plotColor.append("#A20055");  //giSSPS
    plotColor.append("#FF0088");  //giL5RS
    plotColor.append("#FF44AA");  //giSRS
    plotColor.append("#FF88C2");  //giL1SPS
    plotColor.append("#0000CD");  //WAAS
    plotColor.append("#FFD700");  //SDCM
    plotColor.append("#8FBC8F");  //EGNOS
    plotColor.append("#FF0000");  //BDSBAS
    plotColor.append("#C71585");  //MSAS
    plotColor.append("#800080");  //GAGAN
    plotColor.append("#DC143C");  //Threshold
    plotColor.append("#00ffff");  //Status
    plotColor.append("#000000");  //TotalAverage
    //NAME
    graphName.append("GPS:ALL   ");  //gpsALL
    graphName.append("GPS:L1/A  ");  //gpsL1
    graphName.append("GPS:L1P(Y)");  //gpsL1PY
    graphName.append("GPS:L1M   ");  //gpsL1M
    graphName.append("GPS:L2P(Y)");  //gpsL2PY
    graphName.append("GPS:L2C-M ");  //gpsL2CM
    graphName.append("GPS:L2C-L ");  //gpsL2CL
    graphName.append("GPS:L5-I  ");  //gpsL5I
    graphName.append("GPS:L5-Q  ");  //gpsL5
    graphName.append("GLO:ALL   ");  //glALL
    graphName.append("GLO:G1C/A ");  //glG1
    graphName.append("GLO:G1P   ");  //glG1P
    graphName.append("GLO:G2C/A ");  //glG2CA
    graphName.append("GLO:G2P   ");  //glG2P
    graphName.append("GAL:ALL   ");  //gaALL
    graphName.append("GAL:E5a   ");  //gaE5a
    graphName.append("GAL:E5b   ");  //gaE5b
    graphName.append("GAL:E5a+b ");  //gaE5ab
    graphName.append("GAL:E6A   ");  //gaE6A
    graphName.append("GAL:E6BC  ");  //gaE6BC
    graphName.append("GAL:L1A   ");  //gaL1a
    graphName.append("GAL:L1BC  ");  //gaL1BC
    graphName.append("BDS:ALL   ");  //gbALL
    graphName.append("BDS:B1I   ");  //gbLB1i
    graphName.append("BDS:B1Q   ");  //gbLB1Q
    graphName.append("BDS:B1C   ");  //gbLB1C
    graphName.append("BDS:B1A   ");  //gbLB1A
    graphName.append("BDS:B2a   ");  //gbLB2a
    graphName.append("BDS:B2b   ");  //gbLB2b
    graphName.append("BDS:B2a+b ");  //gbLB2ab
    graphName.append("BDS:B3I   ");  //gbLB3I
    graphName.append("BDS:B3Q   ");  //gbLB3Q
    graphName.append("BDS:B3A   ");  //gbLB3A
    graphName.append("BDS:B2I   ");  //gbLB2I
    graphName.append("BDS:B2Q   ");  //gbLB2Q
    graphName.append("QZS:ALL   ");  //gQALL
    graphName.append("QZS:L1C/A ");  //gQL1
    graphName.append("QZS:L1C(D)");  //gQL1CD
    graphName.append("QZS:L1C(P)");  //gQL1CP
    graphName.append("QZS:LIS   ");  //gQLIS
    graphName.append("QZS:L2C-M ");  //gQL2CM
    graphName.append("QZS:L2C-L ");  //gQL2CL
    graphName.append("QZS:L5-I  ");  //gQL5I
    graphName.append("QZS:L5-Q  ");  //gQL5q
    graphName.append("QZS:L6D   ");  //gQL6D
    graphName.append("QZS:L6E   ");  //gQL6E
    graphName.append("NIC:ALL   ");  //giALL
    graphName.append("NIC:L5-SPS");  //giL5SPS
    graphName.append("NIC:S-SPS ");  //giSSPS
    graphName.append("NIC:L5-RS ");  //giL5RS
    graphName.append("NIC:S-RS  ");  //giSRS
    graphName.append("NIC:L1-SPS");  //giL1SPS
    graphName.append("WAAS      ");  //WAAS
    graphName.append("SDCM      ");  //SDCM
    graphName.append("EGNOS     ");  //EGNOS
    graphName.append("BDSBAS    ");  //BDSBAS
    graphName.append("MSAS      ");  //MSAS
    graphName.append("GAGAN     ");  //GAGAN
    graphName.append("Threshold ");  //Threshold
    graphName.append("Status    ");  //Status
    graphName.append("TotalAverage");//TotalAverage
}

void logPlot_MainWindow::setSlotsAssemble(){
    curveShowOrHide(ui->checkBox_GPALL  , 0 , ui->label_CNo_0 , ui->label_AVG_0 , ui->label_MAX_0 , ui->label_MIN_0 );
    curveShowOrHide(ui->checkBox_GPSL1CA, 1 , ui->label_CNo_1 , ui->label_AVG_1 , ui->label_MAX_1 , ui->label_MIN_1 );
    curveShowOrHide(ui->checkBox_GPSL1PY, 2 , ui->label_CNo_2 , ui->label_AVG_2 , ui->label_MAX_2 , ui->label_MIN_2 );
    curveShowOrHide(ui->checkBox_GPSL1M , 3 , ui->label_CNo_3 , ui->label_AVG_3 , ui->label_MAX_3 , ui->label_MIN_3 );
    curveShowOrHide(ui->checkBox_GPSL2PY, 4 , ui->label_CNo_4 , ui->label_AVG_4 , ui->label_MAX_4 , ui->label_MIN_4 );
    curveShowOrHide(ui->checkBox_GPSL2CM, 5 , ui->label_CNo_5 , ui->label_AVG_5 , ui->label_MAX_5 , ui->label_MIN_5 );
    curveShowOrHide(ui->checkBox_GPSL2CL, 6 , ui->label_CNo_6 , ui->label_AVG_6 , ui->label_MAX_6 , ui->label_MIN_6 );
    curveShowOrHide(ui->checkBox_GPSL5I , 7 , ui->label_CNo_7 , ui->label_AVG_7 , ui->label_MAX_7 , ui->label_MIN_7 );
    curveShowOrHide(ui->checkBox_GPSL5Q , 8 , ui->label_CNo_8 , ui->label_AVG_8 , ui->label_MAX_8 , ui->label_MIN_8 );
    curveShowOrHide(ui->checkBox_GLALL  , 9 , ui->label_CNo_9 , ui->label_AVG_9 , ui->label_MAX_9 , ui->label_MIN_9 );
    curveShowOrHide(ui->checkBox_GLG1CA , 10, ui->label_CNo_10, ui->label_AVG_10, ui->label_MAX_10, ui->label_MIN_10);
    curveShowOrHide(ui->checkBox_GLG1P  , 11, ui->label_CNo_11, ui->label_AVG_11, ui->label_MAX_11, ui->label_MIN_11);
    curveShowOrHide(ui->checkBox_GLG2CA , 12, ui->label_CNo_12, ui->label_AVG_12, ui->label_MAX_12, ui->label_MIN_12);
    curveShowOrHide(ui->checkBox_GLG2P  , 13, ui->label_CNo_13, ui->label_AVG_13, ui->label_MAX_13, ui->label_MIN_13);
    curveShowOrHide(ui->checkBox_GAALL  , 14, ui->label_CNo_14, ui->label_AVG_14, ui->label_MAX_14, ui->label_MIN_14);
    curveShowOrHide(ui->checkBox_GaE5a  , 15, ui->label_CNo_15, ui->label_AVG_15, ui->label_MAX_15, ui->label_MIN_15);
    curveShowOrHide(ui->checkBox_GaE5b  , 16, ui->label_CNo_16, ui->label_AVG_16, ui->label_MAX_16, ui->label_MIN_16);
    curveShowOrHide(ui->checkBox_GaE5ab , 17, ui->label_CNo_17, ui->label_AVG_17, ui->label_MAX_17, ui->label_MIN_17);
    curveShowOrHide(ui->checkBox_GaE6A  , 18, ui->label_CNo_18, ui->label_AVG_18, ui->label_MAX_18, ui->label_MIN_18);
    curveShowOrHide(ui->checkBox_GaE6BC , 19, ui->label_CNo_19, ui->label_AVG_19, ui->label_MAX_19, ui->label_MIN_19);
    curveShowOrHide(ui->checkBox_GaL1A  , 20, ui->label_CNo_20, ui->label_AVG_20, ui->label_MAX_20, ui->label_MIN_20);
    curveShowOrHide(ui->checkBox_GaL1BC , 21, ui->label_CNo_21, ui->label_AVG_21, ui->label_MAX_21, ui->label_MIN_21);
    curveShowOrHide(ui->checkBox_GBALL  , 22, ui->label_CNo_22, ui->label_AVG_22, ui->label_MAX_22, ui->label_MIN_22);
    curveShowOrHide(ui->checkBox_BDSB1I , 23, ui->label_CNo_23, ui->label_AVG_23, ui->label_MAX_23, ui->label_MIN_23);
    curveShowOrHide(ui->checkBox_BDSB1Q , 24, ui->label_CNo_24, ui->label_AVG_24, ui->label_MAX_24, ui->label_MIN_24);
    curveShowOrHide(ui->checkBox_BDSB1C , 25, ui->label_CNo_25, ui->label_AVG_25, ui->label_MAX_25, ui->label_MIN_25);
    curveShowOrHide(ui->checkBox_BDSB1A , 26, ui->label_CNo_26, ui->label_AVG_26, ui->label_MAX_26, ui->label_MIN_26);
    curveShowOrHide(ui->checkBox_BDSB2a , 27, ui->label_CNo_27, ui->label_AVG_27, ui->label_MAX_27, ui->label_MIN_27);
    curveShowOrHide(ui->checkBox_BDSB2b , 28, ui->label_CNo_28, ui->label_AVG_28, ui->label_MAX_28, ui->label_MIN_28);
    curveShowOrHide(ui->checkBox_BDSB2ab, 29, ui->label_CNo_29, ui->label_AVG_29, ui->label_MAX_29, ui->label_MIN_29);
    curveShowOrHide(ui->checkBox_BDSB3I , 30, ui->label_CNo_30, ui->label_AVG_30, ui->label_MAX_30, ui->label_MIN_30);
    curveShowOrHide(ui->checkBox_BDSB3Q , 31, ui->label_CNo_31, ui->label_AVG_31, ui->label_MAX_31, ui->label_MIN_31);
    curveShowOrHide(ui->checkBox_BDSB3A , 32, ui->label_CNo_32, ui->label_AVG_32, ui->label_MAX_32, ui->label_MIN_32);
    curveShowOrHide(ui->checkBox_BDSB2I , 33, ui->label_CNo_33, ui->label_AVG_33, ui->label_MAX_33, ui->label_MIN_33);
    curveShowOrHide(ui->checkBox_BDSB2Q , 34, ui->label_CNo_34, ui->label_AVG_34, ui->label_MAX_34, ui->label_MIN_34);
    curveShowOrHide(ui->checkBox_GQALL  , 35, ui->label_CNo_35, ui->label_AVG_35, ui->label_MAX_35, ui->label_MIN_35);
    curveShowOrHide(ui->checkBox_GQL1CA , 36, ui->label_CNo_36, ui->label_AVG_36, ui->label_MAX_36, ui->label_MIN_36);
    curveShowOrHide(ui->checkBox_GQL1CD , 37, ui->label_CNo_37, ui->label_AVG_37, ui->label_MAX_37, ui->label_MIN_37);
    curveShowOrHide(ui->checkBox_GQL1CP , 38, ui->label_CNo_38, ui->label_AVG_38, ui->label_MAX_38, ui->label_MIN_38);
    curveShowOrHide(ui->checkBox_GQLIS  , 39, ui->label_CNo_39, ui->label_AVG_39, ui->label_MAX_39, ui->label_MIN_39);
    curveShowOrHide(ui->checkBox_GQL2CM , 40, ui->label_CNo_40, ui->label_AVG_40, ui->label_MAX_40, ui->label_MIN_40);
    curveShowOrHide(ui->checkBox_GQL2CL , 41, ui->label_CNo_41, ui->label_AVG_41, ui->label_MAX_41, ui->label_MIN_41);
    curveShowOrHide(ui->checkBox_GQL5I  , 42, ui->label_CNo_42, ui->label_AVG_42, ui->label_MAX_42, ui->label_MIN_42);
    curveShowOrHide(ui->checkBox_GQL5Q  , 43, ui->label_CNo_43, ui->label_AVG_43, ui->label_MAX_43, ui->label_MIN_43);
    curveShowOrHide(ui->checkBox_GQL6D  , 44, ui->label_CNo_44, ui->label_AVG_44, ui->label_MAX_44, ui->label_MIN_44);
    curveShowOrHide(ui->checkBox_GQL6E  , 45, ui->label_CNo_45, ui->label_AVG_45, ui->label_MAX_45, ui->label_MIN_45);
    curveShowOrHide(ui->checkBox_GIALL  , 46, ui->label_CNo_46, ui->label_AVG_46, ui->label_MAX_46, ui->label_MIN_46);
    curveShowOrHide(ui->checkBox_GIL5SPS, 47, ui->label_CNo_47, ui->label_AVG_47, ui->label_MAX_47, ui->label_MIN_47);
    curveShowOrHide(ui->checkBox_GISSPS , 48, ui->label_CNo_48, ui->label_AVG_48, ui->label_MAX_48, ui->label_MIN_48);
    curveShowOrHide(ui->checkBox_GIL5RS , 49, ui->label_CNo_49, ui->label_AVG_49, ui->label_MAX_49, ui->label_MIN_49);
    curveShowOrHide(ui->checkBox_GISRS  , 50, ui->label_CNo_50, ui->label_AVG_50, ui->label_MAX_50, ui->label_MIN_50);
    curveShowOrHide(ui->checkBox_GIL1SPS, 51, ui->label_CNo_51, ui->label_AVG_51, ui->label_MAX_51, ui->label_MIN_51);
    curveShowOrHide(ui->checkBox_WAAS   , 52, ui->label_CNo_52, ui->label_AVG_52, ui->label_MAX_52, ui->label_MIN_52);
    curveShowOrHide(ui->checkBox_SDCM   , 53, ui->label_CNo_53, ui->label_AVG_53, ui->label_MAX_53, ui->label_MIN_53);
    curveShowOrHide(ui->checkBox_EGNOS  , 54, ui->label_CNo_54, ui->label_AVG_54, ui->label_MAX_54, ui->label_MIN_54);
    curveShowOrHide(ui->checkBox_BDSBAS , 55, ui->label_CNo_55, ui->label_AVG_55, ui->label_MAX_55, ui->label_MIN_55);
    curveShowOrHide(ui->checkBox_MSAS   , 56, ui->label_CNo_56, ui->label_AVG_56, ui->label_MAX_56, ui->label_MIN_56);
    curveShowOrHide(ui->checkBox_GAGAN  , 57, ui->label_CNo_57, ui->label_AVG_57, ui->label_MAX_57, ui->label_MIN_57);
    ui->customPlot->rescaleAxes(true);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}

void logPlot_MainWindow::getCheckboxTickStatus(QCheckBox *checkbox, QVector<double>y_data ,QLabel *label){
    if(label->text().toInt() > 0){
        checkbox->setCheckState(y_data.count()>0? Qt::Checked:Qt::Unchecked);
        checkbox->setEnabled(y_data.count()>0?true:false);
    }else{
        checkbox->setCheckState(Qt::Unchecked);
        checkbox->setEnabled(false);
    }
}

void logPlot_MainWindow::checkboxTickControl(){
    getCheckboxTickStatus(ui->checkBox_GPALL  ,y_dataList[0 ], ui->label_MAX_0 );
    getCheckboxTickStatus(ui->checkBox_GPSL1CA,y_dataList[1 ], ui->label_MAX_1 );
    getCheckboxTickStatus(ui->checkBox_GPSL1PY,y_dataList[2 ], ui->label_MAX_2 );
    getCheckboxTickStatus(ui->checkBox_GPSL1M ,y_dataList[3 ], ui->label_MAX_3 );
    getCheckboxTickStatus(ui->checkBox_GPSL2PY,y_dataList[4 ], ui->label_MAX_4 );
    getCheckboxTickStatus(ui->checkBox_GPSL2CM,y_dataList[5 ], ui->label_MAX_5 );
    getCheckboxTickStatus(ui->checkBox_GPSL2CL,y_dataList[6 ], ui->label_MAX_6 );
    getCheckboxTickStatus(ui->checkBox_GPSL5I ,y_dataList[7 ], ui->label_MAX_7 );
    getCheckboxTickStatus(ui->checkBox_GPSL5Q ,y_dataList[8 ], ui->label_MAX_8 );
    getCheckboxTickStatus(ui->checkBox_GLALL  ,y_dataList[9 ], ui->label_MAX_9 );
    getCheckboxTickStatus(ui->checkBox_GLG1CA ,y_dataList[10], ui->label_MAX_10);
    getCheckboxTickStatus(ui->checkBox_GLG1P  ,y_dataList[11], ui->label_MAX_11);
    getCheckboxTickStatus(ui->checkBox_GLG2CA ,y_dataList[12], ui->label_MAX_12);
    getCheckboxTickStatus(ui->checkBox_GLG2P  ,y_dataList[13], ui->label_MAX_13);
    getCheckboxTickStatus(ui->checkBox_GAALL  ,y_dataList[14], ui->label_MAX_14);
    getCheckboxTickStatus(ui->checkBox_GaE5a  ,y_dataList[15], ui->label_MAX_15);
    getCheckboxTickStatus(ui->checkBox_GaE5b  ,y_dataList[16], ui->label_MAX_16);
    getCheckboxTickStatus(ui->checkBox_GaE5ab ,y_dataList[17], ui->label_MAX_17);
    getCheckboxTickStatus(ui->checkBox_GaE6A  ,y_dataList[18], ui->label_MAX_18);
    getCheckboxTickStatus(ui->checkBox_GaE6BC ,y_dataList[19], ui->label_MAX_19);
    getCheckboxTickStatus(ui->checkBox_GaL1A  ,y_dataList[20], ui->label_MAX_20);
    getCheckboxTickStatus(ui->checkBox_GaL1BC ,y_dataList[21], ui->label_MAX_21);
    getCheckboxTickStatus(ui->checkBox_GBALL  ,y_dataList[22], ui->label_MAX_22);
    getCheckboxTickStatus(ui->checkBox_BDSB1I ,y_dataList[23], ui->label_MAX_23);
    getCheckboxTickStatus(ui->checkBox_BDSB1Q ,y_dataList[24], ui->label_MAX_24);
    getCheckboxTickStatus(ui->checkBox_BDSB1C ,y_dataList[25], ui->label_MAX_25);
    getCheckboxTickStatus(ui->checkBox_BDSB1A ,y_dataList[26], ui->label_MAX_26);
    getCheckboxTickStatus(ui->checkBox_BDSB2a ,y_dataList[27], ui->label_MAX_27);
    getCheckboxTickStatus(ui->checkBox_BDSB2b ,y_dataList[28], ui->label_MAX_28);
    getCheckboxTickStatus(ui->checkBox_BDSB2ab,y_dataList[29], ui->label_MAX_29);
    getCheckboxTickStatus(ui->checkBox_BDSB3I ,y_dataList[30], ui->label_MAX_30);
    getCheckboxTickStatus(ui->checkBox_BDSB3Q ,y_dataList[31], ui->label_MAX_31);
    getCheckboxTickStatus(ui->checkBox_BDSB3A ,y_dataList[32], ui->label_MAX_32);
    getCheckboxTickStatus(ui->checkBox_BDSB2I ,y_dataList[33], ui->label_MAX_33);
    getCheckboxTickStatus(ui->checkBox_BDSB2Q ,y_dataList[34], ui->label_MAX_34);
    getCheckboxTickStatus(ui->checkBox_GQALL  ,y_dataList[35], ui->label_MAX_35);
    getCheckboxTickStatus(ui->checkBox_GQL1CA ,y_dataList[36], ui->label_MAX_36);
    getCheckboxTickStatus(ui->checkBox_GQL1CD ,y_dataList[37], ui->label_MAX_37);
    getCheckboxTickStatus(ui->checkBox_GQL1CP ,y_dataList[38], ui->label_MAX_38);
    getCheckboxTickStatus(ui->checkBox_GQLIS  ,y_dataList[39], ui->label_MAX_39);
    getCheckboxTickStatus(ui->checkBox_GQL2CM ,y_dataList[40], ui->label_MAX_40);
    getCheckboxTickStatus(ui->checkBox_GQL2CL ,y_dataList[41], ui->label_MAX_41);
    getCheckboxTickStatus(ui->checkBox_GQL5I  ,y_dataList[42], ui->label_MAX_42);
    getCheckboxTickStatus(ui->checkBox_GQL5Q  ,y_dataList[43], ui->label_MAX_43);
    getCheckboxTickStatus(ui->checkBox_GQL6D  ,y_dataList[44], ui->label_MAX_44);
    getCheckboxTickStatus(ui->checkBox_GQL6E  ,y_dataList[45], ui->label_MAX_45);
    getCheckboxTickStatus(ui->checkBox_GIALL  ,y_dataList[46], ui->label_MAX_46);
    getCheckboxTickStatus(ui->checkBox_GIL5SPS,y_dataList[47], ui->label_MAX_47);
    getCheckboxTickStatus(ui->checkBox_GISSPS ,y_dataList[48], ui->label_MAX_48);
    getCheckboxTickStatus(ui->checkBox_GIL5RS ,y_dataList[49], ui->label_MAX_49);
    getCheckboxTickStatus(ui->checkBox_GISRS  ,y_dataList[50], ui->label_MAX_50);
    getCheckboxTickStatus(ui->checkBox_GIL1SPS,y_dataList[51], ui->label_MAX_51);
    getCheckboxTickStatus(ui->checkBox_WAAS   ,y_dataList[52], ui->label_MAX_52);
    getCheckboxTickStatus(ui->checkBox_SDCM   ,y_dataList[53], ui->label_MAX_53);
    getCheckboxTickStatus(ui->checkBox_EGNOS  ,y_dataList[54], ui->label_MAX_54);
    getCheckboxTickStatus(ui->checkBox_BDSBAS ,y_dataList[55], ui->label_MAX_55);
    getCheckboxTickStatus(ui->checkBox_MSAS   ,y_dataList[56], ui->label_MAX_56);
    getCheckboxTickStatus(ui->checkBox_GAGAN  ,y_dataList[57], ui->label_MAX_57);
    setSlotsAssemble();
}

void logPlot_MainWindow::setCheckboxStatus(){
    if(ui->checkBox_ALL->isChecked()){
        ui->checkBox_GPALL  ->setCheckState(ui->checkBox_GPALL  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GPSL1CA->setCheckState(ui->checkBox_GPSL1CA->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GPSL1PY->setCheckState(ui->checkBox_GPSL1PY->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GPSL1M ->setCheckState(ui->checkBox_GPSL1M ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GPSL2PY->setCheckState(ui->checkBox_GPSL2PY->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GPSL2CM->setCheckState(ui->checkBox_GPSL2CM->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GPSL2CL->setCheckState(ui->checkBox_GPSL2CL->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GPSL5I ->setCheckState(ui->checkBox_GPSL5I ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GPSL5Q ->setCheckState(ui->checkBox_GPSL5Q ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GLALL  ->setCheckState(ui->checkBox_GLALL  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GLG1CA ->setCheckState(ui->checkBox_GLG1CA ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GLG1P  ->setCheckState(ui->checkBox_GLG1P  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GLG2CA ->setCheckState(ui->checkBox_GLG2CA ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GLG2P  ->setCheckState(ui->checkBox_GLG2P  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GAALL  ->setCheckState(ui->checkBox_GAALL  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GaE5a  ->setCheckState(ui->checkBox_GaE5a  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GaE5b  ->setCheckState(ui->checkBox_GaE5b  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GaE5ab ->setCheckState(ui->checkBox_GaE5ab ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GaE6A  ->setCheckState(ui->checkBox_GaE6A  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GaE6BC ->setCheckState(ui->checkBox_GaE6BC ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GaL1A  ->setCheckState(ui->checkBox_GaL1A  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GaL1BC ->setCheckState(ui->checkBox_GaL1BC ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GBALL  ->setCheckState(ui->checkBox_GBALL  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB1I ->setCheckState(ui->checkBox_BDSB1I ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB1Q ->setCheckState(ui->checkBox_BDSB1Q ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB1C ->setCheckState(ui->checkBox_BDSB1C ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB1A ->setCheckState(ui->checkBox_BDSB1A ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB2a ->setCheckState(ui->checkBox_BDSB2a ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB2b ->setCheckState(ui->checkBox_BDSB2b ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB2ab->setCheckState(ui->checkBox_BDSB2ab->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB3I ->setCheckState(ui->checkBox_BDSB3I ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB3Q ->setCheckState(ui->checkBox_BDSB3Q ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB3A ->setCheckState(ui->checkBox_BDSB3A ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB2I ->setCheckState(ui->checkBox_BDSB2I ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSB2Q ->setCheckState(ui->checkBox_BDSB2Q ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQALL  ->setCheckState(ui->checkBox_GQALL  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL1CA ->setCheckState(ui->checkBox_GQL1CA ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL1CD ->setCheckState(ui->checkBox_GQL1CD ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL1CP ->setCheckState(ui->checkBox_GQL1CP ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQLIS  ->setCheckState(ui->checkBox_GQLIS  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL2CM ->setCheckState(ui->checkBox_GQL2CM ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL2CL ->setCheckState(ui->checkBox_GQL2CL ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL5I  ->setCheckState(ui->checkBox_GQL5I  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL5Q  ->setCheckState(ui->checkBox_GQL5Q  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL6D  ->setCheckState(ui->checkBox_GQL6D  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GQL6E  ->setCheckState(ui->checkBox_GQL6E  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GIALL  ->setCheckState(ui->checkBox_GIALL  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GIL5SPS->setCheckState(ui->checkBox_GIL5SPS->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GISSPS ->setCheckState(ui->checkBox_GISSPS ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GIL5RS ->setCheckState(ui->checkBox_GIL5RS ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GISRS  ->setCheckState(ui->checkBox_GISRS  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GIL1SPS->setCheckState(ui->checkBox_GIL1SPS->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_WAAS   ->setCheckState(ui->checkBox_WAAS   ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_SDCM   ->setCheckState(ui->checkBox_SDCM   ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_EGNOS  ->setCheckState(ui->checkBox_EGNOS  ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_BDSBAS ->setCheckState(ui->checkBox_BDSBAS ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_MSAS   ->setCheckState(ui->checkBox_MSAS   ->isEnabled()?Qt::Checked:Qt::Unchecked);
        ui->checkBox_GAGAN  ->setCheckState(ui->checkBox_GAGAN  ->isEnabled()?Qt::Checked:Qt::Unchecked);
    }else{
        ui->checkBox_GPALL  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GPSL1CA->setCheckState(Qt::Unchecked);
        ui->checkBox_GPSL1PY->setCheckState(Qt::Unchecked);
        ui->checkBox_GPSL1M ->setCheckState(Qt::Unchecked);
        ui->checkBox_GPSL2PY->setCheckState(Qt::Unchecked);
        ui->checkBox_GPSL2CM->setCheckState(Qt::Unchecked);
        ui->checkBox_GPSL2CL->setCheckState(Qt::Unchecked);
        ui->checkBox_GPSL5I ->setCheckState(Qt::Unchecked);
        ui->checkBox_GPSL5Q ->setCheckState(Qt::Unchecked);
        ui->checkBox_GLALL  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GLG1CA ->setCheckState(Qt::Unchecked);
        ui->checkBox_GLG1P  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GLG2CA ->setCheckState(Qt::Unchecked);
        ui->checkBox_GLG2P  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GAALL  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GaE5a  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GaE5b  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GaE5ab ->setCheckState(Qt::Unchecked);
        ui->checkBox_GaE6A  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GaE6BC ->setCheckState(Qt::Unchecked);
        ui->checkBox_GaL1A  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GaL1BC ->setCheckState(Qt::Unchecked);
        ui->checkBox_GBALL  ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB1I ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB1Q ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB1C ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB1A ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB2a ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB2b ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB2ab->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB3I ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB3Q ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB3A ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB2I ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSB2Q ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQALL  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL1CA ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL1CD ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL1CP ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQLIS  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL2CM ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL2CL ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL5I  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL5Q  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL6D  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GQL6E  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GIALL  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GIL5SPS->setCheckState(Qt::Unchecked);
        ui->checkBox_GISSPS ->setCheckState(Qt::Unchecked);
        ui->checkBox_GIL5RS ->setCheckState(Qt::Unchecked);
        ui->checkBox_GISRS  ->setCheckState(Qt::Unchecked);
        ui->checkBox_GIL1SPS->setCheckState(Qt::Unchecked);
        ui->checkBox_WAAS   ->setCheckState(Qt::Unchecked);
        ui->checkBox_SDCM   ->setCheckState(Qt::Unchecked);
        ui->checkBox_EGNOS  ->setCheckState(Qt::Unchecked);
        ui->checkBox_BDSBAS ->setCheckState(Qt::Unchecked);
        ui->checkBox_MSAS   ->setCheckState(Qt::Unchecked);
        ui->checkBox_GAGAN  ->setCheckState(Qt::Unchecked);
    }
}

void logPlot_MainWindow::on_actionOpen_File_triggered()
{
    QString fileNameQString_ = QFileDialog::getOpenFileName(this,
                                                            tr("Open NMEA Log"), "", tr("NMEA log(*.log *.txt *.nma);;""AllFile(*.*)"));
    QTextCodec *code = QTextCodec::codecForName("GB2312");
    QString strUnicode= code->toUnicode(fileNameQString_.toLocal8Bit().data());
    Read_log(strUnicode.toStdString().c_str());
    QFileInfo fileInfo(strUnicode);
    if(fileInfo.baseName()!=""){
        fileName_save = fileInfo.baseName();
        QString tt = " ("+fileInfo.fileName()+")";
        QString nameFile = LOG_VERSION+tt;
        this->setWindowTitle(nameFile);
    }
}
void logPlot_MainWindow::on_radioButton_CNo_clicked(){
    if(!x_dataList.isEmpty()){
        QWaiting *pQwait = new QWaiting(this);
        pQwait->show();
        showPlot();
        pQwait->close();
    }
}

void logPlot_MainWindow::on_radioButton_View_clicked(){
    if(!x_dataList.isEmpty()){
        QWaiting *pQwait = new QWaiting(this);
        pQwait->show();
        showPlot();
        pQwait->close();
    }
}

void logPlot_MainWindow::on_radioButton_Used_clicked(){
    if(!x_dataList.isEmpty()){
        QWaiting *pQwait = new QWaiting(this);
        pQwait->show();
        showPlot();
        pQwait->close();
    }
}

void logPlot_MainWindow::on_actionAbout_QGNSSLog_triggered(){
    AboutDialog *pAbout = new AboutDialog(this);
    pAbout->show();
}
void logPlot_MainWindow::on_checkBox_GPALL_clicked(){
    curveShowOrHide(ui->checkBox_GPALL, 0, ui->label_CNo_0, ui->label_AVG_0, ui->label_MAX_0, ui->label_MIN_0);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GPSL1CA_clicked(){
    curveShowOrHide(ui->checkBox_GPSL1CA, 1, ui->label_CNo_1, ui->label_AVG_1, ui->label_MAX_1, ui->label_MIN_1);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GPSL1PY_clicked(){
    curveShowOrHide(ui->checkBox_GPSL1PY, 2, ui->label_CNo_2, ui->label_AVG_2, ui->label_MAX_2, ui->label_MIN_2);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GPSL1M_clicked(){
    curveShowOrHide(ui->checkBox_GPSL1M, 3, ui->label_CNo_3, ui->label_AVG_3, ui->label_MAX_3, ui->label_MIN_3);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GPSL2PY_clicked(){
    curveShowOrHide(ui->checkBox_GPSL2PY, 4, ui->label_CNo_4, ui->label_AVG_4, ui->label_MAX_4, ui->label_MIN_4);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GPSL2CM_clicked(){
    curveShowOrHide(ui->checkBox_GPSL2CM, 5, ui->label_CNo_5, ui->label_AVG_5, ui->label_MAX_5, ui->label_MIN_5);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GPSL2CL_clicked(){
    curveShowOrHide(ui->checkBox_GPSL2CL, 6, ui->label_CNo_6, ui->label_AVG_6, ui->label_MAX_6, ui->label_MIN_6);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GPSL5I_clicked(){
    curveShowOrHide(ui->checkBox_GPSL5I, 7, ui->label_CNo_7, ui->label_AVG_7, ui->label_MAX_7, ui->label_MIN_7);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GPSL5Q_clicked(){
    curveShowOrHide(ui->checkBox_GPSL5Q, 8, ui->label_CNo_8, ui->label_AVG_8, ui->label_MAX_8, ui->label_MIN_8);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GLALL_clicked(){
    curveShowOrHide(ui->checkBox_GLALL, 9, ui->label_CNo_9, ui->label_AVG_9, ui->label_MAX_9, ui->label_MIN_9);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GLG1CA_clicked(){
    curveShowOrHide(ui->checkBox_GLG1CA, 10, ui->label_CNo_10, ui->label_AVG_10, ui->label_MAX_10, ui->label_MIN_10);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GLG1P_clicked(){
    curveShowOrHide(ui->checkBox_GLG1P, 11, ui->label_CNo_11, ui->label_AVG_11, ui->label_MAX_11, ui->label_MIN_11);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GLG2CA_clicked(){
    curveShowOrHide(ui->checkBox_GLG2CA, 12, ui->label_CNo_12, ui->label_AVG_12, ui->label_MAX_12, ui->label_MIN_12);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GLG2P_clicked(){
    curveShowOrHide(ui->checkBox_GLG2P, 13, ui->label_CNo_13, ui->label_AVG_13, ui->label_MAX_13, ui->label_MIN_13);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GAALL_clicked(){
    curveShowOrHide(ui->checkBox_GAALL, 14, ui->label_CNo_14, ui->label_AVG_14, ui->label_MAX_14, ui->label_MIN_14);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GaE5a_clicked(){
    curveShowOrHide(ui->checkBox_GaE5a, 15, ui->label_CNo_15, ui->label_AVG_15, ui->label_MAX_15, ui->label_MIN_15);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GaE5b_clicked(){
    curveShowOrHide(ui->checkBox_GaE5b, 16, ui->label_CNo_16, ui->label_AVG_16, ui->label_MAX_16, ui->label_MIN_16);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GaE5ab_clicked(){
    curveShowOrHide(ui->checkBox_GaE5ab, 17, ui->label_CNo_17, ui->label_AVG_17, ui->label_MAX_17, ui->label_MIN_17);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GaE6A_clicked(){
    curveShowOrHide(ui->checkBox_GaE6A, 18, ui->label_CNo_18, ui->label_AVG_18, ui->label_MAX_18, ui->label_MIN_18);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GaE6BC_clicked(){
    curveShowOrHide(ui->checkBox_GaE6BC, 19, ui->label_CNo_19, ui->label_AVG_19, ui->label_MAX_19, ui->label_MIN_19);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GaL1A_clicked(){
    curveShowOrHide(ui->checkBox_GaL1A, 20, ui->label_CNo_20, ui->label_AVG_20, ui->label_MAX_20, ui->label_MIN_20);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GaL1BC_clicked(){
    curveShowOrHide(ui->checkBox_GaL1BC, 21, ui->label_CNo_21, ui->label_AVG_21, ui->label_MAX_21, ui->label_MIN_21);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GBALL_clicked(){
    curveShowOrHide(ui->checkBox_GBALL, 22, ui->label_CNo_22, ui->label_AVG_22, ui->label_MAX_22, ui->label_MIN_22);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB1I_clicked(){
    curveShowOrHide(ui->checkBox_BDSB1I, 23, ui->label_CNo_23, ui->label_AVG_23, ui->label_MAX_23, ui->label_MIN_23);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB1Q_clicked(){
    curveShowOrHide(ui->checkBox_BDSB1Q, 24, ui->label_CNo_24, ui->label_AVG_24, ui->label_MAX_24, ui->label_MIN_24);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB1C_clicked(){
    curveShowOrHide(ui->checkBox_BDSB1C, 25, ui->label_CNo_25, ui->label_AVG_25, ui->label_MAX_25, ui->label_MIN_25);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB1A_clicked(){
    curveShowOrHide(ui->checkBox_BDSB1A, 26, ui->label_CNo_26, ui->label_AVG_26, ui->label_MAX_26, ui->label_MIN_26);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB2a_clicked(){
    curveShowOrHide(ui->checkBox_BDSB2a, 27, ui->label_CNo_27, ui->label_AVG_27, ui->label_MAX_27, ui->label_MIN_27);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB2b_clicked(){
    curveShowOrHide(ui->checkBox_BDSB2b, 28, ui->label_CNo_28, ui->label_AVG_28, ui->label_MAX_28, ui->label_MIN_28);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB2ab_clicked(){
    curveShowOrHide(ui->checkBox_BDSB2ab, 29, ui->label_CNo_29, ui->label_AVG_29, ui->label_MAX_29, ui->label_MIN_29);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB3I_clicked(){
    curveShowOrHide(ui->checkBox_BDSB3I, 30, ui->label_CNo_30, ui->label_AVG_30, ui->label_MAX_30, ui->label_MIN_30);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB3Q_clicked(){
    curveShowOrHide(ui->checkBox_BDSB3Q, 31, ui->label_CNo_31, ui->label_AVG_31, ui->label_MAX_31, ui->label_MIN_31);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB3A_clicked(){
    curveShowOrHide(ui->checkBox_BDSB3A, 32, ui->label_CNo_32, ui->label_AVG_32, ui->label_MAX_32, ui->label_MIN_32);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB2I_clicked(){
    curveShowOrHide(ui->checkBox_BDSB2I, 33, ui->label_CNo_33, ui->label_AVG_33, ui->label_MAX_33, ui->label_MIN_33);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSB2Q_clicked(){
    curveShowOrHide(ui->checkBox_BDSB2Q, 34, ui->label_CNo_34, ui->label_AVG_34, ui->label_MAX_34, ui->label_MIN_34);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQALL_clicked(){
    curveShowOrHide(ui->checkBox_GQALL, 35, ui->label_CNo_35, ui->label_AVG_35, ui->label_MAX_35, ui->label_MIN_35);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL1CA_clicked(){
    curveShowOrHide(ui->checkBox_GQL1CA, 36, ui->label_CNo_36, ui->label_AVG_36, ui->label_MAX_36, ui->label_MIN_36);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL1CD_clicked(){
    curveShowOrHide(ui->checkBox_GQL1CD, 37, ui->label_CNo_37, ui->label_AVG_37, ui->label_MAX_37, ui->label_MIN_37);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL1CP_clicked(){
    curveShowOrHide(ui->checkBox_GQL1CP, 38, ui->label_CNo_38, ui->label_AVG_38, ui->label_MAX_38, ui->label_MIN_38);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQLIS_clicked(){
    curveShowOrHide(ui->checkBox_GQLIS, 39, ui->label_CNo_39, ui->label_AVG_39, ui->label_MAX_39, ui->label_MIN_39);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL2CM_clicked(){
    curveShowOrHide(ui->checkBox_GQL2CM, 40, ui->label_CNo_40, ui->label_AVG_40, ui->label_MAX_40, ui->label_MIN_40);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL2CL_clicked(){
    curveShowOrHide(ui->checkBox_GQL2CL, 41, ui->label_CNo_41, ui->label_AVG_41, ui->label_MAX_41, ui->label_MIN_41);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL5I_clicked(){
    curveShowOrHide(ui->checkBox_GQL5I, 42, ui->label_CNo_42, ui->label_AVG_42, ui->label_MAX_42, ui->label_MIN_42);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL5Q_clicked(){
    curveShowOrHide(ui->checkBox_GQL5Q, 43, ui->label_CNo_43, ui->label_AVG_43, ui->label_MAX_43, ui->label_MIN_43);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL6D_clicked(){
    curveShowOrHide(ui->checkBox_GQL6D, 44, ui->label_CNo_44, ui->label_AVG_44, ui->label_MAX_44, ui->label_MIN_44);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GQL6E_clicked(){
    curveShowOrHide(ui->checkBox_GQL6E, 45, ui->label_CNo_45, ui->label_AVG_45, ui->label_MAX_45, ui->label_MIN_45);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GIALL_clicked(){
    curveShowOrHide(ui->checkBox_GIALL, 46, ui->label_CNo_46, ui->label_AVG_46, ui->label_MAX_46, ui->label_MIN_46);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GIL5SPS_clicked(){
    curveShowOrHide(ui->checkBox_GIL5SPS, 47, ui->label_CNo_47, ui->label_AVG_47, ui->label_MAX_47, ui->label_MIN_47);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GISSPS_clicked(){
    curveShowOrHide(ui->checkBox_GISSPS, 48, ui->label_CNo_48, ui->label_AVG_48, ui->label_MAX_48, ui->label_MIN_48);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GIL5RS_clicked(){
    curveShowOrHide(ui->checkBox_GIL5RS, 49, ui->label_CNo_49, ui->label_AVG_49, ui->label_MAX_49, ui->label_MIN_49);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GISRS_clicked(){
    curveShowOrHide(ui->checkBox_GISRS, 50, ui->label_CNo_50, ui->label_AVG_50, ui->label_MAX_50, ui->label_MIN_50);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GIL1SPS_clicked(){
    curveShowOrHide(ui->checkBox_GIL1SPS, 51, ui->label_CNo_51, ui->label_AVG_51, ui->label_MAX_51, ui->label_MIN_51);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}

void logPlot_MainWindow::on_checkBox_WAAS_clicked(){
    curveShowOrHide(ui->checkBox_WAAS, 52, ui->label_CNo_52, ui->label_AVG_52, ui->label_MAX_52, ui->label_MIN_52);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_SDCM_clicked(){
    curveShowOrHide(ui->checkBox_SDCM, 53, ui->label_CNo_53, ui->label_AVG_53, ui->label_MAX_53, ui->label_MIN_53);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_EGNOS_clicked(){
    curveShowOrHide(ui->checkBox_EGNOS, 54, ui->label_CNo_54, ui->label_AVG_54, ui->label_MAX_54, ui->label_MIN_54);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_BDSBAS_clicked(){
    curveShowOrHide(ui->checkBox_BDSBAS, 55, ui->label_CNo_55, ui->label_AVG_55, ui->label_MAX_55, ui->label_MIN_55);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_MSAS_clicked(){
    curveShowOrHide(ui->checkBox_MSAS, 56, ui->label_CNo_56, ui->label_AVG_56, ui->label_MAX_56, ui->label_MIN_56);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}
void logPlot_MainWindow::on_checkBox_GAGAN_clicked(){
    curveShowOrHide(ui->checkBox_GAGAN, 57, ui->label_CNo_57, ui->label_AVG_57, ui->label_MAX_57, ui->label_MIN_57);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}

void logPlot_MainWindow::on_checkBox_ALL_clicked(){
    setCheckboxStatus();
    setSlotsAssemble();
}

void logPlot_MainWindow::setStyleAllCheckBox()
{
    ui->checkBox_GPALL  ->setStyleSheet(ui->checkBox_GPALL  ->isEnabled()?"color: rgb(85 , 0  , 221)":"color: rgb(170, 170, 170)");
    ui->checkBox_GPSL1CA->setStyleSheet(ui->checkBox_GPSL1CA->isEnabled()?"color: rgb(72 , 61 , 139)":"color: rgb(170, 170, 170)");
    ui->checkBox_GPSL1PY->setStyleSheet(ui->checkBox_GPSL1PY->isEnabled()?"color: rgb(0  , 0  , 205)":"color: rgb(170, 170, 170)");
    ui->checkBox_GPSL1M ->setStyleSheet(ui->checkBox_GPSL1M ->isEnabled()?"color: rgb(30 , 144, 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GPSL2PY->setStyleSheet(ui->checkBox_GPSL2PY->isEnabled()?"color: rgb(100, 149, 237)":"color: rgb(170, 170, 170)");
    ui->checkBox_GPSL2CM->setStyleSheet(ui->checkBox_GPSL2CM->isEnabled()?"color: rgb(0  , 191, 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GPSL2CL->setStyleSheet(ui->checkBox_GPSL2CL->isEnabled()?"color: rgb(135, 206, 250)":"color: rgb(170, 170, 170)");
    ui->checkBox_GPSL5I ->setStyleSheet(ui->checkBox_GPSL5I ->isEnabled()?"color: rgb(85 , 85 , 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GPSL5Q ->setStyleSheet(ui->checkBox_GPSL5Q ->isEnabled()?"color: rgb(173, 216, 230)":"color: rgb(170, 170, 170)");
    ui->checkBox_GLALL  ->setStyleSheet(ui->checkBox_GLALL  ->isEnabled()?"color: rgb(136, 102, 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_GLG1CA ->setStyleSheet(ui->checkBox_GLG1CA ->isEnabled()?"color: rgb(221, 170, 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_GLG1P  ->setStyleSheet(ui->checkBox_GLG1P  ->isEnabled()?"color: rgb(255, 221, 85 )":"color: rgb(170, 170, 170)");
    ui->checkBox_GLG2CA ->setStyleSheet(ui->checkBox_GLG2CA ->isEnabled()?"color: rgb(238, 238, 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_GLG2P  ->setStyleSheet(ui->checkBox_GLG2P  ->isEnabled()?"color: rgb(255, 255, 187)":"color: rgb(170, 170, 170)");
    ui->checkBox_GAALL  ->setStyleSheet(ui->checkBox_GAALL  ->isEnabled()?"color: rgb(0  , 100, 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_GaE5a  ->setStyleSheet(ui->checkBox_GaE5a  ->isEnabled()?"color: rgb(46 , 139, 87 )":"color: rgb(170, 170, 170)");
    ui->checkBox_GaE5b  ->setStyleSheet(ui->checkBox_GaE5b  ->isEnabled()?"color: rgb(60 , 179, 113)":"color: rgb(170, 170, 170)");
    ui->checkBox_GaE5ab ->setStyleSheet(ui->checkBox_GaE5ab ->isEnabled()?"color: rgb(154, 205, 50 )":"color: rgb(170, 170, 170)");
    ui->checkBox_GaE6A  ->setStyleSheet(ui->checkBox_GaE6A  ->isEnabled()?"color: rgb(0  , 255, 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_GaE6BC ->setStyleSheet(ui->checkBox_GaE6BC ->isEnabled()?"color: rgb(124, 252, 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_GaL1A  ->setStyleSheet(ui->checkBox_GaL1A  ->isEnabled()?"color: rgb(173, 255, 47 )":"color: rgb(170, 170, 170)");
    ui->checkBox_GaL1BC ->setStyleSheet(ui->checkBox_GaL1BC ->isEnabled()?"color: rgb(144, 238, 144)":"color: rgb(170, 170, 170)");
    ui->checkBox_GBALL  ->setStyleSheet(ui->checkBox_GBALL  ->isEnabled()?"color: rgb(136, 0  , 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB1I ->setStyleSheet(ui->checkBox_BDSB1I ->isEnabled()?"color: rgb(204, 0  , 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB1Q ->setStyleSheet(ui->checkBox_BDSB1Q ->isEnabled()?"color: rgb(255, 51 , 51 )":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB1C ->setStyleSheet(ui->checkBox_BDSB1C ->isEnabled()?"color: rgb(255, 136, 136)":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB1A ->setStyleSheet(ui->checkBox_BDSB1A ->isEnabled()?"color: rgb(164, 45 , 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB2a ->setStyleSheet(ui->checkBox_BDSB2a ->isEnabled()?"color: rgb(255, 85 , 17 )":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB2b ->setStyleSheet(ui->checkBox_BDSB2b ->isEnabled()?"color: rgb(255, 164, 136)":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB2ab->setStyleSheet(ui->checkBox_BDSB2ab->isEnabled()?"color: rgb(187, 85 , 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB3I ->setStyleSheet(ui->checkBox_BDSB3I ->isEnabled()?"color: rgb(238, 119, 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB3Q ->setStyleSheet(ui->checkBox_BDSB3Q ->isEnabled()?"color: rgb(255, 170, 51 )":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB3A ->setStyleSheet(ui->checkBox_BDSB3A ->isEnabled()?"color: rgb(255, 187, 102)":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB2I ->setStyleSheet(ui->checkBox_BDSB2I ->isEnabled()?"color: rgb(255, 221, 170)":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSB2Q ->setStyleSheet(ui->checkBox_BDSB2Q ->isEnabled()?"color: rgb(255, 204, 204)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQALL  ->setStyleSheet(ui->checkBox_GQALL  ->isEnabled()?"color: rgb(119, 0  , 119)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL1CA ->setStyleSheet(ui->checkBox_GQL1CA ->isEnabled()?"color: rgb(122, 0  , 153)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL1CD ->setStyleSheet(ui->checkBox_GQL1CD ->isEnabled()?"color: rgb(255, 62 , 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL1CP ->setStyleSheet(ui->checkBox_GQL1CP ->isEnabled()?"color: rgb(204, 0  , 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQLIS  ->setStyleSheet(ui->checkBox_GQLIS  ->isEnabled()?"color: rgb(227, 142, 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL2CM ->setStyleSheet(ui->checkBox_GQL2CM ->isEnabled()?"color: rgb(85 , 0  , 136)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL2CL ->setStyleSheet(ui->checkBox_GQL2CL ->isEnabled()?"color: rgb(153, 0  , 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL5I  ->setStyleSheet(ui->checkBox_GQL5I  ->isEnabled()?"color: rgb(210, 142, 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL5Q  ->setStyleSheet(ui->checkBox_GQL5Q  ->isEnabled()?"color: rgb(232, 204, 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL6D  ->setStyleSheet(ui->checkBox_GQL6D  ->isEnabled()?"color: rgb(159, 136, 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GQL6E  ->setStyleSheet(ui->checkBox_GQL6E  ->isEnabled()?"color: rgb(153, 153, 255)":"color: rgb(170, 170, 170)");
    ui->checkBox_GIALL  ->setStyleSheet(ui->checkBox_GIALL  ->isEnabled()?"color: rgb(140, 0  , 68 )":"color: rgb(170, 170, 170)");
    ui->checkBox_GIL5SPS->setStyleSheet(ui->checkBox_GIL5SPS->isEnabled()?"color: rgb(255, 183, 221)":"color: rgb(170, 170, 170)");
    ui->checkBox_GISSPS ->setStyleSheet(ui->checkBox_GISSPS ->isEnabled()?"color: rgb(162, 0  , 85 )":"color: rgb(170, 170, 170)");
    ui->checkBox_GIL5RS ->setStyleSheet(ui->checkBox_GIL5RS ->isEnabled()?"color: rgb(255, 0  , 136)":"color: rgb(170, 170, 170)");
    ui->checkBox_GISRS  ->setStyleSheet(ui->checkBox_GISRS  ->isEnabled()?"color: rgb(255, 68 , 170)":"color: rgb(170, 170, 170)");
    ui->checkBox_GIL1SPS->setStyleSheet(ui->checkBox_GIL1SPS->isEnabled()?"color: rgb(255, 136, 194)":"color: rgb(170, 170, 170)");
    ui->checkBox_WAAS   ->setStyleSheet(ui->checkBox_WAAS   ->isEnabled()?"color: rgb(0  , 0  , 205)":"color: rgb(170, 170, 170)");
    ui->checkBox_SDCM   ->setStyleSheet(ui->checkBox_SDCM   ->isEnabled()?"color: rgb(255, 215, 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_EGNOS  ->setStyleSheet(ui->checkBox_EGNOS  ->isEnabled()?"color: rgb(143, 188, 143)":"color: rgb(170, 170, 170)");
    ui->checkBox_BDSBAS ->setStyleSheet(ui->checkBox_BDSBAS ->isEnabled()?"color: rgb(255, 0  , 0  )":"color: rgb(170, 170, 170)");
    ui->checkBox_MSAS   ->setStyleSheet(ui->checkBox_MSAS   ->isEnabled()?"color: rgb(199, 21 , 133)":"color: rgb(170, 170, 170)");
    ui->checkBox_GAGAN  ->setStyleSheet(ui->checkBox_GAGAN  ->isEnabled()?"color: rgb(128, 0  , 128)":"color: rgb(170, 170, 170)");
}

void logPlot_MainWindow::on_actionline_Chart_triggered(){
    if(!x_dataList.isEmpty()&&plotFlag){
        QWaiting *pQwait = new QWaiting(this);
        pQwait->show();
        plotFlag = false;
        showPlot();
        pQwait->close();
    }
    plotFlag = false;
}

void logPlot_MainWindow::on_actionScatter_Plot_triggered(){
    if(!x_dataList.isEmpty()&&(!plotFlag)){
        QWaiting *pQwait = new QWaiting(this);
        pQwait->show();
        plotFlag = true;
        showPlot();
        pQwait->close();
    }
    plotFlag = true;
}

void logPlot_MainWindow::on_actionPDF_triggered()
{
    QString filename = QFileDialog::getSaveFileName();
    if( filename == "" ){
        QMessageBox::information(this,"Failed to save","Please check whether a valid file name is entered!");
    }else if( filename.endsWith(".pdf") ){
        QMessageBox::information(this,"Saved successfully","The file is saved as a PDF file.");
        ui->customPlot->savePdf( filename, ui->customPlot->width(), ui->customPlot->height() );
    }else{
        QMessageBox::information(this,"Saved successfully","The file is saved as a PDF file by default.");
        ui->customPlot->savePdf( filename.append(".pdf"), ui->customPlot->width(), ui->customPlot->height() );
    }
}

void logPlot_MainWindow::on_actionBMP_triggered()
{
    QString filename = QFileDialog::getSaveFileName();
    if( filename == "" ){
        QMessageBox::information(this,"Failed to save","Please check whether a valid file name is entered!");
    }else if( filename.endsWith(".bmp") ){
        QMessageBox::information(this,"Saved successfully","The file is saved as a BMP file.");
        ui->customPlot->saveBmp( filename, ui->customPlot->width(), ui->customPlot->height() );
    }else{
        QMessageBox::information(this,"Saved successfully","The file is saved as a BMP file by default.");
        ui->customPlot->saveBmp( filename.append(".bmp"), ui->customPlot->width(), ui->customPlot->height() );
    }
}

void logPlot_MainWindow::on_actionJPG_triggered()
{
    QString filename = QFileDialog::getSaveFileName();
    if( filename == "" ){
        QMessageBox::information(this,"Failed to save","Please check whether a valid file name is entered!");
    }else if( filename.endsWith(".jpg")||filename.endsWith(".jpeg") ){
        QMessageBox::information(this,"Saved successfully","The file is saved as a JPG file.");
        ui->customPlot->saveJpg( filename, ui->customPlot->width(), ui->customPlot->height() );
    }else{
        QMessageBox::information(this,"Saved successfully","The file is saved as a JPG file by default.");
        ui->customPlot->saveJpg( filename.append(".jpg"), ui->customPlot->width(), ui->customPlot->height() );
    }
}

void logPlot_MainWindow::on_actionPNG_triggered()
{
    QString filename = QFileDialog::getSaveFileName();
    if( filename == "" ){
        QMessageBox::information(this,"Failed to save","Please check whether a valid file name is entered!");
    }else if( filename.endsWith(".png") ){
        QMessageBox::information(this,"Saved successfully","The file is saved as a PNG file.");
        ui->customPlot->savePng( filename, ui->customPlot->width(), ui->customPlot->height() );
    }else{
        QMessageBox::information(this,"Saved successfully","The file is saved as a PNG file by default.");
        ui->customPlot->savePng( filename.append(".png"), ui->customPlot->width(), ui->customPlot->height() );
    }
}

void logPlot_MainWindow::on_actionPDF_2_triggered()
{
    if(fileName_save!=""){
        QDateTime datetime;
        QString timestr=datetime.currentDateTime().toString("HH-mm-ss");
        QString fileName;
        QDir dir1("./");
        if (!dir1.exists("./pdfFile")){
            dir1.mkdir("./pdfFile");
        }
        fileName = QString("./pdfFile/%1_%2.pdf").arg(fileName_save).arg(timestr);
        QString fileName1 = QString("/pdfFile/%1_%2.pdf").arg(fileName_save).arg(timestr);
        if (QFileInfo(fileName).suffix().isEmpty()){
            fileName.append(".pdf");
        }
        ui->customPlot->savePdf( fileName, ui->customPlot->width(), ui->customPlot->height() );
        QString path = QDir::currentPath()+fileName1;
        QFile file(path);
        if(file.exists()){
            QMessageBox::information(this,"Save Success",path+" PDF File saved successfully.");
        }else{
            QMessageBox::information(this,"Save Failed","Failed to save the file, please check if the path is correct!");
        }
    }
}

void logPlot_MainWindow::on_actionBMP_2_triggered()
{
    if(fileName_save!=""){
        QDateTime datetime;
        QString timestr=datetime.currentDateTime().toString("HH-mm-ss");
        QString fileName;
        QDir dir1("./");
        if (!dir1.exists("./bmpFile")){
            dir1.mkdir("./bmpFile");
        }
        fileName = QString("./bmpFile/%1_%2.bmp").arg(fileName_save).arg(timestr);
        QString fileName1 = QString("/bmpFile/%1_%2.bmp").arg(fileName_save).arg(timestr);
        if (QFileInfo(fileName).suffix().isEmpty()){
            fileName.append(".bmp");
        }
        ui->customPlot->saveBmp( fileName, ui->customPlot->width(), ui->customPlot->height() );
        QString path = QDir::currentPath()+fileName1;
        QFile file(path);
        if(file.exists()){
            QMessageBox::information(this,"Save Success",path+" BMP File saved successfully.");
        }else{
            QMessageBox::information(this,"Save Failed","Failed to save the file, please check if the path is correct!");
        }
    }
}

void logPlot_MainWindow::on_actionJPG_2_triggered()
{
    if(fileName_save!=""){
        QDateTime datetime;
        QString timestr=datetime.currentDateTime().toString("HH-mm-ss");
        QString fileName;
        QDir dir1("./");
        if (!dir1.exists("./jpgFile")){
            dir1.mkdir("./jpgFile");
        }
        fileName = QString("./jpgFile/%1_%2.jpg").arg(fileName_save).arg(timestr);
        QString fileName1 = QString("/jpgFile/%1_%2.jpg").arg(fileName_save).arg(timestr);
        if (QFileInfo(fileName).suffix().isEmpty()){
            fileName.append(".jpg");
        }
        ui->customPlot->saveJpg( fileName, ui->customPlot->width(), ui->customPlot->height() );
        QString path = QDir::currentPath()+fileName1;
        QFile file(path);
        if(file.exists()){
            QMessageBox::information(this,"Save Success",path+" JPG File saved successfully.");
        }else{
            QMessageBox::information(this,"Save Failed","Failed to save the file, please check if the path is correct!");
        }
    }
}

void logPlot_MainWindow::on_actionPNG_2_triggered()
{
    if(fileName_save!=""){
        QDateTime datetime;
        QString timestr=datetime.currentDateTime().toString("HH-mm-ss");
        QString fileName;
        QDir dir1("./");
        if (!dir1.exists("./pngFile")){
            dir1.mkdir("./pngFile");
        }
        fileName = QString("./pngFile/%1_%2.png").arg(fileName_save).arg(timestr);
        QString fileName1 = QString("/pngFile/%1_%2.png").arg(fileName_save).arg(timestr);
        if (QFileInfo(fileName).suffix().isEmpty()){
            fileName.append(".png");
        }
        ui->customPlot->savePng( fileName, ui->customPlot->width(), ui->customPlot->height() );
        QString path = QDir::currentPath()+fileName1;
        QFile file(path);
        if(file.exists()){
            QMessageBox::information(this,"Save Success",path+" PNG File saved successfully.");
        }else{
            QMessageBox::information(this,"Save Failed","Failed to save the file, please check if the path is correct!");
        }
    }
}

void logPlot_MainWindow::on_checkBox_BlackLine_clicked()
{
    if(!x_dataList.isEmpty()){
        ui->checkBox_BlackLine->isChecked()?  ui->customPlot->graph(58)->setVisible(true):ui->customPlot->graph(58)->setVisible(false);
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void logPlot_MainWindow::on_checkBox_Status_clicked()
{
    if(!x_dataList.isEmpty()){
        ui->checkBox_Status->isChecked()?  ui->customPlot->graph(59)->setVisible(true):ui->customPlot->graph(59)->setVisible(false);
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void logPlot_MainWindow::on_checkBox_GrandMean_clicked()
{
    if(!x_dataList.isEmpty()){
        ui->checkBox_GrandMean->isChecked()?  ui->customPlot->graph(60)->setVisible(true):ui->customPlot->graph(60)->setVisible(false);
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void logPlot_MainWindow::on_actionHelp_triggered()
{
    infoDialog *pAbout = new infoDialog(this);
    pAbout->show();
}

void logPlot_MainWindow::on_actionFilter_options_triggered()
{
    FilterOptions->show();
}
