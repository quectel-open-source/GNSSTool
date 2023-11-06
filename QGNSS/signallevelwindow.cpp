#include "signallevelwindow.h"
#include "ui_signallevelwindow.h"
#include "signalform.h"
#include "qdebug.h"
#include "DataParser/rtcm3parser.h"

SignalLevelWindow::SignalLevelWindow(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::SignalLevelWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Signal Level");
    setWindowIcon(QIcon(":/images/gnssSignal.png"));
    SignalForm *tmp;
    for (int i = 0; i < MAX_SIGNAL; i++)
    {
        tmp = new SignalForm(ui->scrollArea);
        ui->horizontalLayout_14->addWidget(tmp); //,Qt::AlignBottom
    }

    ui->DispalyGroup->setExclusive(false);
    connect(ui->DispalyGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            [=](QAbstractButton *button)
            {
                displaySet.clear();
                if (ui->checkBox_DspBDS->isChecked())
                    this->displaySet.insert(SatSys::BeiDou);
                if (ui->checkBox_DspGPS->isChecked())
                    this->displaySet.insert(SatSys::GPS);
                if (ui->checkBox_DspQZSS->isChecked())
                    this->displaySet.insert(SatSys::QZSS);
                if (ui->checkBox_DspSBAS->isChecked())
                    this->displaySet.insert(SatSys::SBAS);
                if (ui->checkBox_DspNAVIC->isChecked())
                    this->displaySet.insert(SatSys::IRNSS);
                if (ui->checkBox_DspGALILEO->isChecked())
                    this->displaySet.insert(SatSys::Galileo);
                if (ui->checkBox_DspGLONASS->isChecked())
                    this->displaySet.insert(SatSys::GLONASS);
                qDebug() << "displaySet " << displaySet;
            });
    ui->checkBox_DspQZSS->setChecked(true);
    ui->checkBox_DspBDS->setChecked(true);
    ui->checkBox_DspGPS->setChecked(true);
    ui->checkBox_DspSBAS->setChecked(true);
    ui->checkBox_DspNAVIC->setChecked(true);
    ui->checkBox_DspGALILEO->setChecked(true);
    ui->checkBox_DspGLONASS->setChecked(true);
}

SignalLevelWindow::~SignalLevelWindow()
{
    delete ui;
}

//void SignalLevelWindow::updateGNSSView(GSV_t &gsv_t)
//{
    //    if(gsv_t.received || gsv_t.timer.elapsed() > 80)
    //    if(1)
    //    {
    //        NATIONAL national;
    //        QString freq = 0;
    //        QList<SignalForm*> signalFormList = ui->scrollAreaWidgetContents->findChildren<SignalForm*>();
    //        if(ui->checkBox_DspGPS->isChecked()){
    //            national = USA;
    //            foreach (SAT_Info_t var, gsv_t.gpGSV_Data->signal_L) {
    //                if(!var.nSNR) continue;
    //                if(var.nChannel>100 && var.nChannel<=132){
    //                    freq = Freq_sig_gps[var.nChannel - Q_FREQ_OFFSET - 1];
    //                }else{
    //                    freq = gpNumOfBaud[var.nChannel];
    //                }
    //                signalFormList.takeFirst()->drowFlag(var,national,freq);
    //            }
    //        }
    //        if(ui->checkBox_DspSBAS->isChecked()){
    //            national = SBAS;
    //            foreach (SAT_Info_t var, gsv_t.gsGSV_Data->signal_L) {
    //                if(!var.nSNR) continue;
    //                if(var.nChannel>100 && var.nChannel<=132){
    //                    freq = Freq_sig_gps[var.nChannel - Q_FREQ_OFFSET - 1];
    //                }else{
    //                    freq = gpNumOfBaud[var.nChannel];
    //                }
    //                signalFormList.takeFirst()->drowFlag(var,national,freq);
    //            }
    //        }
    //        if(ui->checkBox_DspQZSS->isChecked()){
    //            national = JAPAN;
    //            foreach (SAT_Info_t var, gsv_t.gqGSV_Data->signal_L) {
    //                if(!var.nSNR) continue;
    //                if(var.nChannel>100 && var.nChannel<=132){
    //                    freq = Freq_sig_qzs[var.nChannel - Q_FREQ_OFFSET - 1];
    //                }else{
    //                    freq = gqNumOfBaud[var.nChannel];
    //                }
    //                signalFormList.takeFirst()->drowFlag(var,national,freq);
    //            }
    //        }
    //        if(ui->checkBox_DspGLONASS->isChecked()){
    //            national = RUSSIA;
    //            foreach (SAT_Info_t var, gsv_t.glGSV_Data->signal_L) {
    //                if(!var.nSNR) continue;
    //                if(var.nChannel>100 && var.nChannel<=132){
    //                    freq = Freq_sig_glo[var.nChannel - Q_FREQ_OFFSET - 1];
    //                }else{
    //                    freq = glNumOfBaud[var.nChannel];
    //                }
    //                signalFormList.takeFirst()->drowFlag(var,national,freq);
    //            }
    //        }
    //        if(ui->checkBox_DspBDS->isChecked()){
    //            national = CHINA;
    //            foreach (SAT_Info_t var, gsv_t.gbGSV_Data->signal_L) {
    //                if(!var.nSNR) continue;
    //                if(var.nChannel > 100 && var.nChannel<=132){
    //                    freq = Freq_sig_cmp[var.nChannel - Q_FREQ_OFFSET - 1];
    //                }else{
    //                    freq = gbNumOfBaud[var.nChannel];
    //                }
    //                signalFormList.takeFirst()->drowFlag(var,national,freq);
    //            }
    //        }
    //        if(ui->checkBox_DspGALILEO->isChecked()){
    //            national = EUROPE;
    //            foreach (SAT_Info_t var, gsv_t.gaGSV_Data->signal_L) {
    //                if(!var.nSNR) continue;
    //                if(var.nChannel > 100 && var.nChannel<=132){
    //                    freq = Freq_sig_gal[var.nChannel - Q_FREQ_OFFSET - 1];
    //                }else{
    //                    freq = gaNumOfBaud[var.nChannel];
    //                }
    //                signalFormList.takeFirst()->drowFlag(var,national,freq);
    //            }
    //        }
    //        if(ui->checkBox_DspNAVIC->isChecked()){
    //            national = INDIA;
    //            foreach (SAT_Info_t var, gsv_t.giGSV_Data->signal_L) {
    //                if(!var.nSNR) continue;
    //                if(var.nChannel > 100 && var.nChannel<=132){
    //                    freq = Freq_sig_irn[var.nChannel - Q_FREQ_OFFSET - 1];
    //                }else{
    //                    freq = giNumOfBaud[var.nChannel];
    //                }
    //                signalFormList.takeFirst()->drowFlag(var,national,freq);
    //            }
    //        }
    //        QString band = "-";
    //        foreach (SAT_Info_t var, gsv_t.gnGSV_Data->signal_L) {
    //            if(!var.nSNR) continue;
    //            band = "-";
    //            if(!prn2National(var, national, band)) continue;
    //            signalFormList.takeFirst()->drowFlag(var,national,band);
    //        }

    //        while(!signalFormList.isEmpty())
    //        {
    //            signalFormList.takeFirst()->clearData();
    //        }
    //    }
//}

// bool SignalLevelWindow::prn2National(SAT_Info_t var, NATIONAL &national, QString &signalFreq)
//{
//     if(intInRange(1, 32, var.nPRN)){
//         national = USA;
//         signalFreq = gpNumOfBaud[var.nChannel];
//         return ui->checkBox_DspGPS->isChecked();
//     }
//     else if(intInRange(33, 51, var.nPRN)){
//         national = SBAS;
//         signalFreq = gpNumOfBaud[var.nChannel];
//         return ui->checkBox_DspSBAS->isChecked();
//     }
//     else if(intInRange(65, 92, var.nPRN)){
//         national = RUSSIA;
//         signalFreq = glNumOfBaud[var.nChannel];
//         return ui->checkBox_DspGLONASS->isChecked();
//     }
//     else if(intInRange(301, 336, var.nPRN)){
//         national = EUROPE;
//         signalFreq = gaNumOfBaud[var.nChannel];
//         return ui->checkBox_DspGALILEO->isChecked();
//     }
//     else if(intInRange(141, 172, var.nPRN)){
//         national = CHINA;
//         signalFreq = gbNumOfBaud[var.nChannel];
//         return ui->checkBox_DspBDS->isChecked();
//     }
//     else if(intInRange(183, 202, var.nPRN)){
//         national = JAPAN;
//         signalFreq = gqNumOfBaud[var.nChannel];
//         return ui->checkBox_DspQZSS->isChecked();
//     }
//     else if(intInRange(801, 807, var.nPRN)){
//         national = INDIA;
//         signalFreq = giNumOfBaud[var.nChannel];
//         return ui->checkBox_DspNAVIC->isChecked();
//     }
//     else{
//         national = GLOBAL;
//         return true;
//     }
// }

void SignalLevelWindow::updateGNSSView(Solution_t &sol)
{
    QList<SignalForm *> signalFormList = ui->scrollAreaWidgetContents->findChildren<SignalForm *>();
    QString freq = 0;
    foreach (auto var, sol.SatInfo)
    {
        if (!displaySet.contains(var.Sys))
            continue;
        if (!var.SNR)
            continue;
//        if (var.Channel > 100 && var.Channel <= 132)
//        {
//            freq = Freq_sig_gps[var.Channel - Q_FREQ_OFFSET - 1];
//        }
//        else
//        {
//            freq = gpNumOfBaud[var.Channel];
//        }
        if(!signalFormList.isEmpty())
            signalFormList.takeFirst()->drowSat(var);
    }
    while (!signalFormList.isEmpty())
    {
        signalFormList.takeFirst()->clearData();
    }
}
