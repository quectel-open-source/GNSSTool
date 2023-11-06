#include "configuration_p1.h"
#include "ui_configuration_p1.h"
#include "QDebug"

using namespace point_one::fusion_engine::messages;

Q_DECLARE_METATYPE(ConfigType);
Q_DECLARE_METATYPE(CoarseOrientation::Direction);
Q_DECLARE_METATYPE(SaveAction);
Q_DECLARE_METATYPE(AppliedSpeedType);
Q_DECLARE_METATYPE(WheelSensorType);
Q_DECLARE_METATYPE(SteeringType);
Q_DECLARE_METATYPE(TransportType);
Q_DECLARE_METATYPE(MessageType);
Q_DECLARE_METATYPE(ProtocolType);
Q_DECLARE_METATYPE(MessageRate);
Q_DECLARE_METATYPE(TickDirection);
Q_DECLARE_METATYPE(TickMode);

configuration_P1::configuration_P1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_P1),
    statusBar(new QStatusBar(this))
{
    ui->setupUi(this);
    ui->treeWidget->hideColumn(1);

    QTreeWidgetItemIterator It(ui->treeWidget);
    while(*It)
    {
        if((*It)->text(1).isEmpty()){
            (*It)->setHidden(true);
        }
        It++;
    }
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Configure dialog");
    ui->statusBarLayout->addWidget(statusBar);
    statusBar->showMessage("No message");
    m_Header.source_identifier = 0;

    ui->comboBox_UDE->addItem("UART1", QVariant::fromValue(ConfigType::UART1_OUTPUT_DIAGNOSTICS_MESSAGES));
    ui->comboBox_UDE->addItem("UART2", QVariant::fromValue(ConfigType::UART2_OUTPUT_DIAGNOSTICS_MESSAGES));
    ui->comboBox_port->addItem("UART1", QVariant::fromValue(ConfigType::UART1_BAUD));
    ui->comboBox_port->addItem("UART2", QVariant::fromValue(ConfigType::UART2_BAUD));
    ui->comboBox_x_direction->addItem("FORWARD", QVariant::fromValue(CoarseOrientation::Direction::FORWARD));
    ui->comboBox_x_direction->addItem("BACKWARD", QVariant::fromValue(CoarseOrientation::Direction::BACKWARD));
    ui->comboBox_x_direction->addItem("LEFT", QVariant::fromValue(CoarseOrientation::Direction::LEFT));
    ui->comboBox_x_direction->addItem("RIGHT", QVariant::fromValue(CoarseOrientation::Direction::RIGHT));
    ui->comboBox_x_direction->addItem("UP", QVariant::fromValue(CoarseOrientation::Direction::UP));
    ui->comboBox_x_direction->addItem("DOWN", QVariant::fromValue(CoarseOrientation::Direction::DOWN));

    ui->comboBox_z_direction->addItem("FORWARD", QVariant::fromValue(CoarseOrientation::Direction::FORWARD));
    ui->comboBox_z_direction->addItem("BACKWARD", QVariant::fromValue(CoarseOrientation::Direction::BACKWARD));
    ui->comboBox_z_direction->addItem("LEFT", QVariant::fromValue(CoarseOrientation::Direction::LEFT));
    ui->comboBox_z_direction->addItem("RIGHT", QVariant::fromValue(CoarseOrientation::Direction::RIGHT));
    ui->comboBox_z_direction->addItem("UP", QVariant::fromValue(CoarseOrientation::Direction::UP));
    ui->comboBox_z_direction->addItem("DOWN", QVariant::fromValue(CoarseOrientation::Direction::DOWN));
    ui->comboBox_z_direction->setCurrentIndex((int)CoarseOrientation::Direction::UP);

    ui->comboBox_SaveOption->addItem(to_string(SaveAction::SAVE), QVariant::fromValue(SaveAction::SAVE));
    ui->comboBox_SaveOption->addItem(to_string(SaveAction::REVERT_TO_SAVED), QVariant::fromValue(SaveAction::REVERT_TO_SAVED));
    ui->comboBox_SaveOption->addItem(to_string(SaveAction::REVERT_TO_DEFAULT), QVariant::fromValue(SaveAction::REVERT_TO_DEFAULT));

    ui->buttonGroup_special_reset->setId(ui->radioButton_hot, ResetRequest::HOT_START);
    ui->buttonGroup_special_reset->setId(ui->radioButton_cold, ResetRequest::COLD_START);
    ui->buttonGroup_special_reset->setId(ui->radioButton_warm, ResetRequest::WARM_START);
    ui->buttonGroup_special_reset->setId(ui->radioButton_factory, ResetRequest::FACTORY_RESET);

    ui->tableWidget_vechicleModel->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->comboBox_WS_type->addItem(to_string(WheelSensorType::NONE), QVariant::fromValue(WheelSensorType::NONE));
    ui->comboBox_WS_type->addItem(to_string(WheelSensorType::TICKS), QVariant::fromValue(WheelSensorType::TICKS));
    ui->comboBox_WS_type->addItem(to_string(WheelSensorType::TICK_RATE), QVariant::fromValue(WheelSensorType::TICK_RATE));
    ui->comboBox_WS_type->addItem(to_string(WheelSensorType::WHEEL_SPEED), QVariant::fromValue(WheelSensorType::WHEEL_SPEED));
    ui->comboBox_WS_type->addItem(to_string(WheelSensorType::VEHICLE_SPEED), QVariant::fromValue(WheelSensorType::VEHICLE_SPEED));

    ui->comboBox_AS_type->addItem(to_string(AppliedSpeedType::NONE), QVariant::fromValue(AppliedSpeedType::NONE));
    ui->comboBox_AS_type->addItem(to_string(AppliedSpeedType::REAR_WHEELS), QVariant::fromValue(AppliedSpeedType::REAR_WHEELS));
    ui->comboBox_AS_type->addItem(to_string(AppliedSpeedType::FRONT_WHEELS), QVariant::fromValue(AppliedSpeedType::FRONT_WHEELS));
    ui->comboBox_AS_type->addItem(to_string(AppliedSpeedType::FRONT_AND_REAR_WHEELS), QVariant::fromValue(AppliedSpeedType::FRONT_AND_REAR_WHEELS));
    ui->comboBox_AS_type->addItem(to_string(AppliedSpeedType::VEHICLE_BODY), QVariant::fromValue(AppliedSpeedType::VEHICLE_BODY));

    ui->comboBox_SteeringType->addItem(to_string(SteeringType::UNKNOWN), QVariant::fromValue(SteeringType::UNKNOWN));
    ui->comboBox_SteeringType->addItem(to_string(SteeringType::FRONT), QVariant::fromValue(SteeringType::FRONT));
    ui->comboBox_SteeringType->addItem(to_string(SteeringType::FRONT_AND_REAR), QVariant::fromValue(SteeringType::FRONT_AND_REAR));

    ui->comboBox_transportType->addItem(to_string(TransportType::CURRENT), QVariant::fromValue(TransportType::CURRENT));
    ui->comboBox_transportType->addItem(to_string(TransportType::ALL), QVariant::fromValue(TransportType::ALL));
    ui->comboBox_transportType->addItem(to_string(TransportType::FILE), QVariant::fromValue(TransportType::FILE));
    ui->comboBox_transportType->addItem(to_string(TransportType::SERIAL), QVariant::fromValue(TransportType::SERIAL));
    ui->comboBox_transportType->addItem(to_string(TransportType::TCP_CLIENT), QVariant::fromValue(TransportType::TCP_CLIENT));
    ui->comboBox_transportType->addItem(to_string(TransportType::TCP_SERVER), QVariant::fromValue(TransportType::TCP_SERVER));
    ui->comboBox_transportType->addItem(to_string(TransportType::UDP_CLIENT), QVariant::fromValue(TransportType::UDP_CLIENT));
    ui->comboBox_transportType->addItem(to_string(TransportType::UDP_SERVER), QVariant::fromValue(TransportType::UDP_SERVER));

    ui->comboBox_protocolType->addItem(to_string(ProtocolType::ALL), QVariant::fromValue(ProtocolType::ALL));
    ui->comboBox_protocolType->addItem(to_string(ProtocolType::NMEA), QVariant::fromValue(ProtocolType::NMEA));
    ui->comboBox_protocolType->addItem(to_string(ProtocolType::RTCM), QVariant::fromValue(ProtocolType::RTCM));
    ui->comboBox_protocolType->addItem(to_string(ProtocolType::FUSION_ENGINE), QVariant::fromValue(ProtocolType::FUSION_ENGINE));
    ui->comboBox_protocolType->addItem(to_string(ProtocolType::INVALID), QVariant::fromValue(ProtocolType::INVALID));

    ui->comboBox_msgRate->addItem(to_string(MessageRate::OFF), QVariant::fromValue(MessageRate::OFF));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::DEFAULT), QVariant::fromValue(MessageRate::DEFAULT));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::MAX_RATE), QVariant::fromValue(MessageRate::MAX_RATE));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::ON_CHANGE), QVariant::fromValue(MessageRate::ON_CHANGE));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_1_S), QVariant::fromValue(MessageRate::INTERVAL_1_S));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_2_S), QVariant::fromValue(MessageRate::INTERVAL_2_S));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_5_S), QVariant::fromValue(MessageRate::INTERVAL_5_S));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_10_S), QVariant::fromValue(MessageRate::INTERVAL_10_S));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_10_MS), QVariant::fromValue(MessageRate::INTERVAL_10_MS));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_20_MS), QVariant::fromValue(MessageRate::INTERVAL_20_MS));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_40_MS), QVariant::fromValue(MessageRate::INTERVAL_40_MS));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_50_MS), QVariant::fromValue(MessageRate::INTERVAL_50_MS));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_100_MS), QVariant::fromValue(MessageRate::INTERVAL_100_MS));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_200_MS), QVariant::fromValue(MessageRate::INTERVAL_200_MS));
    ui->comboBox_msgRate->addItem(to_string(MessageRate::INTERVAL_500_MS), QVariant::fromValue(MessageRate::INTERVAL_500_MS));

    ui->comboBox_msgType->addItem(to_string(MessageType::VERSION_INFO), QVariant::fromValue(MessageType::VERSION_INFO));

    ui->comboBox_tickMode->addItem(to_string(TickMode::OFF), QVariant::fromValue(TickMode::OFF));
    ui->comboBox_tickMode->addItem(to_string(TickMode::RISING_EDGE), QVariant::fromValue(TickMode::RISING_EDGE));
    ui->comboBox_tickMode->addItem(to_string(TickMode::FALLING_EDGE), QVariant::fromValue(TickMode::FALLING_EDGE));
    ui->comboBox_tickDir->addItem(to_string(TickDirection::OFF), QVariant::fromValue(TickDirection::OFF));
    ui->comboBox_tickDir->addItem(to_string(TickDirection::FORWARD_ACTIVE_LOW), QVariant::fromValue(TickDirection::FORWARD_ACTIVE_LOW));
    ui->comboBox_tickDir->addItem(to_string(TickDirection::FORWARD_ACTIVE_HIGH), QVariant::fromValue(TickDirection::FORWARD_ACTIVE_HIGH));
 }

configuration_P1::~configuration_P1()
{
    delete ui;
}

void configuration_P1::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>(current->text(1))[0]);
}

void configuration_P1::updateUI(QByteArray msg)
{
    MessageHeader *msgHeader = reinterpret_cast<MessageHeader *>(msg.data());
    ConfigResponseMessage *configRsp= reinterpret_cast<ConfigResponseMessage *>(&msg.data()[sizeof(MessageHeader)]);

    switch (msgHeader->message_type) {
    case MessageType::CONFIG_RESPONSE:
        if(configRsp->config_type == ConfigType::DEVICE_LEVER_ARM){
            float *asia = reinterpret_cast<float *>(configRsp->config_change_data);
            QString numString = QString::number(*asia);
            ui->lineEdit_DX->setText(QString::number(*asia));
            ui->lineEdit_DY->setText(QString::number(*(asia+1)));
            ui->lineEdit_DZ->setText(QString::number(*(asia+2)));
            statusBar->showMessage(tr("Poll the device location successfully."),3000);
        }
        else if(configRsp->config_type == ConfigType::GNSS_LEVER_ARM){
            float *asia = reinterpret_cast<float *>(configRsp->config_change_data);
            QString numString = QString::number(*asia);
            ui->lineEdit_AX->setText(QString::number(*asia));
            ui->lineEdit_AY->setText(QString::number(*(asia+1)));
            ui->lineEdit_AZ->setText(QString::number(*(asia+2)));
            statusBar->showMessage(tr("Poll the antenna location location successfully."),3000);
        }
        else if(configRsp->config_type == ConfigType::OUTPUT_LEVER_ARM){
            float *asia = reinterpret_cast<float *>(configRsp->config_change_data);
            QString numString = QString::number(*asia);
            ui->lineEdit_OX->setText(QString::number(*asia));
            ui->lineEdit_OY->setText(QString::number(*(asia+1)));
            ui->lineEdit_OZ->setText(QString::number(*(asia+2)));
            statusBar->showMessage(tr("Poll the 'OUTPUT location' successfully."),3000);
        }
        else if(configRsp->config_type == ConfigType::DEVICE_COARSE_ORIENTATION){
            uint8_t *orientation = reinterpret_cast<uint8_t *>(configRsp->config_change_data);
            ui->comboBox_x_direction->setCurrentIndex(*orientation);
            ui->comboBox_z_direction->setCurrentIndex(*(orientation+1));
            statusBar->showMessage(tr("Poll the device coarse orientation successfully."),3000);
        }
        break;
    default:
        break;
    }
}

void configuration_P1::on_GC_baudrate_clicked()
{
    //change data
    uint32_t baudrate = ui->lineEdit_baudrate->text().toUInt();

    m_setConfigPayload.config_length_bytes = sizeof(baudrate);
    m_setConfigPayload.config_type = ui->comboBox_port->currentData().value<ConfigType>();

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(baudrate) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&baudrate, sizeof(baudrate));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&baudrate, sizeof(baudrate));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

uint32_t configuration_P1::_crc32(const char *change_data, uint32_t lenght)
{
    QByteArray need_crc_part;
    need_crc_part.append(reinterpret_cast<char *>(&(m_Header.protocol_version)), sizeof(m_Header) - D_CRC_OFFSET);
    need_crc_part.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    need_crc_part.append(change_data, lenght);
    qDebug()<<"configuration_P1 need_crc_part: "<<need_crc_part.toHex(' ').toUpper();
    return c_crc32((uint8_t *)need_crc_part.constData(), need_crc_part.size());
}

void configuration_P1::on_pushButton_send_clicked()
{
    QByteArray text = QByteArray::fromHex(ui->lineEditCommand->text().toLocal8Bit());
    emit sendData(text);
}

void configuration_P1::on_GC_DeviceOrientation_clicked()
{
    //change data
    CoarseOrientation ori;
    ori.x_direction = ui->comboBox_x_direction->currentData().value<CoarseOrientation::Direction>();
    ori.z_direction = ui->comboBox_z_direction->currentData().value<CoarseOrientation::Direction>();

    m_setConfigPayload.config_length_bytes = sizeof(ori);
    m_setConfigPayload.config_type = ConfigType::DEVICE_COARSE_ORIENTATION;

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(ori) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&ori, sizeof(ori));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&ori, sizeof(ori));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_GC_DeviceLocation_clicked()
{
    //change data
    Point3f devLoction;
    devLoction.x = ui->lineEdit_DX->text().toFloat();
    devLoction.y = ui->lineEdit_DY->text().toFloat();
    devLoction.z = ui->lineEdit_DZ->text().toFloat();

    m_setConfigPayload.config_length_bytes = sizeof(devLoction);
    m_setConfigPayload.config_type = ConfigType::DEVICE_LEVER_ARM;

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(devLoction) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&devLoction, sizeof(devLoction));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&devLoction, sizeof(devLoction));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_GC_AntLocation_clicked()
{
    //change data
    Point3f antLoction;
    antLoction.x = ui->lineEdit_AX->text().toFloat();
    antLoction.y = ui->lineEdit_AY->text().toFloat();
    antLoction.z = ui->lineEdit_AZ->text().toFloat();

    m_setConfigPayload.config_length_bytes = sizeof(antLoction);
    m_setConfigPayload.config_type = ConfigType::GNSS_LEVER_ARM;

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(antLoction) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&antLoction, sizeof(antLoction));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&antLoction, sizeof(antLoction));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}


void configuration_P1::on_GC_SaveSettings_clicked()
{
    SaveConfigMessage saveConfig;
    saveConfig.action = ui->comboBox_SaveOption->currentData().value<SaveAction>();
    m_Header.payload_size_bytes = sizeof(saveConfig);
    m_Header.message_type = saveConfig.MESSAGE_TYPE;

    QByteArray need_crc_part;
    need_crc_part.append(reinterpret_cast<char *>(&(m_Header.protocol_version)), sizeof(m_Header) - D_CRC_OFFSET);
    need_crc_part.append((char*)&saveConfig, sizeof(saveConfig));
    qDebug()<<"configuration_P1 need_crc_part: "<<need_crc_part.toHex(' ').toUpper();

    m_Header.crc = c_crc32((uint8_t *)need_crc_part.constData(), need_crc_part.size());
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&saveConfig, sizeof(saveConfig));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_PL_AntLocation_clicked()
{
    GetConfigMessage get;
    get.config_type = ConfigType::GNSS_LEVER_ARM;
    m_Header.payload_size_bytes = sizeof(get);
    m_Header.message_type = get.MESSAGE_TYPE;

    emit sendData(GenerateCMD(QByteArray((char*)&get,sizeof(get))));
}

QByteArray configuration_P1::GenerateCMD(QByteArray payload)
{
    QByteArray need_crc_part;
    need_crc_part.append(reinterpret_cast<char *>(&(m_Header.protocol_version)), sizeof(m_Header) - D_CRC_OFFSET);
    need_crc_part.append(payload);
    qDebug()<<"configuration_P1 need_crc_part: "<<need_crc_part.toHex(' ').toUpper();

    m_Header.crc = c_crc32((uint8_t *)need_crc_part.constData(), need_crc_part.size());
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append(payload);
    return cmd;
}

void configuration_P1::on_GC_OutputLocation_clicked()
{
    //change data
    Point3f Loction;
    Loction.x = ui->lineEdit_OX->text().toFloat();
    Loction.y = ui->lineEdit_OY->text().toFloat();
    Loction.z = ui->lineEdit_OZ->text().toFloat();

    m_setConfigPayload.config_length_bytes = sizeof(Loction);
    m_setConfigPayload.config_type = ConfigType::OUTPUT_LEVER_ARM;

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(Loction) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&Loction, sizeof(Loction));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&Loction, sizeof(Loction));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_PL_DevOrientation_clicked()
{
    GetConfigMessage get;
    get.config_type = ConfigType::DEVICE_COARSE_ORIENTATION;
    m_Header.payload_size_bytes = sizeof(get);
    m_Header.message_type = get.MESSAGE_TYPE;
    emit sendData(GenerateCMD(QByteArray((char*)&get,sizeof(get))));
}

void configuration_P1::on_PL_OutputLocation_clicked()
{
    GetConfigMessage get;
    get.config_type = ConfigType::OUTPUT_LEVER_ARM;
    m_Header.payload_size_bytes = sizeof(get);
    m_Header.message_type = get.MESSAGE_TYPE;
    emit sendData(GenerateCMD(QByteArray((char*)&get,sizeof(get))));
}

void configuration_P1::on_PL_DevLocation_clicked()
{
    GetConfigMessage get;
    get.config_type = ConfigType::DEVICE_LEVER_ARM;
    m_Header.payload_size_bytes = sizeof(get);
    m_Header.message_type = get.MESSAGE_TYPE;
    emit sendData(GenerateCMD(QByteArray((char*)&get,sizeof(get))));
}

void configuration_P1::on_groupBox_cunstomReset_clicked(bool checked)
{
    if(checked)
        ui->groupBox_specialReset->setChecked(false);
}

void configuration_P1::on_groupBox_specialReset_clicked(bool checked)
{
    if(checked)
        ui->groupBox_cunstomReset->setChecked(false);
}

void configuration_P1::on_GC_reset_clicked()
{
    ResetRequest reset;
    reset.reset_mask = 0;
    if(ui->groupBox_cunstomReset->isChecked()){
        reset.reset_mask += ui->checkBox_reset_0->isChecked()?ResetRequest::RESTART_NAVIGATION_ENGINE:0;
        reset.reset_mask += ui->checkBox_reset_1->isChecked()?ResetRequest::RESET_GNSS_CORRECTIONS:0;
        reset.reset_mask += ui->checkBox_reset_8->isChecked()?ResetRequest::RESET_POSITION_DATA:0;
        reset.reset_mask += ui->checkBox_reset_9->isChecked()?ResetRequest::RESET_EPHEMERIS:0;
        reset.reset_mask += ui->checkBox_reset_10->isChecked()?ResetRequest::RESET_FAST_IMU_CORRECTIONS:0;
        reset.reset_mask += ui->checkBox_reset_12->isChecked()?ResetRequest::RESET_NAVIGATION_ENGINE_DATA:0;
        reset.reset_mask += ui->checkBox_reset_13->isChecked()?ResetRequest::RESET_CALIBRATION_DATA:0;
        reset.reset_mask += ui->checkBox_reset_20->isChecked()?ResetRequest::RESET_CONFIG:0;
        reset.reset_mask += ui->checkBox_reset_24->isChecked()?ResetRequest::RESTART_GNSS_MEASUREMENT_ENGINE:0;

    }else if(ui->groupBox_specialReset->isChecked()) {
        reset.reset_mask = ui->buttonGroup_special_reset->checkedId();
        // The read value is-5 when id equals 0xffffffff
        if(ui->radioButton_factory->isChecked())
            reset.reset_mask = ResetRequest::FACTORY_RESET;
    }
    m_Header.payload_size_bytes = sizeof(ResetRequest);
    m_Header.message_type = reset.MESSAGE_TYPE;

    QByteArray need_crc_part;
    need_crc_part.append(reinterpret_cast<char *>(&(m_Header.protocol_version)), sizeof(m_Header) - D_CRC_OFFSET);
    need_crc_part.append((char*)&reset, sizeof(reset));
    qDebug()<<"configuration_P1 need_crc_part: "<<need_crc_part.toHex(' ').toUpper();

    m_Header.crc = c_crc32((uint8_t *)need_crc_part.constData(), need_crc_part.size());
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&reset, sizeof(reset));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_GC_vehicleDetails_clicked()
{
    VehicleDetails vehicle;
    vehicle.vehicle_model = static_cast<VehicleModel>(ui->lineEdit_model->text().toUShort());
    vehicle.wheelbase_m = ui->lineEdit_wheelbase->text().toFloat();
    vehicle.rear_track_width_m = ui->lineEdit_rearTrackWidth->text().toFloat();
    vehicle.front_track_width_m = ui->lineEdit_frontTrackWidth->text().toFloat();

    m_setConfigPayload.config_length_bytes = sizeof(VehicleDetails);
    m_setConfigPayload.config_type = ConfigType::VEHICLE_DETAILS;

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(VehicleDetails) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&vehicle, sizeof(vehicle));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&vehicle, sizeof(vehicle));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_GC_wheelMeasurement_clicked()
{
    WheelConfig wheel;
    wheel.wheel_sensor_type = ui->comboBox_WS_type->currentData().value<WheelSensorType>();
    wheel.steering_type = ui->comboBox_SteeringType->currentData().value<SteeringType>();
    wheel.applied_speed_type = ui->comboBox_AS_type->currentData().value<AppliedSpeedType>();
    wheel.wheel_update_interval_sec = ui->lineEdit_whellUpInt->text().toFloat();
    wheel.steering_ratio = ui->lineEdit_steeringRatio->text().toFloat();
    wheel.wheel_ticks_to_m = ui->lineEdit_tickScale->text().toFloat();
    wheel.wheel_tick_max_value = ui->lineEdit_wTickMaxValue->text().toUInt();
    wheel.wheel_ticks_signed = ui->lineEdit_wTicksSigned->text().toUInt();
    wheel.wheel_ticks_always_increase = ui->lineEdit_wTicksAlwaysInc->text().toUInt();

    m_setConfigPayload.config_length_bytes = sizeof(WheelConfig);
    m_setConfigPayload.config_type = ConfigType::WHEEL_CONFIG;

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(VehicleDetails) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&wheel, sizeof(wheel));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&wheel, sizeof(wheel));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

//void configuration_P1::on_PL_outputInterface_clicked()
//{
//    GetOutputInterfaceConfigMessage get;
//    get.output_interface.type = TransportType::ALL;
//    get.output_interface.index = ui->lineEdit->text().toUInt();
//    m_Header.payload_size_bytes = sizeof(get);
//    m_Header.message_type = get.MESSAGE_TYPE;

//    QByteArray need_crc_part;
//    need_crc_part.append(reinterpret_cast<char *>(&(m_Header.protocol_version)), sizeof(m_Header) - D_CRC_OFFSET);
//    need_crc_part.append((char*)&get, sizeof(get));
//    qDebug()<<"configuration_P1 need_crc_part: "<<need_crc_part.toHex(' ').toUpper();

//    m_Header.crc = c_crc32((uint8_t *)need_crc_part.constData(), need_crc_part.size());
//    QByteArray cmd;
//    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
//    cmd.append((char*)&get, sizeof(get));
//    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
//}

void configuration_P1::on_GC_setOutput_clicked()
{
    SetMessageRate output;
    output.output_interface.type = ui->comboBox_transportType->currentData().value<TransportType>();
    output.output_interface.index = ui->lineEdit_portIndex->text().toUInt();

    output.protocol = ui->comboBox_protocolType->currentData().value<ProtocolType>();

    output.flags += ui->checkBox_output_0->isChecked()?SetMessageRate::FLAG_APPLY_AND_SAVE:0;
    output.flags += ui->checkBox_output_1->isChecked()?SetMessageRate::FLAG_INCLUDE_DISABLED_MESSAGES:0;

    output.rate = ui->comboBox_msgRate->currentData().value<MessageRate>();

    output.message_id = ui->lineEdit_msgID->text().toUShort();

    m_Header.payload_size_bytes = sizeof(output);
    m_Header.message_type = output.MESSAGE_TYPE;

    QByteArray need_crc_part;
    need_crc_part.append(reinterpret_cast<char *>(&(m_Header.protocol_version)), sizeof(m_Header) - D_CRC_OFFSET);
    need_crc_part.append((char*)&output, sizeof(output));
    qDebug()<<"configuration_P1 need_crc_part: "<<need_crc_part.toHex(' ').toUpper();

    m_Header.crc = c_crc32((uint8_t *)need_crc_part.constData(), need_crc_part.size());
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&output, sizeof(output));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::setModel_type(const quectel_gnss::QGNSS_Model &value)
{
//    model_type = value;
//    QString titleName="Configuration:LG69TAQ";
//    auto h = ui->listWidget->findItems("Vehicle details", Qt::MatchContains);
//    if(value == quectel_gnss::QGNSS_Model::LG69TAM){
//        titleName="Configuration:LG69TAM";
//        h.append(ui->listWidget->findItems("Device location", Qt::MatchContains));
//        h.append(ui->listWidget->findItems("Device orientation", Qt::MatchContains));
//        h.append(ui->listWidget->findItems("Output location", Qt::MatchContains));
//        h.append(ui->listWidget->findItems("Antenna location", Qt::MatchContains));
//        h.append(ui->listWidget->findItems("Wheel measurement", Qt::MatchContains));
//    }
//    this->setWindowTitle(titleName);
//    for (auto item : h) {
//        item->setHidden(true);
//    }
}


void configuration_P1::on_GC_13001_clicked()
{
    MessageRequest req;
    req.message_type = ui->comboBox_msgType->currentData().value<MessageType>();
    m_Header.payload_size_bytes = sizeof(req);
    m_Header.message_type = req.MESSAGE_TYPE;

    QByteArray need_crc_part;
    need_crc_part.append(reinterpret_cast<char *>(&(m_Header.protocol_version)), sizeof(m_Header) - D_CRC_OFFSET);
    need_crc_part.append((char*)&req, sizeof(req));
    qDebug()<<"configuration_P1 need_crc_part: "<<need_crc_part.toHex(' ').toUpper();

    m_Header.crc = c_crc32((uint8_t *)need_crc_part.constData(), need_crc_part.size());
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&req, sizeof(req));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_GC_shutdown_clicked()
{
    ShutdownRequest shutdown;
    m_Header.payload_size_bytes = sizeof(shutdown);
    m_Header.message_type = shutdown.MESSAGE_TYPE;

    QByteArray need_crc_part;
    need_crc_part.append(reinterpret_cast<char *>(&(m_Header.protocol_version)), sizeof(m_Header) - D_CRC_OFFSET);
    need_crc_part.append((char*)&shutdown, sizeof(shutdown));
    qDebug()<<"configuration_P1 need_crc_part: "<<need_crc_part.toHex(' ').toUpper();

    m_Header.crc = c_crc32((uint8_t *)need_crc_part.constData(), need_crc_part.size());
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&shutdown, sizeof(shutdown));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_GC_HWTC_clicked()
{
    HardwareTickConfig hwtc;
    hwtc.tick_mode = ui->comboBox_tickMode->currentData().value<TickMode>();
    hwtc.tick_direction = ui->comboBox_tickDir->currentData().value<TickDirection>();
    hwtc.wheel_ticks_to_m = ui->lineEdit_tickScaleF->text().toFloat();

    m_setConfigPayload.config_length_bytes = sizeof(hwtc);
    m_setConfigPayload.config_type = ConfigType::HARDWARE_TICK_CONFIG;

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(hwtc) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&hwtc, sizeof(hwtc));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&hwtc, sizeof(hwtc));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_GC_UDE_clicked()
{
    bool isEnable = ui->checkBox_UDE->isChecked();
    m_setConfigPayload.config_length_bytes = sizeof(isEnable);
    m_setConfigPayload.config_type = ui->comboBox_UDE->currentData().value<ConfigType>();

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(isEnable) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&isEnable, sizeof(isEnable));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&isEnable, sizeof(isEnable));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}

void configuration_P1::on_GC_WDogEnable_clicked()
{
    bool isEnable = ui->checkBox_WDogEnable->isChecked();
    m_setConfigPayload.config_length_bytes = sizeof(isEnable);
    m_setConfigPayload.config_type = ConfigType::ENABLE_WATCHDOG_TIMER;

    m_Header.message_type = m_setConfigPayload.MESSAGE_TYPE;
    m_Header.payload_size_bytes = sizeof(isEnable) + sizeof(m_setConfigPayload);
    m_Header.crc = _crc32((char*)&isEnable, sizeof(isEnable));
    QByteArray cmd;
    cmd.append(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
    cmd.append((char*)&m_setConfigPayload, sizeof(m_setConfigPayload));
    cmd.append((char*)&isEnable, sizeof(isEnable));
    ui->lineEditCommand->setText(cmd.toHex(' ').toUpper());
}
