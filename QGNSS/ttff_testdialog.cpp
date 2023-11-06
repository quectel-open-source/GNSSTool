#include "ttff_testdialog.h"
#include "ui_ttff_testdialog.h"


TTFF_TestDialog::TTFF_TestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TTFF_TestDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("TTFF");
    fTimer = new QTimer(this);
    fTimer->stop();
    fTimer->setInterval(50);
    connect(fTimer,SIGNAL(timeout()),this,SLOT(check_postion_timer()));

    auto graph = ui->customPlot->addGraph();
    graph->setPen(QPen(QColor(Qt::blue)));
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
}

TTFF_TestDialog::~TTFF_TestDialog()
{
    delete ui;
    delete fTimer;
}

void TTFF_TestDialog::on_pushButton_Run_clicked(bool checked)
{
    if(checked){
//        QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
        ui->pushButton_Run->setText("Stop");
        setEnabledUI(false);
        clearUI();
        NumOfTests = ui->lineEdit_NumOfTests->text().toUInt();
        Timeout_ms = ui->lineEdit_Timeout->text().toUInt()*1000;
        maxFixTime = 0;
        minFixTime = Timeout_ms;
        sumFixTime = 0;
        TestedCount = 0;
        invalidFixTime = 0;
        step = 0;
        ui->customPlot->xAxis->setRange(1,NumOfTests);
//        series->setName("MAX:--MIN:--MEAN:--");
//        if(NumOfTests > 20)
//        {
//            axisX->setRange(1,20);
//            axisX->setTickCount(20);
//        }else{
//            axisX->setRange(1,NumOfTests);
//            axisX->setTickCount(NumOfTests);
//        }
        fTimer->start(100);
        QString file_name;
        QDateTime time = QDateTime::currentDateTime();
        file_name = "TTFF_" + time.toString("MMddhhmmss_") + mainForm->portName+".log";
        ttffLogFile = new QFile("./logFile/" + file_name);
    }
    else{
        ui->pushButton_Run->setText("Run");
        fTimer->stop();
//        TestedCount = 0;
//        step = 0;
        setEnabledUI(true);
    }
}

void TTFF_TestDialog::check_postion_timer()
{
    switch (step) {
    case 0:
    {
        if(NumOfTests){
//            fTimeCounter.restart();
            sendRestart();
            NumOfTests--;
            TestedCount++;
            step = 1;
//            if(ui->checkBox_use_AGNSS->isChecked()){
//                emit sendAgnssData();
//            }
            fTimeCounter.restart();
        }
        else{
            ui->pushButton_Run->click();
        }
        break;
    }
    case 1:
    {
        if(solution_data != nullptr){
            bool pass = false;
            int fixTime = fTimeCounter.elapsed();
            if(fixTime > Timeout_ms){
                fixTime = Timeout_ms;
                pass = true;
            }
            if(solution_data->savedData_L.size() == 2 || pass)
            {
                if(mainForm->NMEA_Data.lastTTFF.has_value())
                    fixTime = mainForm->NMEA_Data.lastTTFF.value();
                else
                    fixTime = fTimeCounter.elapsed();
                if(fixTime < 200){
                    fixTime = 200;
                    invalidFixTime++;
                }
                else
                    sumFixTime += fixTime;
                if(fixTime > maxFixTime)
                {
//                    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
                    maxFixTime = fixTime;
//                    axisY->setRange(0,maxFixTime/1000.0+6);
                }
                if(TestedCount > 20){
//                    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
//                    axisX->setRange(TestedCount-17,TestedCount+2);
//                    axisX->setTickCount(20);
                }
                if(fixTime < minFixTime && fixTime > 200)
                    minFixTime = fixTime;
//                series->append(TestedCount,fixTime/1000.0);
                ui->customPlot->graph(0)->addData(TestedCount,fixTime/1000.0);
                ui->customPlot->replot();
                ui->customPlot->graph(0)->rescaleValueAxis();
                step = 0;
                QString str = QString().sprintf(
                            "MAX:%.2f    MIN:%.2f    MEAN:%.2f"
                                ,maxFixTime/1000.0
                                ,minFixTime/1000.0
                                ,sumFixTime/1000.0/(TestedCount-invalidFixTime));
//                series->setName(str);

                ui->lineEdit_Max->setText(QString::number(maxFixTime/1000.0,'f',2));
                ui->lineEdit_Avg->setText(QString::number(sumFixTime/1000.0/(TestedCount-invalidFixTime),'f',2));
                ui->lineEdit_Min->setText(QString::number(minFixTime/1000.0,'f',2));

                str = QString().sprintf("%4d    %5.2f   %1",TestedCount,fixTime/1000.0).arg(str);
                if (ttffLogFile->open(QFile::Append))
                {
                    ttffLogFile->write(str.toLatin1());
                    ttffLogFile->write("\r\n");
                    ttffLogFile->close();
                }
                qgnss_sleep(3000);
            }
        }
        break;
    }
    default:
        break;
    }
}

bool TTFF_TestDialog::sendRestart()
{
    mainForm->sendResetCMD(rType);
    return true;
}

void TTFF_TestDialog::setEnabledUI(bool en)
{
    ui->radioButton_hot->setEnabled(en);
    ui->radioButton_cold->setEnabled(en);
    ui->radioButton_full->setEnabled(en);
    ui->radioButton_warm->setEnabled(en);
    ui->lineEdit_Timeout->setEnabled(en);
    ui->lineEdit_NumOfTests->setEnabled(en);
}

void TTFF_TestDialog::clearUI()
{
    ui->lineEdit_Max->clear();
    ui->lineEdit_Avg->clear();
    ui->lineEdit_Min->clear();
    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->replot();
}

void TTFF_TestDialog::setSolution_data(Solution_t *value)
{
    solution_data = value;
}

void TTFF_TestDialog::on_radioButton_cold_clicked(bool checked)
{
    if(checked)
        rType = RestartType::Cold;
}

void TTFF_TestDialog::on_radioButton_warm_clicked(bool checked)
{
    if(checked)
        rType = RestartType::Warm;
}

void TTFF_TestDialog::on_radioButton_hot_clicked(bool checked)
{
    if(checked)
        rType = RestartType::Hot;
}

void TTFF_TestDialog::on_radioButton_full_clicked(bool checked)
{
    if(checked)
        rType = RestartType::FullCold;
}

void TTFF_TestDialog::on_pushButton_SetAGNSS_clicked()
{
    emit openSetAGNSS_Dialog();
}
