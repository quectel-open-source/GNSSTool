#include "device_info.h"
#include "ui_device_info.h"

#include <windows.h>
#include <QDebug>
#include <QCompleter>
#include <QSerialPort>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QMetaEnum>
#include <QSerialPortInfo>

device_info::device_info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::device_info)
{
    ui->setupUi(this);
    setWindowTitle("Device Information");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->modelBox->setEditable(true);
    QCompleter* pCompleter = new QCompleter(ui->modelBox->model());
    pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    pCompleter->setFilterMode(Qt::MatchContains);
    ui->modelBox->setCompleter(pCompleter);
    //隐藏详细配置选项
    for (int i = 0; i < ui->moreLayout->count(); ++i) {
        QWidget* w = ui->moreLayout->itemAt(i)->widget();
        if (w != NULL)
            w->setVisible(false);
    }
    //读取可用串口
    fillPortInfo();
    fillPortsParameters();
    //加载最近一次参数
    loadSettings();
}

device_info::~device_info()
{
    qDebug()<<"~Device Information";
    delete ui;
}

void device_info::on_checkBox_stateChanged(int arg1)
{
    for (int i = 0; i < ui->moreLayout->count(); ++i) {
        QWidget* w = ui->moreLayout->itemAt(i)->widget();
        if (w != NULL)
            w->setVisible(arg1);
    }
}

void device_info::fillPortInfo()
{
    ui->comboBox_Port->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        ui->comboBox_Port->addItem(info.portName()+"   "+info.description());
    }
}

void device_info::fillPortsParameters()
{
    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

    //读取模块基本信息文件
    QFile loadFile(QStringLiteral("./ModelInfo.json"));
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save file.");
    }
    QByteArray saveData = loadFile.readAll();
    loadFile.close();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject json(loadDoc.object());
    QStringList modelNameList;
    if (json.contains("Model") && json["Model"].isArray())
    {
        QJsonArray modelArray = json["Model"].toArray();
        ui->modelBox->clear();
        foreach (QJsonValue iJson, modelArray)
        {
            QJsonObject modelObject = iJson.toObject();
//            ui->modelBox->addItem(modelObject["name"].toString());
            modelNameList.append(modelObject["name"].toString());
        }
    }
    modelNameList.sort();
    ui->modelBox->clear();
    ui->modelBox->addItems(modelNameList);
    ui->modelBox->setCurrentIndex(ui->modelBox->count());
    qDebug()<<modelNameList;
}

void device_info::saveSettings()
{

    QSettings settings;
    settings.setValue("COMPORT/SelectedModel", ui->modelBox->currentIndex());
    settings.setValue("COMPORT/SelectedCOM", ui->comboBox_Port->currentIndex());
    settings.setValue("COMPORT/SelectedBaudRate", ui->comboBox_Baudrate->currentIndex());
}

void device_info::loadSettings()
{
    QSettings settings;
    ui->modelBox->setCurrentIndex(settings.value("COMPORT/SelectedModel", 1).toInt());
    ui->comboBox_Port->setCurrentIndex(settings.value("COMPORT/SelectedCOM", 1).toInt());
    ui->comboBox_Baudrate->setCurrentIndex(settings.value("COMPORT/SelectedBaudRate", 1).toInt());
}

device_info::device_info_t device_info::getInfo_t() const
{
    return this->info_t;
}


void device_info::on_buttonBox_accepted()
{
    QMetaEnum curType = QMetaEnum::fromType<QGNSS_Model>();
    info_t.model_type = static_cast<QGNSS_Model>(curType.keyToValue(ui->modelBox->currentText().toLocal8Bit()));
    info_t.stringModelType = ui->modelBox->currentText();

    info_t.portName = ui->comboBox_Port->currentText().split(' ')[0];

    info_t.baudRate = ui->comboBox_Baudrate->currentText().toInt();
    info_t.stringBaudRate = ui->comboBox_Baudrate->currentText();

    info_t.dataBits = static_cast<QSerialPort::DataBits>(
        ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    info_t.stringDataBits = ui->dataBitsBox->currentText();

    info_t.parity = static_cast<QSerialPort::Parity>(
        ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    info_t.stringParity = ui->parityBox->currentText();

    info_t.stopBits = static_cast<QSerialPort::StopBits>(
        ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    info_t.stringStopBits = ui->stopBitsBox->currentText();
    // soft FlowControl specialization
    if (ui->flowControlBox->currentIndex() == 1)
    {
        info_t.flowControl = QSerialPort::HardwareControl;
    }
    else
    {
        info_t.flowControl = QSerialPort::NoFlowControl;
    }
    info_t.softFlowControl = ui->flowControlBox->currentIndex() == 2 ? true : false;

    saveSettings();
    qDebug()<<"buttonBox_accepted";
    this->accept();
}

bool device_info::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message == WM_DEVICECHANGE)
    {
        fillPortInfo();
//        fillPortsParameters();
    }
    return false;
}
