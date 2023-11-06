#include "ntrip_clientdialog.h"
#include "ui_ntrip_clientdialog.h"
#include "table_of_ntrip_mount_dialog.h"

#include <QSettings>
#include <QTime>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

NTRIP_ClientDialog::NTRIP_ClientDialog(QWidget *parent) : QDialog(parent),
                                                          ui(new Ui::NTRIP_ClientDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->clientLogEdit->setMaximumBlockCount(10000);
    ui->connectSwitch->setChecked(false);
    ui->connectSwitch->setFixedSize(87, 28);
    ui->connectSwitch->setButtonStyle(ImageSwitch::ButtonStyle_3);
    connect(ui->connectSwitch, SIGNAL(statusChanged(bool)), this, SLOT(ConnectToHost(bool)));
    //    ui->clientLogEdit->setMouseTracking(false);
    //    ui->clientLogEdit->setAcceptDrops(false);
    //    ui->clientLogEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    rwSettingData(false);
    NtripSocket = new QTcpSocket();
    requestTimer = new QTimer(this);
    reconnectTimer = new QTimer(this);
    requestTimer->stop();
    QDateTime time = QDateTime::currentDateTime();
    hostLogFile = new QFile("./logFile/NTRIP_Server" + time.toString("MMdd_hhmmss") + ".log");
    connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestCasterTimer()));
    connect(reconnectTimer, SIGNAL(timeout()), this, SLOT(Reconnect()));
    connect(NtripSocket, &QTcpSocket::readyRead, this, &NTRIP_ClientDialog::socket_Read_Data);
    connect(NtripSocket, &QTcpSocket::disconnected, this, &NTRIP_ClientDialog::socket_Disconnected);
    connect(NtripSocket,&QTcpSocket::stateChanged,this,[=](QAbstractSocket::SocketState state)
            {
              qDebug()<<"SocketState "<<state;
            });
    connect(NtripSocket,QOverload<QAbstractSocket::SocketError>().of( &QTcpSocket::error),this,[=](QAbstractSocket::SocketError state)
            {
              qDebug()<<"SocketError "<<state;
            });

    mAccessManager = new QNetworkAccessManager(this);
}

NTRIP_ClientDialog::~NTRIP_ClientDialog()
{
    ConnectToHost(false);
    //    NtripSocket->disconnectFromHost();
    delete ui;
    delete NtripSocket;
    delete requestTimer;
    delete reconnectTimer;
    delete hostLogFile;

    if (mAccessManager) {
        delete mAccessManager;
        mAccessManager = NULL;
    }
}

void NTRIP_ClientDialog::requestCasterTimer()
{
    if (ui->manualPosCheckBox->isChecked())
    {
        char gga[512];
        char *q, *p = gga, sum;
        int dd, ddd;
        double mm, mmm, lat, lon, mamualLat, mamualLon, mamualAlt, mamualGep;
        QDateTime date = QDateTime::currentDateTimeUtc();
        QByteArray timeStr(date.toString("hhmmss").toUtf8());
        mamualLat = ui->mamual_position_latitude_lineEdit->text().toDouble();
        mamualLon = ui->mamual_position_longitude_lineEdit->text().toDouble();
        mamualAlt = ui->mamual_position_altitude_lineEdit->text().toDouble();
        mamualGep = ui->mamual_position_geoid_sep_lineEdit->text().toDouble();
        lat = fabs(mamualLat);
        lon = fabs(mamualLon);
        dd = static_cast<int>(lat);
        ddd = static_cast<int>(lon);
        mm = (lat - dd) * 60;
        mmm = (lon - ddd) * 60;
        //
        //$GNGGA,075931.000,3149.32428,N,11706.98016,E,1,34,0.48,139.8,M,-3.6,M,,*6C
        p += sprintf(p, "$GNGGA,%s,%02d%09.6f,%s,%03d%09.6f,%s,1,34,0.48,%.1f,M,%.1f,M,,",
                     timeStr.data(),
                     dd, mm, mamualLat >= 0 ? "N" : "S",
                     ddd, mmm, mamualLon >= 0 ? "E" : "W",
                     mamualAlt,
                     mamualGep);
        for (q = gga + 1, sum = 0; *q; q++)
            sum ^= *q; /* check-sum */
        p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
        NtripSocket->write(gga, strlen(gga));
        qDebug() << "user gga" << gga;
        outputLog("Send GGA to server.");
    }
    else if (ggaStr != nullptr)
    {
        qDebug() << ggaStr;
        NtripSocket->write(ggaStr);
        outputLog("Send GGA to server.");
    }
}

void NTRIP_ClientDialog::socket_Read_Data()
{
    QByteArray hostData = NtripSocket->readAll();
    if (hostLogFile->open(QFile::Append))
    {
        hostLogFile->write(hostData);
        hostLogFile->close();
    }
    //    qDebug()<<hostData;
    outputLog(QString("Received (%1)bytes from server").arg(hostData.size()));
    emit send2Model(hostData);
    if (connectState != 3)
    {
        if (hostData.indexOf("ICY 200 OK") >= 0)
        {
            uint32_t interval = ui->requestInterval->text().toUInt();
            requestTimer->setInterval(interval > 0 ? interval * 1000 : 1000);
            requestTimer->start();
            reconnectTimer->stop();
            rwSettingData(true);
            connectState = 3;
        }
        else if (hostData.indexOf("SOURCETABLE 200 OK") >= 0)
        {
            QFile file("./logFile/NTRIP_SourceTable.txt");
            file.open(QIODevice::Append);
            file.write(QString("-----------------------------%1: %2-------------------------------\r\n")
                       .arg(ui->caster_address_LineEdit->text())
                       .arg(ui->caster_port_LineEdit->text())
                       .toLocal8Bit()
                       );
            file.write(hostData);
            file.close();
            outputLog("Get sourcetable success.");
            updateNTRIP_Sourcetable(hostData);
            connectState = 2;
        }
        else if (hostData.indexOf("Unauthorized") >= 0)
        {
            ui->connectSwitch->setChecked(false);
            outputLog("Incorrect username or password.");
            connectState = 1;
        }
    }
}

void NTRIP_ClientDialog::socket_Disconnected()
{

    requestTimer->stop();
    outputLog("Disconnected.");
    if (connectState == 3)
    {
        outputLog("Reconnection......");
        connectState = 0;
        reconnectTimer->start(5 * 1000);
    }
}

void NTRIP_ClientDialog::Reconnect()
{
    outputLog("......");
    ConnectToHost(true);
}

// true:write  false:read
int NTRIP_ClientDialog::rwSettingData(bool rw)
{
    QSettings settings;
    if (rw)
    {
        settings.setValue("Ntrip/IP", ui->caster_address_LineEdit->text());
        settings.setValue("Ntrip/Port", ui->caster_port_LineEdit->text());
        settings.setValue("Ntrip/Username", ui->caster_userName_LineEdit->text());
        //        settings.setValue("Ntrip/Password", ui->caster_password_LineEdit->text());
        return 1;
    }
    ui->caster_address_LineEdit->setText(settings.value("Ntrip/IP", "127.0.0.1").toString());
    ui->caster_port_LineEdit->setText(settings.value("Ntrip/Port", "8002").toString());
    ui->caster_userName_LineEdit->setText(settings.value("Ntrip/Username", "Quectel").toString());
    //    ui->caster_password_LineEdit->setText(settings.value("Ntrip/Password", "").toString());
    return 0;
}

void NTRIP_ClientDialog::updateNTRIP_Sourcetable(QByteArray &sourcetable)
{
    QString saw_data(sourcetable);
    this->sourcetable = sourcetable;
    //    ui->mountPointsComboBox->clear();
    foreach (auto text, saw_data.split('\n'))
    {
        QStringList strList = text.split(';');
        if (strList.takeFirst() == "STR")
        {
            ui->mountPointsComboBox->addItem(strList.takeFirst());
        }
    }
}

void NTRIP_ClientDialog::outputLog(QString log)
{
    QTime time = QTime::currentTime();
    ui->clientLogEdit->appendPlainText(QString("%1   %2").arg(time.toString()).arg(log));
    qDebug() << log;
}

void NTRIP_ClientDialog::on_update_NTRIP_souretable_PushButton_clicked()
{
    ui->mountPointsComboBox->clear();
    NtripSocket->connectToHost(ui->caster_address_LineEdit->text(), ui->caster_port_LineEdit->text().toUInt());
    if(!NtripSocket->waitForConnected (1000))
    {
      outputLog("Unconnected may be address error!");
      ui->connectSwitch->setChecked(false);
      return ;
    }
    NtripSocket->write("GET / HTTP/1.0\r\n"
                       "User-Agent: NTRIP QGNSS\r\n"
                       "Accept: */*\r\n"
                       "Connection: close\r\n"
                       "\r\n");
}

void NTRIP_ClientDialog::on_pushButton_point_details_clicked()
{
    Table_Of_NTRIP_Mount_Dialog infoDialog(this);
    QString saw_data(this->sourcetable);
    foreach (auto text, saw_data.split('\n'))
    {
        QStringList strList = text.split(';');
        if (strList.takeFirst() == "STR")
        {
            strList.takeAt(12);
            strList.takeAt(13);
            strList.takeLast();
            infoDialog.insertMountPointInfo2Table(strList);
        }
    }
    infoDialog.resize(800, 300);
    infoDialog.exec();
}

void NTRIP_ClientDialog::setGgaStr(char *value)
{
    ggaStr = value;
}

void NTRIP_ClientDialog::ConnectToHost(bool checked)
{
    if (checked)
    {
        if (ui->mountPointsComboBox->currentText().isEmpty())
        {
            outputLog("Please select the mount point.");
            ui->connectSwitch->setChecked(false);
            return;
        }
        if (ui->requestInterval->text().toUInt() < 1)
        {
            outputLog("Request Interval must be >= 1.");
            ui->connectSwitch->setChecked(false);
            return;
        }
        QString Authorization = QString("%1:%2")
                                    .arg(ui->caster_userName_LineEdit->text())
                                    .arg(ui->caster_password_LineEdit->text())
                                    .toLocal8Bit()
                                    .toBase64();
        QString requestHead = QString("GET /%1 HTTP/1.0\r\n"
                                      "User-Agent: NTRIP QGNSS\r\n"
                                      "Accept: */*\r\n"
                                      "Connection: close\r\n"
                                      "Authorization: Basic %2\r\n"
                                      "\r\n")
                                  .arg(ui->mountPointsComboBox->currentText())
                                  .arg(Authorization);
        NtripSocket->connectToHost(ui->caster_address_LineEdit->text(), ui->caster_port_LineEdit->text().toUInt());
        if(!NtripSocket->waitForConnected (1000))
        {
          outputLog("Unconnected may be address error!");
          ui->connectSwitch->setChecked(false);
          return ;
        }
        NtripSocket->write(requestHead.toLocal8Bit());
        qDebug() << "NTRIP_Client:" << requestHead;

    }
    else
    {
        connectState = 0;
        reconnectTimer->stop();
        NtripSocket->disconnectFromHost();
        NtripSocket->close();
        ui->connectSwitch->setChecked(false);
        outputLog("Active disconnection.");
    }
}

// 0:uncheck    2:checked
void NTRIP_ClientDialog::on_manualPosCheckBox_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        ui->mamual_position_longitude_lineEdit->setEnabled(true);
        ui->mamual_position_latitude_lineEdit->setEnabled(true);
        ui->mamual_position_altitude_lineEdit->setEnabled(true);
        ui->mamual_position_geoid_sep_lineEdit->setEnabled(true);
    }
    else
    {
        ui->mamual_position_longitude_lineEdit->setEnabled(false);
        ui->mamual_position_latitude_lineEdit->setEnabled(false);
        ui->mamual_position_altitude_lineEdit->setEnabled(false);
        ui->mamual_position_geoid_sep_lineEdit->setEnabled(false);
    }
}

void NTRIP_ClientDialog::on_pushButton_BS_clicked(bool checked)
{
    ui->pushButton_BS->setEnabled(false);
    mMCC = ui->lineEdit_MCC->text();
    mDeviceID = ui->lineEdit_DeviceID->text();
    mKey = ui->lineEdit_LisenceKey->text();
    if(mMCC.isEmpty()&&mDeviceID.isEmpty()&&mKey.isEmpty()){
        QMessageBox::warning(this,"Warning","Please enter the correct information!",QMessageBox::Ok);
        ui->pushButton_BS->setEnabled(true);
        return;
    }


    bool bSupp = QSslSocket::supportsSsl();
    QString buildVersion = QSslSocket::sslLibraryBuildVersionString();
    QString version = QSslSocket::sslLibraryVersionString();
    qDebug() << bSupp << buildVersion << version << endl;
    qDebug() << mAccessManager->supportedSchemes();

    QNetworkRequest request;
    QSslConfiguration cfg;
    cfg.setPeerVerifyMode(QSslSocket::VerifyNone);
    cfg.setProtocol(QSsl::AnyProtocol);

    request.setSslConfiguration(cfg);
//    request.setUrl(QUrl("https://106.14.222.3:31109/v1/bootstrap/getAddress"));
    request.setUrl(QUrl("https://bootstrap.quectel.com/v1/bootstrap/getAddress"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
//    request.setRawHeader("Accept", "*/*");

    QJsonObject json;
    json.insert("mcc", mMCC);
    json.insert("token", "JSUzI1NiJ9");
    json.insert("serviceName", "quec-rtk");

    QJsonDocument document;
    document.setObject(json);
    QByteArray dataArray = document.toJson(QJsonDocument::Compact);

    mReply = mAccessManager->post(request, dataArray);
    connect(mReply, &QNetworkReply::finished, [=](){
        QVariant statusCode = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        int code = statusCode.toInt();
        QByteArray resp = mReply->readAll();
        qInfo()<<"------BS1-------";
        qInfo()<<code;
        qInfo()<<QString(resp);
        if(code != 200){
            qInfo()<<QString ("BS1 error %1 !").arg(code);
            QMessageBox::warning(this,"Warning",QString ("BS1 error %1 !").arg(code),QMessageBox::Ok);
            ui->pushButton_BS->setEnabled(true);
            return;
        }
        auto res = QJsonDocument::fromJson(resp).object();
        if(res.contains("data")&&res.value("data").toObject().contains("address")){
            BS2URL = res.value("data").toObject().value("address").toString();
            qDebug()<<BS2URL;
            Q_BS2();
        }else {
            BS2URL.clear();
            QMessageBox::warning(this,"Warning",QString ("(BS1)Wrong data for the response !"),QMessageBox::Ok);
            ui->pushButton_BS->setEnabled(true);
            return;
        }
    });
    connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(httpErrorSlot(QNetworkReply::NetworkError)));}

void NTRIP_ClientDialog::httpErrorSlot(QNetworkReply::NetworkError err)
{
    QMessageBox::warning(this,"Warning",QString ("https error: %1\r\n").arg(err),QMessageBox::Ok);
    ui->pushButton_BS->setEnabled(true);
}

void NTRIP_ClientDialog::Q_BS2()
{
    QNetworkRequest request;
    QSslConfiguration cfg;
    cfg.setPeerVerifyMode(QSslSocket::VerifyNone);
    cfg.setProtocol(QSsl::AnyProtocol);

    request.setSslConfiguration(cfg);
//    request.setUrl(QUrl(QString("http://192.168.25.251:30039/")+"router/v1/rtk/router/device/auth"));
    request.setUrl(QUrl(BS2URL+"/router/v1/rtk/router/device/auth"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject json;
    json.insert("deviceID", mDeviceID);
    json.insert("licenseKey", mKey);

    QJsonDocument document;
    document.setObject(json);
    QByteArray dataArray = document.toJson(QJsonDocument::Compact);

    mReply = mAccessManager->post(request, dataArray);
    connect(mReply, &QNetworkReply::finished, [=](){
        QVariant statusCode = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        int code = statusCode.toInt();
        QByteArray resp = mReply->readAll();
        qInfo()<<"------BS2-------";
        qInfo()<<code;
        qInfo()<<QString(resp);
        if(code != 200){
            qInfo()<<QString ("BS2 error %1 !").arg(code);
            QMessageBox::warning(this,"Warning",QString ("BS2 error %1 !").arg(code),QMessageBox::Ok);
            ui->pushButton_BS->setEnabled(true);
            return;
        }

        auto res = QJsonDocument::fromJson(resp).object();
        QString msg = "Unknow error !";
        if(res.contains("code")){
            switch (res.value("code").toInt()) {
            case 200:
                if(res.contains("data")){
                    auto rJson = res.value("data").toObject();
                    QString port;
                    QString address = rJson.value("ntripUrl").toString();
                    if(rJson.contains("port")){
                        port = rJson.value("port").toString();
                    }else{
                        port = address.split(':')[1];
                        address = address.split(':')[0];
                    }
                    ui->caster_port_LineEdit->setText(port);
                    ui->caster_address_LineEdit->setText(address);
                    ui->caster_password_LineEdit->setText(rJson.value("password").toString());
                    ui->caster_userName_LineEdit->setText(rJson.value("account").toString());
                    ui->mountPointsComboBox->setEditText(rJson.value("mountPoint").toString());
                    ui->pushButton_BS->setEnabled(true);
                    msg = "Successfully, the account information has been automatically filled in the Client Tab.";
                }
                else{
                    qInfo()<<"error BS2 response no data !!!";
                }
                break;
            default:
                msg = QString("BS2 Execute the result code(%1) !").arg(res.value("code").toInt());
                break;
            }
        }
        QMessageBox::information(this,"Info",msg,QMessageBox::Ok);
        ui->pushButton_BS->setEnabled(true);
    });
    connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(httpErrorSlot(QNetworkReply::NetworkError)));
}
