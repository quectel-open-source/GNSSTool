#include "playercontrol.h"
#include "ui_playercontrol.h"
#include "mainwindow.h"

#include <QDebug>

PlayerControl::PlayerControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerControl)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, false);
//    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->comboBox_Rate->addItem("512B/s",512);
    ui->comboBox_Rate->addItem("1KB/s",1024);
    ui->comboBox_Rate->addItem("5KB/s",1024*5);
    ui->comboBox_Rate->addItem("10KB/s",1024*10);
    ui->comboBox_Rate->addItem("50KB/s",1024*50);
    ui->comboBox_Rate->addItem("100KB/s",1024*100);
    ui->comboBox_Rate->addItem("Max",1024*200);//200KB/s
//    ui->spinBox_Rate->setRange(128, 10240);
//    ui->spinBox_Rate->setSingleStep(128);
    m_QSlider = ui->horizontalSlider;
    m_QSlider->setRange(0,D_SLIDER_MAX_VALUE);
    m_QSlider->setSingleStep(1);
    style = QApplication::style();
    ui->pushButton_play->setIcon(style->standardIcon(QStyle::SP_MediaPlay));
//    ui->pushButton_playFulll->setIcon(style->standardIcon(QStyle::SP_MediaSeekForward));
    m_replayFile_Timer = new QTimer();
    connect(m_replayFile_Timer, SIGNAL(timeout()), this, SLOT(readFileEvent()));
    m_replayFile_Timer->setInterval(1000);
}

PlayerControl::~PlayerControl()
{
    delete ui;
    delete m_replayFile_Timer;
}

void PlayerControl::on_pushButton_exit_clicked()
{
    emit this->close();
}

void PlayerControl::readFileEvent()
{
    if(!m_replayFile.isOpen()){
        qDebug()<<"logFile is not open.";
        return;
    }
    m_QSlider->setValue(m_replayFile.pos() * D_SLIDER_MAX_VALUE / (m_replayFile.size()));
//    qDebug()<<m_replayFile.read(ui->spinBox_Rate->value());
    emit sendData2Parser(m_replayFile.read(ui->comboBox_Rate->currentData().toUInt()));
}


void PlayerControl::on_pushButton_play_toggled(bool checked)
{
    if(checked){
        m_replayFile_Timer->start();
        ui->pushButton_play->setIcon(style->standardIcon(QStyle::SP_MediaPause));
    }
    else{
        m_replayFile_Timer->stop();
        ui->pushButton_play->setIcon(style->standardIcon(QStyle::SP_MediaPlay));
    }
}

void PlayerControl::on_horizontalSlider_sliderPressed()
{
    m_replayFile_Timer->stop();
}

void PlayerControl::on_horizontalSlider_sliderReleased()
{
    if(ui->pushButton_play->isChecked()){
        m_replayFile_Timer->start();
    }
    if(ui->horizontalSlider->value()<D_SLIDER_MAX_VALUE){
        m_replayFile.seek(static_cast<qint64>(ui->horizontalSlider->value()/(D_SLIDER_MAX_VALUE*1.0) * m_replayFile.size()));
    }
}

bool PlayerControl::getLogFileName()
{
    QSettings settings(SETTINGS_FILE_NAME, QSettings::IniFormat);
    QString startFile = settings.value("LOG_INPUT_PATH", "./logFile").toString();
    QString replayFileName = QFileDialog::getOpenFileName(nullptr, tr("Select one log file"), startFile);
    if(replayFileName.isEmpty()){
        //TODO: tip
        qInfo()<<"log file name is empty";
        return false;
    }
    QFileInfo finfo(replayFileName);
    startFile = finfo.dir().absolutePath();
    settings.setValue("LOG_INPUT_PATH", startFile);

    m_replayFile.close();
    m_replayFile.setFileName(replayFileName);
    if(!m_replayFile.open(QFile::ReadOnly)){
        //TODO: tip
        return false;
    }
    if(m_replayFile.size() == 0){
        //TODO: tip
        return false;
    }
    isPlayMode = true;
    return true;
}

void PlayerControl::closeEvent(QCloseEvent *event)
{
    ui->pushButton_play->setChecked(false);
    m_replayFile_Timer->stop();
    m_replayFile.close();
    m_QSlider->setValue(0);
    isPlayMode = false;
    emit rejected();//recover series button
    event->accept();
}

bool PlayerControl::IsPlayMode() const
{
    return isPlayMode;
}
