/*==================================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN(yyyy/mm/dd)    WHO         WHAT, WHERE, WHY
------------        -------     ----------------------------------------------------
2020/11/23          wise.hu     Add FTP file not completely downloaded from FTP server handle
===================================================================================*/


#include "FtpClientDialog.h"
#include "ui_FtpClientDialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include "mainwindow.h"
#include "AGNSS/LC79D_AGNSS_LTO.h"
#include <time.h>
#include "mainwindow.h"
#include "bream.h"
#include "bream_handler.h"

using namespace quectel_gnss;

FtpClientDialog::FtpClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FtpClientDialog),
    statusBar(new QStatusBar(this))
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_QuitOnClose, false);

    m_pFtp = nullptr;
    m_pProgressDialog = nullptr;

    ui->fileList->setEnabled(false);
    ui->fileList->setRootIsDecorated(false);
    ui->fileList->setHeaderLabels(QStringList() << tr("Name") << tr("Size") << tr("Owner") << tr("Group") << tr("Time"));

    ui->fileList->header()->setStretchLastSection(true);
    ui->fileList->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->connectButton   ->setDefault(true);
    ui->cdToParentButton       ->setVisible(false);
    ui->downloadToModule_Button->setVisible(true); //TODO:set to true after add file download to module function
    ui->cdToParentButton       ->setEnabled(false);
    ui->downloadButton         ->setEnabled(false);

    ui->downloadToModule_progressBar->setValue(0);
    ui->downloadToModule_progressBar->setVisible(false);

    connect(ui->fileList,         SIGNAL(itemActivated(QTreeWidgetItem*,int)),                   this, SLOT(processItem(QTreeWidgetItem*, int)));
    connect(ui->fileList,         SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(enableDownloadButton()));
    connect(ui->connectButton,    SIGNAL(clicked()),                                             this, SLOT(connectOrDisconnect()));
    connect(ui->cdToParentButton, SIGNAL(clicked()),                                             this, SLOT(cdToParent()));
    connect(ui->downloadButton,   SIGNAL(clicked()),                                             this, SLOT(downloadFile()));
    connect(ui->quitButton,       SIGNAL(clicked()),                                             this, SLOT(close()));

}

FtpClientDialog::~FtpClientDialog()
{
    if(m_pProgressDialog != nullptr) {
        delete m_pProgressDialog;
    }
    delete ui;
    delete m_pFtp;
    delete m_pNetworkSession;
    delete m_pFile;
}

void FtpClientDialog:: switch_agnss_online_offline_mode( bool mode)
{
    if(0==mode)
    {
        this->setWindowTitle(" Assistant GNSS Offline");
        ui->downloadToModule_Button->setVisible(true);
    }else{
        this->setWindowTitle(" Assistant GNSS Online");
        ui->downloadToModule_Button->setVisible(false);
    }
}

void FtpClientDialog::connectOrDisconnect()
{
    if (m_pFtp) {
        m_pFtp->abort();
        m_pFtp->deleteLater();
        m_pFtp = nullptr;

        ui->fileList->setEnabled(false);
        ui->cdToParentButton->setEnabled(false);
        ui->downloadButton->setEnabled(false);
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText(tr("Connect"));
        setCursor(Qt::ArrowCursor);
        ui->status_plainTextEdit->clear();
        ui->status_plainTextEdit->insertPlainText(tr("Please enter the name of an FTP server."));
        return;
    }

    if (!m_pNetworkSession || !m_pNetworkSession->isOpen()) {
        if (m_Manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
            if (!m_pNetworkSession) {
                // Get saved network configuration
                QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
                settings.beginGroup(QLatin1String("QtNetwork"));
                const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
                settings.endGroup();

                // If the saved network configuration is not currently discovered use the system default
                QNetworkConfiguration config = m_Manager.configurationFromIdentifier(id);
                if ((config.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
                    config = m_Manager.defaultConfiguration();
                }

                m_pNetworkSession = new QNetworkSession(config, this);
                connect(m_pNetworkSession, SIGNAL(opened()), this, SLOT(connectToFtp()));
                connect(m_pNetworkSession, SIGNAL(error(Qm_pNetworkSession::SessionError)), this, SLOT(enableConnectButton()));
            }

            m_pNetworkSession->open();

            return;
        }
    }
    connectToFtp();
}

void FtpClientDialog::connectToFtp()
{
    m_pFtp = new QFtp(this);
//    m_pFtp->setTransferMode(QFtp::Passive);
    QString address=ui->ftpServerLineEdit->text().isEmpty()? "agnss.queclocator.com":ui->ftpServerLineEdit->text();
    QString userName=ui->edtUser->text()=="Default"?"QEPO":ui->edtUser->text();
    QString password=ui->edtPassword->text().isEmpty()? "123456":ui->edtPassword->text();
//    MainWindow * mainForm = (MainWindow*) parentWidget();


    //"LC29DA"
    if(mainForm->modelIndex == LC29DA)
    {
        userName="BreamLTO";
    }
    //"L26-DR"
    else if(mainForm->modelIndex == L26ADR
            ||mainForm->modelIndex == LC99TIA
            ||mainForm->modelIndex == L26DRAA
            ||mainForm->modelIndex == LC29TAA
            ||mainForm->modelIndex == L26UDR
            ||mainForm->modelIndex == L26ADRC
            ||mainForm->modelIndex == L26T
            ||mainForm->modelIndex == LC98S
            ||mainForm->modelIndex == L26P
            ||mainForm->modelIndex == L89
            )
    {
        userName="st-demo";
    }
    //("LC79D"));
//    else if( 0 == QString::compare(mainForm->m_settings->m_module_name,"LC79D",Qt::CaseInsensitive))
//    {
//        userName="QLTO";
//    }

    connect(m_pFtp, SIGNAL(commandFinished(int,bool)), this, SLOT(ftpCommandFinished(int,bool)));
    connect(m_pFtp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
    connect(m_pFtp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(updateDataTransferProgress(qint64,qint64)));

    ui->fileList   ->clear();
    m_strCurrentPath.clear();
    m_bIsDirectory  .clear();

    QUrl url(ui->ftpServerLineEdit->text());
    if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp")) {
//        m_pFtp->setTransferMode(QFtp::Passive);

        qDebug()<<"FTP address:"<<address;
        m_pFtp->connectToHost(address , 21);

        qDebug()<<"FTP userName:"<<userName;
        m_pFtp->login(userName,
                      password);
    } else {
        m_pFtp->connectToHost(url.host(), (quint16)url.port(21));

        if (!url.userName().isEmpty())
            m_pFtp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
        else
            m_pFtp->login();
        if (!url.path().isEmpty())
            m_pFtp->cd(url.path());
    }

    ui->fileList->setEnabled(true);
    ui->connectButton->setEnabled(false);
    ui->connectButton->setText(tr("Disconnect"));

    ui->status_plainTextEdit->clear();
    ui->status_plainTextEdit->insertPlainText(tr("Connecting to FTP server %1...").arg(ui->ftpServerLineEdit->text()));
}

void FtpClientDialog::downloadFile()
{
    QString fileName;

#if 1
    {
        fileName = ui->fileList->currentItem()->text(0);
        ui->angssFileDir_lineEdit->setText(fileName);
    }
#endif
    /* remove old file */

    if (QFile::exists(fileName)) {
        QFile::remove(fileName);
    }
    if (QFile::exists(fileName)) {
        QMessageBox::information(this, tr("FTP"), tr("There already exists a file called %1 in the current directory.").arg(fileName));
        return;
    }

    m_pFile = new QFile(ui->angssFileDir_lineEdit->text());

    if (m_pFile->exists()) {
        m_pFile->remove();
    }

    if (!m_pFile->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("FTP"),
        tr("Unable to save the file %1: %2.")
        .arg(fileName).arg(m_pFile->errorString()));
        delete m_pFile;
        return;
    }

    m_pFtp->get(ui->fileList->currentItem()->text(0), m_pFile);

    if(m_pProgressDialog == nullptr) {
        m_pProgressDialog = new QProgressDialog(this);
        connect(m_pProgressDialog,    SIGNAL(canceled()),                                            this, SLOT(cancelDownload()));
    }
    m_pProgressDialog->setLabelText(tr("Downloading %1...").arg(fileName));
    ui->downloadButton->setEnabled(false);
    m_pProgressDialog->exec();
}

void FtpClientDialog::cancelDownload()
{
    if(m_pFtp != nullptr) {
        m_pFtp->abort();

        if (m_pFile->exists()) {
            m_pFile->close();
            m_pFile->remove();
        }
        delete m_pFtp;
    }
}

void FtpClientDialog::ftpCommandFinished(int, bool error)
{
    setCursor(Qt::ArrowCursor);

    if (m_pFtp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::information(this, tr("FTP"),
            tr("Unable to connect to the FTP server "
            "at %1. Please check that the host "
            "name is correct.")
            .arg(ui->ftpServerLineEdit->text()));
            connectOrDisconnect();
            return;
        }
        ui->status_plainTextEdit->clear();
        ui->status_plainTextEdit->insertPlainText(tr("Logged onto %1.").arg(ui->ftpServerLineEdit->text()));

        ui->fileList->setFocus();
        ui->downloadButton->setDefault(true);
        ui->connectButton->setEnabled(true);
        return;
    }
    if (m_pFtp->currentCommand() == QFtp::Login)
    {
        qDebug()<<"QFtp::Login"<<error;
        if(error == false)
            m_pFtp->list();
    }
    if (m_pFtp->currentCommand() == QFtp::Get) {
        if (error) {
            ui->status_plainTextEdit->clear();
            ui->status_plainTextEdit->insertPlainText(tr("Canceled download of %1.").arg(m_pFile->fileName()));
            m_pFile->close();
            m_pFile->remove();
        } else {

            ui->status_plainTextEdit->clear();
            ui->status_plainTextEdit->insertPlainText(tr("Downloaded  to directory %1.").arg(m_pFile->fileName()));
            m_pFile->close();

            qDebug()<<"FTP FILE: File size:"<<m_pFile->size()<<" Need download file size:"<<m_file_total_size;

            if(m_pFile->size() < m_file_total_size)
            {
                m_pFile->remove();
                ui->status_plainTextEdit->insertPlainText(tr("File wasn't completely downloaded from FTP sever,delete %1.").arg(m_pFile->fileName()));
            }
        }
        enableDownloadButton();
        m_pProgressDialog->hide();

    }
    if (m_pFtp->currentCommand() == QFtp::List) {
        qDebug()<<"QFtp::List"<<error;
        if (m_bIsDirectory.isEmpty()) {
            ui->fileList->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
            ui->fileList->setEnabled(false);
        }
    }
}

void FtpClientDialog::addToList(const QUrlInfo &urlInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, urlInfo.name());
    item->setText(1, QString::number(urlInfo.size()));
    item->setText(2, urlInfo.owner());
    item->setText(3, urlInfo.group());
    item->setText(4, urlInfo.lastModified().toString("yyyy-MM-dd hh:mm"));

    QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
    item->setIcon(0, pixmap);

    m_bIsDirectory[urlInfo.name()] = urlInfo.isDir();
    ui->fileList->addTopLevelItem(item);
    if (!ui->fileList->currentItem()) {
        ui->fileList->setCurrentItem(ui->fileList->topLevelItem(0));
        ui->fileList->setEnabled(true);
    }
}
//![10]

//![11]
void FtpClientDialog::processItem(QTreeWidgetItem *item, int /*column*/)
{
    QString name = item->text(0);

    m_storeAgnssFileDir =name;
    ui->angssFileDir_lineEdit->setText(m_storeAgnssFileDir);

    if (m_bIsDirectory.value(name)) {
        ui->fileList->clear();
        m_bIsDirectory.clear();
        m_strCurrentPath += '/';
        m_strCurrentPath += name;
        m_pFtp->cd(name);
        m_pFtp->list();
        ui->cdToParentButton->setEnabled(true);
        ui->cdToParentButton->setVisible(false);
        setCursor(Qt::WaitCursor);
        return;
    }
}

void FtpClientDialog::cdToParent()
{
    setCursor(Qt::WaitCursor);
    ui->fileList->clear();
    m_bIsDirectory.clear();
    m_strCurrentPath = m_strCurrentPath.left(m_strCurrentPath.lastIndexOf('/'));
    if (m_strCurrentPath.isEmpty()) {
        ui->cdToParentButton->setEnabled(false);
        m_pFtp->cd("/");
    } else {
        m_pFtp->cd(m_strCurrentPath);
    }
    m_pFtp->list();
}

void FtpClientDialog::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    m_pProgressDialog->setMaximum(totalBytes);
    m_pProgressDialog->setValue(readBytes);

    m_file_total_size = totalBytes;
    qDebug()<<"m_file_total_size need get is"<<m_file_total_size;
}

void FtpClientDialog::enableDownloadButton()
{
    QTreeWidgetItem *current = ui->fileList->currentItem();
    if (current) {
        QString currentFile = current->text(0);
        ui->downloadButton->setEnabled(!m_bIsDirectory.value(currentFile));
    } else {
        ui->downloadButton->setEnabled(false);
    }
}

void FtpClientDialog::enableConnectButton()
{
    // Save the used configuration
    QNetworkConfiguration config = m_pNetworkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = m_pNetworkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    ui->connectButton->setEnabled(true);

    ui->status_plainTextEdit->clear();
    ui->status_plainTextEdit->insertPlainText(tr("Please enter the name of an FTP server."));
}

void FtpClientDialog::on_cdToParentButton_clicked()
{
    setCursor(Qt::WaitCursor);
    ui->fileList->clear();
    m_bIsDirectory.clear();
    m_strCurrentPath = m_strCurrentPath.left(m_strCurrentPath.lastIndexOf('/'));
    if (m_strCurrentPath.isEmpty()) {
        ui->cdToParentButton->setEnabled(false);
        m_pFtp->cd("/");
    } else {
        m_pFtp->cd(m_strCurrentPath);
    }
    m_pFtp->list();
}

void FtpClientDialog::sendTimeAndPositionAidingToLC29D()
{
    QDateTime time;
    short length=0;  /* frame length */
    char buf[LD2BRM_MAX_PACKET_SIZE] = {0};

    //3) Send BRM-AST-REF_TIME_UTC command to the module to inject the assistance time.
    if(ui->timeAiding_checkBox->isChecked())
    {
        if(ui->radioButton_2->isChecked())
        {

            time = QDateTime::currentDateTime().toUTC();
            ui->dateTime_lineEdit->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
        }else{
            time = QDateTime::fromString(ui->dateTime_lineEdit->text(),"yyyy-MM-dd hh:mm:ss");
        }

        {
            LD2BRM_AstRefTimeUtcInputPayload payload0;
            //memset(&payload0,00,sizeof(LD2BRM_AstNvmemPollPayload));
            /* 3-2 fill  payload */
            payload0.type         = 0x10;
            payload0.version      = 0;
            payload0.ref          = 0;
            payload0.leapSecs     = 0x80; //0x80 = –128 if unknown
            payload0.year         = time.date().year();
            payload0.month        = time.date().month();
            payload0.day          = time.date().day();
            payload0.hour         = time.time().hour();
            payload0.minute       = time.time().minute();
            payload0.second       = time.time().second();
            //payload0.reserved1    = ui->tab_5_tableWidget->item(10, 0)->text().toInt();
            payload0.ns           = time.time().msec()*1000;
            payload0.tAccS        = ui->accuarcy_lineEdit->text().toFloat()/1000;
            //payload0.reserved2[0] = ui->tab_5_tableWidget->item(12, 0)->text().toInt();
            //payload0.reserved2[1] = ui->tab_5_tableWidget->item(12, 0)->text().toInt();
            payload0.tAccNs       = (ui->accuarcy_lineEdit->text().toFloat()-payload0.tAccS)*1000000;

            /* 3-2 build frame */
            BuildBreamFrame(BRM_AST_INI,  sizeof(LD2BRM_AstRefTimeUtcInputPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
            /* 3-3 send frame */
            length=sizeof(LD2BRM_AstRefTimeUtcInputPayload)+8;
            qgnss_sleep(1200);
            emit sendData(buf, length);
        }
    }

    //2) Send BRM-AST-REF_LOCATION command to the module to inject the assistance position.
    if(ui->positionAiding_checkBox->isChecked())
    {
        qgnss_sleep(100);

        LD2BRM_AstIniPosLlhInputPayload payload0;
        //memset(&payload0,00,sizeof(LD2BRM_AstNvmemPollPayload));
        /* 3-2 fill  payload */
        payload0.type         = 1;
        payload0.version      = 0;
        payload0.reserved1[0] = 0;
        payload0.reserved1[1] = 0;
        payload0.lat        = ui->latitude_lineEdit   ->text().toInt()*10000000;
        payload0.lon        = ui->longitude_lineEdit  ->text().toInt()*10000000;
        payload0.alt        = ui->altitude_lineEdit   ->text().toInt()*1000;
        payload0.posAcc     = ui->posAccuracy_lineEdit->text().toInt()*1000;

        /* 3-2 build frame */
        BuildBreamFrame(BRM_AST_INI, sizeof(LD2BRM_AstIniPosLlhInputPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
        /* 3-3 send frame */
        length=sizeof(LD2BRM_AstIniPosLlhInputPayload)+8;
        emit sendData(buf, length);
    }
    return;
}

/* Compute NMEA check SUM(XOR SUM) */
uint8_t NMEA_ComputeChksum( uint8_t *pBuf)
{
    uint8_t ckSum = 0;

    while ( (*pBuf != '\0') && ((*pBuf != '*')) )
    {
        ckSum ^= *pBuf++;
    }

    return ckSum;

}

void FtpClientDialog::sendTimeAndPositionAidingToL26DR()
{
    QDateTime time;
    char nmea_cmd[128]="";
    unsigned char checkSum;
    if(ui->timeAiding_checkBox->isChecked())
    {
        if(ui->radioButton_2->isChecked())
        {

            time = QDateTime::currentDateTime().toUTC().addSecs(ui->LeapSecond->text().toInt());
            ui->dateTime_lineEdit->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
        }else{
            time = QDateTime::fromString(ui->dateTime_lineEdit->text(),"yyyy-MM-dd hh:mm:ss").addSecs(ui->LeapSecond->text().toInt());
        }
        snprintf(nmea_cmd,128,
                 "$PSTMINITTIME,%d,%d,%d,%d,%d,%d",
                 time.date().day(),
                 time.date().month(),
                 time.date().year(),
                 time.time().hour(),
                 time.time().minute(),
                 time.time().second());

        checkSum = NMEA_ComputeChksum((unsigned char *)&nmea_cmd[1]);
        snprintf(nmea_cmd,128,"%s*%02X\r\n",nmea_cmd,checkSum);
        qgnss_sleep(1200);
        emit sendData((char *)nmea_cmd,strlen(nmea_cmd) );

    }


    //$PSTMINITGPS,<Lat>,<LatRef>,<Lon>,<LonRef>,<Alt>,<Day>,<Month>,<Year>,<Hour>,<Minute>,<Second>*<checksum><CR><LR>
    if(ui->positionAiding_checkBox->isChecked())
    {
        double Lat = ui->latitude_lineEdit   ->text().toDouble();
        double Lon = ui->longitude_lineEdit   ->text().toDouble();
        int LatD = static_cast<int>(fabs(Lat));
        int LonD = static_cast<int>(fabs(Lon));
        double LatM = (fabs(Lat) - LatD)*60.0;
        double LonM = (fabs(Lon) - LonD)*60.0;

        snprintf(nmea_cmd,128,
                 "$PSTMINITGPS,%2d%05.3f,%s,%3d%06.3f,%s,%s,%02d,%02d,%04d,%02d,%02d,%02d",
                 LatD,LatM,
                 Lat > 0?"N":"S",
                 LonD,LonM,
                 Lon > 0?"E":"W",
                 ui->altitude_lineEdit->text().toStdString().c_str(),
                 time.date().day(),
                 time.date().month(),
                 time.date().year(),
                 time.time().hour(),
                 time.time().minute(),
                 time.time().second());

        checkSum = NMEA_ComputeChksum((unsigned char *)&nmea_cmd[1]);
        snprintf(nmea_cmd,128,"%s*%02X\r\n",nmea_cmd,checkSum);
        qgnss_sleep(100);
        emit sendData((char *)nmea_cmd,strlen(nmea_cmd) );

    }
}

void FtpClientDialog::sendTimeAndPositionAidingToL76()
{
    QDateTime time;
    char nmea_cmd[128]="";
    unsigned char checkSum;
    if(ui->timeAiding_checkBox->isChecked())
    {
        if(ui->radioButton_2->isChecked())
        {

            time = QDateTime::currentDateTime().toUTC().addSecs(ui->LeapSecond->text().toInt());
            ui->dateTime_lineEdit->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
        }else{
            time = QDateTime::fromString(ui->dateTime_lineEdit->text(),"yyyy-MM-dd hh:mm:ss").addSecs(ui->LeapSecond->text().toInt());
        }
        snprintf(nmea_cmd,128,
                 "$PMTK740,%04d,%02d,%02d,%02d,%02d,%02d",
                 time.date().year(),
                 time.date().month(),
                 time.date().day(),
                 time.time().hour(),
                 time.time().minute(),
                 time.time().second());

        checkSum = NMEA_ComputeChksum((unsigned char *)&nmea_cmd[1]);
        snprintf(nmea_cmd,128,"%s*%02X\r\n",nmea_cmd,checkSum);
        qgnss_sleep(1200);
        emit sendData((char *)nmea_cmd,strlen(nmea_cmd) );

    }

    //$PMTK741,24.772816,121.022636,160,2016,01,01,12,00,00*17<CR><LF>
    if(ui->positionAiding_checkBox->isChecked())
    {
//        double Lat = ui->latitude_lineEdit   ->text().toDouble();
//        double Lon = ui->longitude_lineEdit   ->text().toDouble();
//        int LatD = static_cast<int>(fabs(Lat));
//        int LonD = static_cast<int>(fabs(Lon));
//        double LatM = (fabs(Lat) - LatD)*60.0;
//        double LonM = (fabs(Lon) - LonD)*60.0;

        snprintf(nmea_cmd,128,
                 "$PMTK741,%f,%f,%f,%04d,%02d,%02d,%02d,%02d,%02d",
                 ui->latitude_lineEdit->text().toDouble(),
                 ui->longitude_lineEdit->text().toDouble(),
                 ui->altitude_lineEdit->text().toDouble(),
                 time.date().year(),
                 time.date().month(),
                 time.date().day(),
                 time.time().hour(),
                 time.time().minute(),
                 time.time().second());

        checkSum = NMEA_ComputeChksum((unsigned char *)&nmea_cmd[1]);
        snprintf(nmea_cmd,128,"%s*%02X\r\n",nmea_cmd,checkSum);
        qgnss_sleep(100);
        emit sendData((char *)nmea_cmd,strlen(nmea_cmd) );
    }
}
void FtpClientDialog::sendTimeAndPositionAidingToLC79D()
{

    QDateTime time;
    char nmea_cmd[128]="";
    unsigned char checkSum;


    /*stop GNSS(need about 2000mS)*/
    qgnss_sleep(2000);
    /*stop GNSS */
    emit sendData((char *)"$PQSTOPGNSS*10\r\n",strlen("$PQSTOPGNSS*10\r\n") );

    //TIME aiding $PQSETASSTIME,<year>,<month>,<day>,<hour>,<min>,<sec>*<checksum><CR><LF>
    qgnss_sleep(1000);
    if(ui->timeAiding_checkBox->isChecked())
    {
        if(ui->radioButton_2->isChecked())
        {

            time = QDateTime::currentDateTime().toUTC();
            ui->dateTime_lineEdit->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
        }else{
            time = QDateTime::fromString(ui->dateTime_lineEdit->text(),"yyyy-MM-dd hh:mm:ss");
        }
        snprintf(nmea_cmd,128,
                 "$PQSETASSTIME,%d,%d,%d,%d,%d,%d",
                 time.date().year(),
                 time.date().month(),
                 time.date().day(),
                 time.time().hour(),
                 time.time().minute(),
                 time.time().second());

        checkSum = NMEA_ComputeChksum((unsigned char *)&nmea_cmd[1]);
        snprintf(nmea_cmd,128,"%s*%02X\r\n",nmea_cmd,checkSum);
        emit sendData((char *)nmea_cmd,strlen(nmea_cmd) );

    }

    //POS aiding  $PQSETASSPOS,<lat>,<lon>,<alt>[,<hor_acc>,<ver_acc>]*<checksum><CR><LF>
    if(ui->positionAiding_checkBox->isChecked())
    {


        qgnss_sleep(600);
        snprintf(nmea_cmd,128,
                 "$PQSETASSPOS,%s,%s,%s,%s,%s",
                 ui->latitude_lineEdit   ->text().toStdString().c_str(),
                 ui->longitude_lineEdit  ->text().toStdString().c_str(),
                 ui->altitude_lineEdit   ->text().toStdString().c_str(),
                 ui->posAccuracy_lineEdit->text().toStdString().c_str(),
                 ui->posAccuracy_lineEdit->text().toStdString().c_str());

        checkSum = NMEA_ComputeChksum((unsigned char *)&nmea_cmd[1]);
        snprintf(nmea_cmd,128,"%s*%02X\r\n",nmea_cmd,checkSum);

        emit sendData((char *)nmea_cmd,strlen(nmea_cmd) );

    }

    qgnss_sleep(1000);
    /*restart GNSS */
    emit sendData((char *)"$PQSTARTGNSS*48\r\n",strlen("$PQSTARTGNSS*48\r\n") );

    return;
}

void FtpClientDialog::on_useCurrentLLApushButton_clicked()
{
    if(solution_data == nullptr)
        return;
    if(solution_data->savedData_L.size())
    {
        ui->latitude_lineEdit ->setText(QString::number(solution_data->savedData_L.first().Latitude,'g',10)) ;
        ui->longitude_lineEdit->setText(QString::number(solution_data->savedData_L.first().Longitude,'g',10));
        ui->altitude_lineEdit ->setText(QString::number(solution_data->savedData_L.first().AltitudeMSL,'g',10));
    }
}

void FtpClientDialog::on_radioButton_2_toggled(bool checked)
{
    QDateTime time = QDateTime::currentDateTime().toUTC();
    ui->dateTime_lineEdit->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
    if(checked)
    {
        ui->radioButton->setChecked(false);

    }else{
        ui->radioButton->setChecked(true);
    }
}

void FtpClientDialog::on_radioButton_toggled(bool checked)
{

    QDateTime time = QDateTime::currentDateTime().toUTC();

    if(checked)
    {
        ui->dateTime_lineEdit->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
        ui->radioButton_2->setChecked(false);

    }else{
        ui->radioButton_2->setChecked(true);
    }
}

void FtpClientDialog::on_saveAgnssFile_pushButton_clicked()
{

    qDebug()<<"LTO AGNSS file save ";
    m_storeAgnssFileDir = QFileDialog::getOpenFileName(this,tr("Open AGNSS data file"),".",tr("AGNSS File(*.brm *.MD5 *.DAT *.EPO *.txt);;ALL(*)"));

    qDebug()<<"m_storeAgnssFileDir:"<<m_storeAgnssFileDir;
    if(m_storeAgnssFileDir.isEmpty())
    {
        m_storeStatus = false;
    }

    m_storeAgnssFile = new QFile(m_storeAgnssFileDir);

    qDebug()<<"m_storeAgnssFile:"<<m_storeAgnssFile;
    if(m_storeAgnssFile != nullptr)
    {

        m_storeStatus = true;
        ui->angssFileDir_lineEdit->setText(m_storeAgnssFileDir);
    }
    else
    {

        m_storeStatus = false;
    }
    qDebug()<<"m_storeStatus:"<<m_storeStatus;
}

void FtpClientDialog::on_resetAndAiding_pushButtonfon_clicked()
{
//    MainWindow * mainForm = (MainWindow*) parentWidget();

   if(ui->coldStart_radioButton->isChecked())
   {
       mainForm->send_GNSS_cold_start();
       qgnss_sleep(150);
   }
   else if(ui->warmStart_radioButton->isChecked())
   {
       mainForm->send_GNSS_warm_start();
       qgnss_sleep(150);
   }
   else if(ui->hotStart_radioButton->isChecked())
   {
       mainForm->send_GNSS_hot_start();
       qgnss_sleep(50);
   }

   sendTimeAndPosition(mainForm->modelIndex);
   /*AGNSS  online inject online file to module after COLD/WARM/HOT reset */
   if(ui->addEPO->isChecked()){
       downloadToModule();
   }
}

void FtpClientDialog::sendAidingDataToModule(void)
{
//   MainWindow * p = (MainWindow*) parentWidget();
   qDebug() << "AGNSS:LC29d FtpClientDialog::sendAidingDataToModule";
   sendTimeAndPosition(mainForm->modelIndex);
   /*AGNSS  online inject online file to module after COLD/WARM/HOT reset */
   if(ui->addEPO->isChecked()){
       downloadToModule();
   }
}

uint calculate_File_ether_crc32(uint crc32val, QFile * str,unsigned int file_length)
{
    unsigned int i = 0;
    unsigned char tempByte = 0;
    QByteArray byte;
    unsigned int len = file_length;

    crc32val = crc32val ^ 0xffffffff;

    for (i = 0; i < len; i++)
    {
        byte = str->read(1);
        if(byte.size()>0)
        {
            tempByte = static_cast<unsigned char>(byte.at(0));

            crc32val = crc32_tab[(crc32val ^ tempByte) & 0xff] ^ (crc32val >> 8);
        }else{
            break;
        }
    }

    return crc32val ^ 0xffffffff;
}


void  FtpClientDialog::UpdateSizeAndCRC()
{
    if(nullptr!=m_pFile)
    {
        if(m_pFile->isOpen())
        {
            m_pFile->close();
            qDebug() << "AGNSS: Close File first.";

        }
    }
    m_pFile = new QFile(ui->angssFileDir_lineEdit->text());
    m_pFile->open(QIODevice::ReadOnly);
    fileSize = static_cast<uint32_t>(m_pFile->size());
    qDebug()<<"lto File size:"<<fileSize;

    m_pFile->seek(0);

    // Calculate CRC32 on "file size"
    crc32 = calculate_ether_crc32(0, static_cast<uint32_t>(fileSize));

    // Calculate CRC32 on "code"
    crc32 = calculate_File_ether_crc32(crc32, m_pFile,fileSize);
    qDebug()<<"ltoFileCRC32:" << crc32;
    m_pFile->close();
    ltoOptions.Size = fileSize;
    ltoOptions.CRC = crc32;
}
//----------------------------------------------------------------------------------------
// Write LTO file info
//----------------------------------------------------------------------------------------
int FtpClientDialog::WriteLTOFileInfo()
{
    QByteArray data ;
    char * binaryInfo=nullptr;
    char buffer[14]    ={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;

//    MainWindow * mainForm = (MainWindow*) parentWidget();

    // Convert Binary Infos to char array
    binaryInfo =(char*) &ltoOptions;

    // Send Binary info

    buffer[0] = 0xAA;
    buffer[1] = 0x01;
    buffer[2] = 0x01;
    buffer[3] = 0x08;
    buffer[4] = binaryInfo[0];
    buffer[5] = binaryInfo[1];
    buffer[6] = binaryInfo[2];
    buffer[7] = binaryInfo[3];
    buffer[8] = binaryInfo[4];
    buffer[9] = binaryInfo[5];
    buffer[10] = binaryInfo[6];
    buffer[11] = binaryInfo[7];
    for (i = 0; i < 9; i++)
    {
        buffer[12] ^= buffer[3 + i];
    }
    buffer[13] = 0x55;

    char debugBuffer[3*14]={0};
    for (i = 0; i < 14; i++)
    {
        snprintf(debugBuffer+strlen(debugBuffer),3*14,"%02X ", (unsigned char)buffer[i]);
    }
    qDebug() << "AGNSS: WriteLTOFileInfo :" << debugBuffer;

    mainForm->m_serial->write(buffer, 14);
    mainForm->m_serial->waitForReadyRead(3000);
    data = mainForm->m_serial->readAll();
    if(data.contains(static_cast<char>(DEVICE_ACK))){
        return 1;
    }else{
        QMessageBox::critical(this, "Error",tr("The module did not receive LTO file information!") , QMessageBox::Ok);
        return 0;
    }
}

int FtpClientDialog:: ProgramFlashMemory()
{
    int i = 0;
    int j = 0;
    double numCycles = 0 ;
    QByteArray dataRead;  /* data read from file */
    QByteArray data;      /* data read from serial port */
//    MainWindow * mainForm = (MainWindow*) parentWidget();
    if(nullptr!=m_pFile){
        m_pFile->open(QIODevice::ReadOnly);
    }
    // Start to read from the start of the file
    m_pFile->seek(0);
    numCycles = ceil(fileSize * 1.0 / FLASHER_BLOCK_SIZE);
    // Send Binary Image
    for ( i = 1; i <= numCycles; i++)
    {
        dataRead = m_pFile->read(FLASHER_BLOCK_SIZE);
        for (j=0; j<3; j++) {
            mainForm->m_serial->write(dataRead.toStdString().c_str(), dataRead.length());
            if(mainForm->m_serial->waitForReadyRead(3000)){
                if(mainForm->m_serial->readAll().contains(static_cast<char>(DEVICE_ACK))){
                    //ShowProgressInfo("Program Flash memory...", 6 + i);
                    m_update_percent = static_cast<int>(i/numCycles*40) + 50;
                    ui->downloadToModule_progressBar->setValue(m_update_percent);
                    break;
                }
            }else if(j == 2){
                QMessageBox::critical(this, "Error", "LTO file download failed!", QMessageBox::Ok);
                ui->downloadToModule_progressBar->setValue(0);
                return 0;
            }
        }
    }
    return 1;
}


void FtpClientDialog::downloadAgnssDataToLC79dModule()//offline
{
//    MainWindow * mainForm = (MainWindow*) parentWidget();
    QByteArray data;

    if(mainForm->sendAndWaitForACK(NMEA_FWUPGRADE_COMMAND_STRING,mainForm->NMEA_Data.ACK._nST_LTODOWNLOAD)){
        ui->downloadToModule_progressBar->setValue(3);
        disconnect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
        UpdateSizeAndCRC();
        if(WriteLTOFileInfo()){
            qDebug()<<"AGNSS: Receive LTO file information Ok ";
            //Wait Erase Flash
            mainForm->m_serial->waitForReadyRead(8000);
            data = mainForm->m_serial->readAll();
            if(data.contains(static_cast<char>(DEVICE_ACK))){
                qDebug()<<"AGNSS: EraseFlashProgramArea Ok ";
                ui->downloadToModule_progressBar->setValue(40);
                if(ProgramFlashMemory()){
                    qDebug()<<"AGNSS: LTO Transfer complete";
                    //Wait CRC passed
                    mainForm->m_serial->waitForReadyRead(8000);
                    if(mainForm->m_serial->readAll().contains(static_cast<char>(DEVICE_ACK))){
                        ui->downloadToModule_progressBar->setValue(100);
                        qDebug()<<"AGNSS: CRC passed";
                    }
                    else{
                        QMessageBox::critical(this, "Error",tr("LTO file CRC not passed!") , QMessageBox::Ok);
                        ui->downloadToModule_progressBar->setValue(0);
                    }
                }
            }
            else{
                QMessageBox::critical(this, "Error", SERIAL_FLASH_ERASING_ERROR_STRING, QMessageBox::Ok);
                ui->downloadToModule_progressBar->setValue(0);
            }
        }
    }else{
        QMessageBox::critical(this, "Error",tr("Module can not enter LTO data download mode!") , QMessageBox::Ok);
        ui->downloadToModule_progressBar->setValue(0);
    }
    connect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
}

void FtpClientDialog::downloadAgnssDataToLC29dModule()//online
{
    QByteArray data ;
    int i=0;
    bool openFileResult=false;
    time_t rawtime = QDateTime::currentDateTime().toUTC().toSecsSinceEpoch();
    int64_t time_threshold = 10800; // 3 hours
    int64_t time_diff;
    int     FCT=0;
    int     a0=0,a1=0,a2=0,a3=0;

    qDebug() << "AGNSS:LC29d downloadAgnssDataToLC29dModule :" << m_pFile<<"rawtime:"<<rawtime;


    if(nullptr!=m_pFile)
    {
        if(m_pFile->isOpen())
        {
            m_pFile->close();
            qDebug() << "AGNSS:LC29d Close File first.";

        }

    }else{

    }

    m_pFile = new QFile(ui->angssFileDir_lineEdit->text());
    openFileResult = m_pFile->open(QIODevice::ReadOnly);//TODO: file size

    qDebug() << "AGNSS:LC29d Open File Result:" << openFileResult;

    // Get the size of selected code
    fileSize = m_pFile->size();//TODO: file size
    qDebug() << "AGNSS:LC29d Download to moudle file size:" << fileSize;

    // Start to read from the start of the file
    m_pFile->seek(0);


    while(1)
    {
        data = m_pFile->read(84);
        if(data.length() >=84 )
        {
            QString fctString;
            /*judge time range */
            a0 = (unsigned char)data.at(81); a0 = a0<<24;
            qDebug() << "AGNSS:LC29d a0:" << a0;
            a1 = (unsigned char)data.at(80); a1 = a1<<16;
            qDebug() << "AGNSS:LC29d a1:" << a1;
            a2 = (unsigned char)data.at(79); a2 = a2<<8;
            a3 = (unsigned char)data.at(78);
            FCT = a0+a1+a2+a3;

            qDebug() << "AGNSS:LC29d FCT:" <<FCT;
            time_diff = FCT+315964800 - rawtime;
            qDebug() << "AGNSS:LC29d FCT+315964800:" << (int)(FCT+315964800)<<"time_diff="<< (int)time_diff;
            // Current BRM-AST-LTO message is valid.
            if((time_diff > -time_threshold) && (time_diff < time_threshold))
            {
                qDebug() <<"AGNSS:LC29d Send AGNSS data["<<data.toHex(' ').toUpper()<<"]";
                emit sendData((char *)data.toStdString().c_str(),84);
                qgnss_sleep(5);
            }
            m_update_percent=m_pFile->pos()*100/fileSize;
            ui->downloadToModule_progressBar->setValue(m_update_percent);

        }else{
            m_pFile->close();//TODO: file size
            m_update_percent=100;
            ui->downloadToModule_progressBar->setValue(m_update_percent);
            break;
        }
    }
    qDebug()<<"AGNSS:LC29d send file end";
    return;
}

void FtpClientDialog::downloadAgnssDataToL26DRModule()
{
//    MainWindow * mainForm = (MainWindow*) parentWidget();
    QByteArray data;
    if(nullptr!=m_pFile)
    {
        if(m_pFile->isOpen())
        {
            m_pFile->close();
            qDebug() << "AGNSS:LC29d Close File first.";
        }
    }
    m_pFile = new QFile(ui->angssFileDir_lineEdit->text());
    if(!m_pFile->open(QIODevice::ReadOnly)){
        qDebug() << "downloadAgnssDataToL26DRModule::Open file failed";
        return;
    }
    qgnss_sleep(800);
    while(1)
    {
        data = m_pFile->readLine();
        if(data.length() >0 )
        {
            mainForm->sendAndWaitForACK(data,mainForm->NMEA_Data.ACK._L26DR);
            m_update_percent = static_cast<int>(m_pFile->pos()*100/m_pFile->size());
            ui->downloadToModule_progressBar->setValue(m_update_percent);

        }else{
            m_pFile->close();
            m_update_percent = 100;
            ui->downloadToModule_progressBar->setValue(m_update_percent);
            break;
        }
    }
}

void FtpClientDialog::downloadAgnssDataToL76ModuleHost()
{
//    MainWindow * mainForm = (MainWindow*) parentWidget();
    QDateTime time;
    QByteArray data;
    QByteArray nmea_cmd;
    QString str;
    int32_t wn;
    double tow;
    if(nullptr!=m_pFile)
    {
        if(m_pFile->isOpen())
        {
            m_pFile->close();
            qDebug() << "AGNSS:LC76 Close File first.";
        }
    }
    m_pFile = new QFile(ui->angssFileDir_lineEdit->text());
    if(!m_pFile->open(QIODevice::ReadOnly)){
        qDebug() << "downloadAgnssDataToL76ModuleHost::Open file failed";
        return;
    }
    time = QDateTime::currentDateTime().toUTC();
    utc_to_gpstime(time.date().year(),
                   time.date().month(),
                   time.date().day(),
                   time.time().hour(),
                   time.time().minute(),
                   time.time().second(),&wn,&tow);
    int cur_gps_second = wn*604800 + tow;
    while(1){
        data = m_pFile->read(72);
        if(data.size() == 0){
            break;
        }
        int epo_gps_second = static_cast<uint8_t>(data.at(1))*256 + static_cast<uint8_t>(data.at(2))*256*256 + static_cast<uint8_t>(data.at(0));
        epo_gps_second *= 3600;
        if((epo_gps_second + 21600) >= cur_gps_second){
            break;
        }
    }
    for (int n = 1; n < 33; n++) {
        nmea_cmd.append(str.sprintf("$PMTK721,%X",n));
        if(data.size() >= 72){
            for(int i = 0; i < 72; i+=4){
                nmea_cmd.append(str.sprintf(",%X%02X%02X%02X",
                                            static_cast<uint8_t>(data.at(i+3)),
                                            static_cast<uint8_t>(data.at(i+2)),
                                            static_cast<uint8_t>(data.at(i+1)),
                                            static_cast<uint8_t>(data.at(i))));
            }
            addNMEA_CS(nmea_cmd);
            qDebug() << nmea_cmd;
            mainForm->sendAndWaitForACK(nmea_cmd,mainForm->NMEA_Data.ACK._MTK);
            nmea_cmd.clear();
            m_update_percent = static_cast<int>(m_pFile->pos()*100/m_pFile->size());
            ui->downloadToModule_progressBar->setValue(m_update_percent);
        }else{
            qDebug() << "downloadAgnssDataToL76ModuleHost::EPO segment data < 72 ";
        }
        data = m_pFile->read(72);
    }
    ui->downloadToModule_progressBar->setValue(100);
}

void FtpClientDialog::downloadAgnssDataToL76Module()
{
    ui->downloadToModule_Button->setEnabled(false);
//    MainWindow * mainForm = (MainWindow*) parentWidget();
    const char *cmd = "$PMTK253,1,0*37\r\n";
    QByteArray data;
    QByteArray nmea_cmd;
    QString str;
    int32_t wn;
    uint16_t seq = 0;

    double tow;
    qgnss_sleep(100);
    emit sendData((char *)cmd,strlen(cmd));
    if(nullptr!=m_pFile)
    {
        if(m_pFile->isOpen())
        {
            m_pFile->close();
            qDebug() << "AGNSS:LC76 Close File first.";
        }
    }
    m_pFile = new QFile(ui->angssFileDir_lineEdit->text());
    if(m_pFile->size() == 0 || !((m_pFile->size()%2304 == 0)||(m_pFile->size()%4032 == 0))){
        QMessageBox::warning(this, "Warning", "File Open Failed!",QMessageBox::Ok);
        ui->downloadToModule_Button->setEnabled(true);
        return;
    }
    if(!m_pFile->open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "Warning", "File open!",QMessageBox::Ok);
        return;
    }
    QDateTime time = QDateTime::currentDateTime().toUTC();
    utc_to_gpstime(time.date().year(),
                   time.date().month(),
                   time.date().day(),
                   time.time().hour(),
                   time.time().minute(),
                   time.time().second(),&wn,&tow);
    int cur_gps_second = wn*604800 + tow;
    while(1){
        data = m_pFile->read(72);
        if(data.size() == 0){
            break;
        }
        int epo_gps_second = static_cast<uint8_t>(data.at(1))*256 + static_cast<uint8_t>(data.at(2))*256*256 + static_cast<uint8_t>(data.at(0));
        epo_gps_second *= 3600;
        if((epo_gps_second + 21600) >= cur_gps_second){
            break;
        }
    }
    QByteArray msgHead = QByteArray::fromHex("04 24 E3 00 D3 02");
    const char defaultData[72] = {0};
    while(1){
        nmea_cmd.append(msgHead);
        nmea_cmd.append(static_cast<char>(seq))
                .append(static_cast<char>(seq>>8));
        seq++;
        for (int i = 0; i < 3; i++) {
            if(data.size() >=72){
                nmea_cmd.append(data);
                data = m_pFile->read(72);
            }
            else{
                nmea_cmd.append(defaultData,72);
            }
        }
        add_CS(nmea_cmd,2);
        nmea_cmd.append(0x0d).append(0x0a);
        mainForm->sendAndWaitForACK(nmea_cmd,mainForm->MTKbin_Data.nEPO_ACK);
        m_update_percent = static_cast<int>(m_pFile->pos()*100/m_pFile->size());
        ui->downloadToModule_progressBar->setValue(m_update_percent);
        nmea_cmd.clear();
        if(!data.size()){
            nmea_cmd.append(msgHead).append(0xff).append(0xff);
            nmea_cmd.append(defaultData,72).append(defaultData,72).append(defaultData,72);
            add_CS(nmea_cmd,2);
            nmea_cmd.append(0x0d).append(0x0a);
            qgnss_sleep(200);
            emit sendData(nmea_cmd.data(),nmea_cmd.size());
            break;
        }
    }
    nmea_cmd.clear();
    nmea_cmd.append(QByteArray::fromHex("04 24 0E 00 FD 00 00 00 00 00 00 F3 0D 0A"));
    qgnss_sleep(300);
    emit sendData(nmea_cmd.data(),nmea_cmd.size());
    qgnss_sleep(500);
    emit sendData(nmea_cmd.data(),nmea_cmd.size());
    ui->downloadToModule_Button->setEnabled(true);
}

void FtpClientDialog::on_downloadToModule_Button_clicked()
{
    ui->downloadToModule_Button->setEnabled(false);
    qgnss_sleep(100);
    downloadToModule();
}

void FtpClientDialog::downloadToModule()
{
    ui->downloadToModule_Button->setEnabled(false);
    if (false ==mainForm->m_serial->isOpen())
    {
        QMessageBox::information(this, "Indication:", "Please open serial port first.", QMessageBox::Ok);
        ui->downloadToModule_Button->setEnabled(true);
        return ;
    }

    ui->downloadToModule_progressBar->setVisible(true);

    m_update_percent=1;
    ui->downloadToModule_progressBar->setValue(m_update_percent);
    switch (mainForm->modelIndex)
    {
    case QGNSS_Model::LC79DA:
        downloadAgnssDataToLC79dModule();
        break;
    case QGNSS_Model::L26ADR:
    case QGNSS_Model::L26DRAA:
    case QGNSS_Model::LC99TIA:
    case QGNSS_Model::LC29TAA:
    case QGNSS_Model::L26UDR :
    case QGNSS_Model::L26ADRC:
    case QGNSS_Model::L26T   :
    case QGNSS_Model::LC98S  :
    case QGNSS_Model::L26P   :
    case QGNSS_Model::L89   :
        downloadAgnssDataToL26DRModule();
        break;
    case QGNSS_Model::LC29DA:
        downloadAgnssDataToLC29dModule();
        break;
    case QGNSS_Model::L76:
        {
            if( 0 == QString::compare(this->windowTitle()," Assistant GNSS Online",Qt::CaseInsensitive))
            {
                downloadAgnssDataToL76ModuleHost();
            }
            else{
                downloadAgnssDataToL76Module();
            }
            break;
        }
    }
    ui->downloadToModule_Button->setEnabled(true);
}

void FtpClientDialog::sendTimeAndPosition(uint32_t Index)
{
    /* wait 100ms */
    qgnss_sleep(100);
    switch (Index)
    {
    case QGNSS_Model::L26ADR:
    case QGNSS_Model::LC99TIA:
    case QGNSS_Model::LC29TAA:
    case QGNSS_Model::L26UDR:
    case QGNSS_Model::L26ADRC:
    case QGNSS_Model::L26T:
    case QGNSS_Model::LC98S:
    case QGNSS_Model::L26P:
    case QGNSS_Model::L89:
        sendTimeAndPositionAidingToL26DR();
        break;
    case LC29DA:sendTimeAndPositionAidingToLC29D();
        break;
    case LC79DA:sendTimeAndPositionAidingToLC79D();
        break;
    case L76:sendTimeAndPositionAidingToL76();
        break;
    }
}

void FtpClientDialog::setSolution_data(Solution_t *value)
{
    solution_data = value;
}
