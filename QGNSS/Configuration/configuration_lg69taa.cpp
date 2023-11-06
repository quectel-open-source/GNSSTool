#include "configuration_lg69taa.h"
#include "ui_configuration_lg69taa.h"
#include "mainwindow.h"
#include <RTKLib/rtklib.h>

configuration_LG69TAA::configuration_LG69TAA(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_LG69TAA)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowTitle("Configuration LG69TAA");
}

configuration_LG69TAA::~configuration_LG69TAA()
{
    delete ui;
}

void configuration_LG69TAA::on_pushButton_pollVersion_clicked()
{
    ui->lineEdit_version->setText(lg96taa_version);
    struct {
            unsigned MessageNumber:12;
            unsigned SubTypeID:8;
            unsigned ResponseID:10;
            unsigned RetransmissionMessageID:8;
            unsigned PageNumber:8;
            unsigned PageMask:16;
    }TXREQ;
    TXREQ.MessageNumber = 999;
    TXREQ.SubTypeID = 18;
    TXREQ.ResponseID = 0;
    TXREQ.RetransmissionMessageID = 24;
    TXREQ.PageNumber = 255;
    TXREQ.PageMask = 0;
    int size = sizeof (TXREQ);
    QByteArray cmd;
//    cmd.append(TXREQ,sizeof (TXREQ));
    cmd.append((char *)&TXREQ,size);
    qDebug()<<cmd;
//    mainForm->m_serial->write("aaa");
}
