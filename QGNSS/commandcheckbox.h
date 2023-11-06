/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         commandcheckbox.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-22      Dallas.xu
* ***************************************************************************/
#ifndef COMMANDCHECKBOX_H
#define COMMANDCHECKBOX_H

#include <QWidget>
#include <QCheckBox>
#include <QList>
#include <QLineEdit>
#include <QDebug>

namespace Ui {
class CommandCheckBox;
}

class CommandCheckBox : public QWidget
{
    Q_OBJECT

public:
    explicit CommandCheckBox(QWidget *parent = nullptr);
    ~CommandCheckBox();
    void setCheckBoxInfo(QString str);

signals:
    void sendCMD(const char * data, unsigned int size);
    void sendCMD(const QByteArray &data);

private slots:
    void on_command_pushButton_clicked();

private:
    Ui::CommandCheckBox *ui;

};

#endif // COMMANDCHECKBOX_H
