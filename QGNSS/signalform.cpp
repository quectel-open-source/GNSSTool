#include "signalform.h"
#include "ui_signalform.h"
#include "QPainter"
#include "mainwindow.h"

SignalForm::SignalForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignalForm)
{
    ui->setupUi(this);
    ui->BAND_Label->setText(" ");
    ui->AZI_Label->setText(" ");
    ui->ELE_Label->setText(" ");
    ui->PRN_Label->setText(" ");
    ui->display_Label->installEventFilter(this);
}

SignalForm::~SignalForm()
{
    delete ui;
}

//void SignalForm::drowFlag(const SAT_Info_t info,const NATIONAL national,const QString &band)
//{
//    this->svsinfo = info;
//    this->national = national;
//    //Special case
//    if(mainForm->dev_info.stringModelType == "LG69TAA"){
//        if(intInRange(401,432,svsinfo.nPRN)){
//            ui->BAND_Label->setText("L2C");
//        }
//        else if(intInRange(465,492,svsinfo.nPRN)){
//            ui->BAND_Label->setText("G2");
//        }
//        else if(intInRange(541,577,svsinfo.nPRN)){
//            ui->BAND_Label->setText("B2i");
//        }
//        else if(intInRange(501,532,svsinfo.nPRN)){
//            ui->BAND_Label->setText("L5");
//        }
//        else if(intInRange(601,636,svsinfo.nPRN)){
//            ui->BAND_Label->setText("E5a");
//        }
//        else if(intInRange(651,685,svsinfo.nPRN)){
//            ui->BAND_Label->setText("E5b");
//        }
//        else if(intInRange(203,212,svsinfo.nPRN)){
//            ui->BAND_Label->setText("L2C");
//        }
//        else if(intInRange(213,222,svsinfo.nPRN)){
//            ui->BAND_Label->setText("I5C");
//        }
//        else{
//            ui->BAND_Label->setText(band);
//        }//Special case --end
//    }else{
//        ui->BAND_Label->setText(band);
//    }
//    ui->AZI_Label->setText(svsinfo.dAzimuth>0?QString::number(svsinfo.dAzimuth):"-");
//    ui->ELE_Label->setText(svsinfo.dElevation>0?QString::number(svsinfo.dElevation):"-");
//    ui->PRN_Label->setText(QString::number(svsinfo.nPRN));
//    update();
//}

void SignalForm::drowSat(const SignalInfo_t sat)
{
    this->SatelliteInfo = sat;
    ui->AZI_Label->setText(SatelliteInfo.Azimuth>0?QString::number(SatelliteInfo.Azimuth):"-");
    ui->ELE_Label->setText(SatelliteInfo.Elevation>0?QString::number(SatelliteInfo.Elevation):"-");
    ui->PRN_Label->setText(QString::number(SatelliteInfo.PRN));
    update();
}

void SignalForm::clearData()
{
    this->SatelliteInfo.SNR = 0;
    ui->BAND_Label->setText(" ");
    ui->AZI_Label->setText(" ");
    ui->ELE_Label->setText(" ");
    ui->PRN_Label->setText(" ");
    update();
}

bool SignalForm::eventFilter(QObject *watched, QEvent *event)
{
    if(watched ==ui->display_Label && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->display_Label);
        QPixmap pix;
        QString freq = "-";
        int labelHeight = ui->display_Label->height();
        if(SatelliteInfo.SNR > 0){
            switch (SatelliteInfo.Sys) {
            case SatSys::All:
//                pix.load(":/images/longUSA.jpg");
                break;
            case SatSys::GPS:
                pix.load(":/images/longUSA.jpg");
                if (SatelliteInfo.Channel > 100 && SatelliteInfo.Channel <= 132)
                    freq = Freq_sig_gps[SatelliteInfo.Channel - Q_FREQ_OFFSET - 1];
                else
                    freq = gpNumOfBaud[SatelliteInfo.Channel];
                break;
            case SatSys::BeiDou:
                pix.load(":/images/longChina.jpg");
                if (SatelliteInfo.Channel > 100 && SatelliteInfo.Channel <= 132)
                    freq = Freq_sig_cmp[SatelliteInfo.Channel - Q_FREQ_OFFSET - 1];
                else
                    freq = gbNumOfBaud[SatelliteInfo.Channel];
                break;
            case SatSys::Galileo:
                pix.load(":/images/longEurope.jpg");
                if (SatelliteInfo.Channel > 100 && SatelliteInfo.Channel <= 132)
                    freq = Freq_sig_gal[SatelliteInfo.Channel - Q_FREQ_OFFSET - 1];
                else
                    freq = gaNumOfBaud[SatelliteInfo.Channel];
                ui->BAND_Label->setText(gaNumOfBaud[SatelliteInfo.Channel]);
                break;
            case SatSys::GLONASS:
                pix.load(":/images/longRussia.jpg");
                if (SatelliteInfo.Channel > 100 && SatelliteInfo.Channel <= 132)
                    freq = Freq_sig_glo[SatelliteInfo.Channel - Q_FREQ_OFFSET - 1];
                else
                    freq = glNumOfBaud[SatelliteInfo.Channel];
                ui->BAND_Label->setText(glNumOfBaud[SatelliteInfo.Channel]);
                break;
            case SatSys::QZSS:
                pix.load(":/images/longJapan.jpg");
                if (SatelliteInfo.Channel > 100 && SatelliteInfo.Channel <= 132)
                    freq = Freq_sig_qzs[SatelliteInfo.Channel - Q_FREQ_OFFSET - 1];
                else
                    freq = gqNumOfBaud[SatelliteInfo.Channel];
                ui->BAND_Label->setText(gqNumOfBaud[SatelliteInfo.Channel]);
                break;
            case SatSys::IRNSS:
                pix.load(":/images/longIndia.jpg");
                if (SatelliteInfo.Channel > 100 && SatelliteInfo.Channel <= 132)
                    freq = Freq_sig_irn[SatelliteInfo.Channel - Q_FREQ_OFFSET - 1];
                else
                    freq = giNumOfBaud[SatelliteInfo.Channel];
                break;
            case SatSys::SBAS:
                pix.load(":/images/longSBAS.jpg");
                if (SatelliteInfo.Channel > 100 && SatelliteInfo.Channel <= 132)
                    freq = Freq_sig_sbs[SatelliteInfo.Channel - Q_FREQ_OFFSET - 1];
                else
                    freq = gpNumOfBaud[SatelliteInfo.Channel];
                break;
//            case SatSys::GLONASS:
//                pix.load(":/images/longGLO.jpg");
//                break;
            }
            ui->BAND_Label->setText(freq);
            painter.drawText(4,labelHeight-4-static_cast<int>(SatelliteInfo.SNR/60.0*labelHeight),QString::number(SatelliteInfo.SNR));
            if(!SatelliteInfo.usedInSolution)
                painter.setOpacity(0.3);
            painter.drawPixmap(0,labelHeight-static_cast<int>(SatelliteInfo.SNR/60.0*labelHeight),pix);
        }
        return true;
    }
    return false;
}
