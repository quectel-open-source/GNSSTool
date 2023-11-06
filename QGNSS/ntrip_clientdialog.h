#ifndef NTRIP_CLIENTDIALOG_H
#define NTRIP_CLIENTDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QTimer>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class NTRIP_ClientDialog;
}

class NTRIP_ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NTRIP_ClientDialog(QWidget *parent = nullptr);
    ~NTRIP_ClientDialog();
    void setGgaStr(char *value);

signals:
    void send2Model(QByteArray data);

private slots:
    void requestCasterTimer();
    void socket_Read_Data();
    void socket_Disconnected();
    void Reconnect();

    void on_update_NTRIP_souretable_PushButton_clicked();

    void on_pushButton_point_details_clicked();

    void ConnectToHost(bool checked);

    void on_manualPosCheckBox_stateChanged(int arg1);

    void on_pushButton_BS_clicked(bool checked);

    void httpErrorSlot(QNetworkReply::NetworkError);

private:
    Ui::NTRIP_ClientDialog *ui;
    QTcpSocket *NtripSocket;
    QString sourcetable;
    QTimer *requestTimer;
    QTimer *reconnectTimer;
    QFile *hostLogFile;
    const char *ggaStr = nullptr;
    int connectState = 0;

    QNetworkAccessManager *mAccessManager;
    QNetworkReply *mReply;
    QString BS2URL;

    QString mMCC;
    QString mDeviceID;
    QString mKey;

    int rwSettingData(bool rw);
    void updateNTRIP_Sourcetable(QByteArray &sourcetable);
    void outputLog(QString log);
    void Q_BS2();
};

#endif // NTRIP_CLIENTDIALOG_H
