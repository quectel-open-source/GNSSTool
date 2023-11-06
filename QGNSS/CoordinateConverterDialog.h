/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         CoordinateConverterDialog.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-10-12      Dallas.xu
* ***************************************************************************/
#ifndef COORDINATECONVERTERDIALOG_H
#define COORDINATECONVERTERDIALOG_H

#include <QDialog>

namespace Ui {
class CoordinateConverterDialog;
}

class CoordinateConverterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CoordinateConverterDialog(QWidget *parent = nullptr);
    ~CoordinateConverterDialog();
    void enableRadioButtonStstus();

private slots:
    void on_pushButton_Apply_clicked();

    void on_radioButton_LLHD_clicked();

    void on_radioButton_LLHDM_clicked();

    void on_radioButton_ECEF_clicked();

    void on_radioButton_LLHDMS_clicked();

    void on_pushButton_Clear_clicked();

private:
    Ui::CoordinateConverterDialog *ui;
};

#endif // COORDINATECONVERTERDIALOG_H
