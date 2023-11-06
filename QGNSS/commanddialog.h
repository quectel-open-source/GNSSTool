/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         commanddialog.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-22      Dallas.xu
* ***************************************************************************/
#ifndef COMMANDDIALOG_H
#define COMMANDDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QCheckBox>
#include <QList>
#include <QLineEdit>
#include <QDir>

#include <QFileDialog>
#include <QTimer>
#include <QDateTime>

#include <QSettings>


#include "commandcheckbox.h"

#define D_COMMAND_NUM 21

namespace Ui {
class commandDialog;
}

class commandDialog : public QDialog
{
    Q_OBJECT

public:
    explicit commandDialog(QWidget *parent = nullptr);
    ~commandDialog();

    void update_command_cycle_list();
    void qsettingWrite();
    void qsettingRead(QString full_file_name);
    void qsettingINIlength();
    void insertData(int var);

    int g_checkBox;
    int id_Tmp;

signals:
    void sendData(const QByteArray &data);
    void sendData(const char * data, unsigned int size);
    void sendDTRStatu(bool);
    void sendRTSStatu(bool);

private slots:
    void on_pushButton_clicked();
    void on_Load_configuration_pushButton_clicked();
    void cycleTimer_Timeout();
    void on_sendButton_clicked();
    void on_chooseAll_stateChanged(int arg1);
    void on_pushButton_ClearAll_clicked();
    void on_checkBox_HexAll_stateChanged(int arg1);
    void on_pushButton_add_clicked();
    void on_checkBox_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void on_carriage_char_checkBox_stateChanged(int arg1);
    void on_pushButton_NMEACS_clicked();

private:
    Ui::commandDialog *ui;
    QTimer             m_cycleTimer ;        /* cycle timer   */
    int                m_cycleCurrentIndex=0;  /* current cycle index */
    QList<QCheckBox *> m_checkBox_List;
    QList<QLineEdit *> m_lineEdit_List;
    QList<QCheckBox *> m_HexCheckBox_List;
    QList<QCheckBox *> m_CRLFCheckBox_List;
    QList<QLineEdit *> m_delayTime_lineEdit_List;

};

#endif // COMMANDDIALOG_H
