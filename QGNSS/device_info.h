#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include <QDialog>
#include <QSerialPort>

#include "common.h"

namespace Ui {
class device_info;
}

using namespace quectel_gnss;



//struct device_info_t
//{
//    device_info_t() {}

//    QGNSS_Model model_type;
//    QString name;
//    qint32 baudRate;
//    QString stringBaudRate;
//    QSerialPort::DataBits dataBits;
//    QString stringDataBits;
//    QSerialPort::Parity parity;
//    QString stringParity;
//    QSerialPort::StopBits stopBits;
//    QString stringStopBits;
//    QSerialPort::FlowControl flowControl;
//    bool localEchoEnabled;
//    bool softFlowControl;
//};

class device_info : public QDialog
{
    Q_OBJECT

public:
    struct device_info_t
    {

        QString stringModelType;
        QGNSS_Model model_type;
        QString portName;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        bool localEchoEnabled;
        bool softFlowControl;
    };

    explicit device_info(QWidget *parent = nullptr);
    ~device_info();

    device_info_t getInfo_t() const;

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_buttonBox_accepted();

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
    Ui::device_info *ui;
    device_info_t info_t;

    void fillPortInfo();
    void fillPortsParameters();
    void saveSettings();
    void loadSettings();



};

#endif // DEVICE_INFO_H
