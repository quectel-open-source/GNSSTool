/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         commandcheckbox.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-22      Dallas.xu
* ***************************************************************************/
#include "commandcheckbox.h"
#include "ui_commandcheckbox.h"
#include <QIntValidator>

CommandCheckBox::CommandCheckBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandCheckBox)
{
    ui->setupUi(this);
    QIntValidator* aIntValidator = new QIntValidator;
    aIntValidator->setRange(0, 100000000);
    ui->delayTime_lineEdit->setValidator(aIntValidator);
}

CommandCheckBox::~CommandCheckBox()
{
    delete ui;
}
void CommandCheckBox::setCheckBoxInfo(QString str)
{
    ui->cycle_checkBox->setText(str);
}

void CommandCheckBox::on_command_pushButton_clicked()
{
    //hex
    if( ui->checkBox_Hex->checkState())
    {
        QString string_tmp3 = " 0D 0A";
        if(ui->checkBox_CRLF->isChecked()){
            QString string_tmp4 = ui->command_LineEdit->text()+string_tmp3;
            QByteArray  text = QByteArray::fromHex(string_tmp4.toLatin1());
            emit sendCMD(text);

        }else{
            QByteArray  text = QByteArray::fromHex(ui->command_LineEdit->text().toLatin1());
            emit sendCMD(text);

        }

    }else{
        QString string_tmp1 = "\r\n";
        if(ui->checkBox_CRLF->isChecked()){
            QByteArray  text = (ui->command_LineEdit->text()+string_tmp1).toLatin1();
            emit sendCMD(text);

        }else{
            //  AscII
            QByteArray  text = ui->command_LineEdit->text().toLatin1();
            emit sendCMD(text);
        }
    }

}
