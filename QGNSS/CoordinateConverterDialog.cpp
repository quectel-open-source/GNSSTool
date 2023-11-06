/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         CoordinateConverterDialog.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-10-12      Dallas.xu
* ***************************************************************************/
#include "CoordinateConverterDialog.h"
#include "ui_CoordinateConverterDialog.h"
#include "mainwindow.h"
#include "RTKLib/rtklib.h"
#include <QRegExp>

CoordinateConverterDialog::CoordinateConverterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoordinateConverterDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Coordinate Converter");

    QRegExp exp("[0-9\\.-]+$");
    QValidator *Validator = new QRegExpValidator(exp);
    ui->lineEdit_DLat  ->setValidator(Validator);
    ui->lineEdit_DLon  ->setValidator(Validator);
    ui->lineEdit_DHgt  ->setValidator(Validator);
    ui->lineEdit_LDLat ->setValidator(Validator);
    ui->lineEdit_LDLon ->setValidator(Validator);
    ui->lineEdit_LDHgt ->setValidator(Validator);
    ui->lineEdit_LMLat ->setValidator(Validator);
    ui->lineEdit_LMLon ->setValidator(Validator);
    ui->lineEdit_LHDLat->setValidator(Validator);
    ui->lineEdit_LHDLon->setValidator(Validator);
    ui->lineEdit_LHDHgt->setValidator(Validator);
    ui->lineEdit_LHMLat->setValidator(Validator);
    ui->lineEdit_LHMLon->setValidator(Validator);
    ui->lineEdit_LHSLat->setValidator(Validator);
    ui->lineEdit_LHSLon->setValidator(Validator);
    ui->lineEdit_X     ->setValidator(Validator);
    ui->lineEdit_Y     ->setValidator(Validator);
    ui->lineEdit_Z     ->setValidator(Validator);
}

CoordinateConverterDialog::~CoordinateConverterDialog()
{
    delete ui;
}

void CoordinateConverterDialog::on_pushButton_Apply_clicked()
{
    if(ui->radioButton_LLHD->isChecked()&&(ui->lineEdit_DLat->text().count()||ui->lineEdit_DLon->text().count()||ui->lineEdit_DHgt->text().count()))
    {
        double pos[3]={0};
        double ecef[3]={0};
        pos[0] = ui->lineEdit_DLat->text().toDouble()*D2R;
        pos[1] = ui->lineEdit_DLon->text().toDouble()*D2R;
        pos[2] = ui->lineEdit_DHgt->text().toDouble();
        pos2ecef(pos, ecef);
        ui->lineEdit_X->setText(QString::number(ecef[0], 'f', 2));
        ui->lineEdit_Y->setText(QString::number(ecef[1], 'f', 2));
        ui->lineEdit_Z->setText(QString::number(ecef[2], 'f', 2));
        ui->lineEdit_LDLat ->setText(QString::number((int)ui->lineEdit_DLat->text().toDouble()));
        ui->lineEdit_LMLat ->setText(QString::number((ui->lineEdit_DLat->text().toDouble()-(int)ui->lineEdit_DLat->text().toDouble())*60, 'f', 8));
        ui->lineEdit_LDLon ->setText(QString::number((int)ui->lineEdit_DLon->text().toDouble()));
        ui->lineEdit_LMLon ->setText(QString::number((ui->lineEdit_DLon->text().toDouble()-(int)ui->lineEdit_DLon->text().toDouble())*60, 'f', 8));
        ui->lineEdit_LDHgt ->setText(ui->lineEdit_DHgt ->text());
        ui->lineEdit_LHDLat->setText(ui->lineEdit_LDLat->text());
        ui->lineEdit_LHMLat->setText(QString::number((int)ui->lineEdit_LMLat->text().toDouble()));
        ui->lineEdit_LHSLat->setText(QString::number(((ui->lineEdit_LMLat->text().toDouble()-(int)ui->lineEdit_LMLat->text().toDouble()))*60, 'f', 8));
        ui->lineEdit_LHDLon->setText(ui->lineEdit_LDLon->text());
        ui->lineEdit_LHMLon->setText(QString::number((int)ui->lineEdit_LMLon->text().toDouble()));
        ui->lineEdit_LHSLon->setText(QString::number(((ui->lineEdit_LMLon->text().toDouble()-(int)ui->lineEdit_LMLon->text().toDouble()))*60, 'f', 8));
        ui->lineEdit_LHDHgt->setText(ui->lineEdit_DHgt->text());

    }else if(ui->radioButton_LLHDM->isChecked()&&(ui->lineEdit_LDLat->text().count()||ui->lineEdit_LDLon->text().count()||ui->lineEdit_LDHgt->text().count()
                                                  ||ui->lineEdit_LDLon->text().count()||ui->lineEdit_LDHgt->text().count()))
    {
        double pos[3]={0};
        double ecef[3]={0};
        pos[0] = (ui->lineEdit_LDLat->text().toDouble()+(ui->lineEdit_LMLat->text().toDouble()/60))*D2R;
        pos[1] = (ui->lineEdit_LDLon->text().toDouble()+(ui->lineEdit_LMLon->text().toDouble()/60))*D2R;
        pos[2] = ui->lineEdit_LDHgt->text().toDouble();
        pos2ecef(pos, ecef);
        ui->lineEdit_X->setText(QString::number(ecef[0], 'f', 2));
        ui->lineEdit_Y->setText(QString::number(ecef[1], 'f', 2));
        ui->lineEdit_Z->setText(QString::number(ecef[2], 'f', 2));
        ui->lineEdit_DLat->setText(QString::number((ui->lineEdit_LDLat->text().toDouble()+(ui->lineEdit_LMLat->text().toDouble()/60)), 'f', 8));
        ui->lineEdit_DLon->setText(QString::number((ui->lineEdit_LDLon->text().toDouble()+(ui->lineEdit_LMLon->text().toDouble()/60)), 'f', 8));
        ui->lineEdit_DHgt->setText(ui->lineEdit_LDHgt->text());
        ui->lineEdit_LHDLat->setText(ui->lineEdit_LDLat->text());
        ui->lineEdit_LHMLat->setText(QString::number((int)ui->lineEdit_LMLat->text().toDouble()));
        ui->lineEdit_LHSLat->setText(QString::number(((ui->lineEdit_LMLat->text().toDouble()-(int)ui->lineEdit_LMLat->text().toDouble()))*60, 'f', 8));
        ui->lineEdit_LHDLon->setText(ui->lineEdit_LDLon->text());
        ui->lineEdit_LHMLon->setText(QString::number((int)ui->lineEdit_LMLon->text().toDouble()));
        ui->lineEdit_LHSLon->setText(QString::number(((ui->lineEdit_LMLon->text().toDouble()-(int)ui->lineEdit_LMLon->text().toDouble()))*60, 'f', 8));
        ui->lineEdit_LHDHgt->setText(ui->lineEdit_DHgt->text());

    }else if(ui->radioButton_LLHDMS->isChecked()&&(ui->lineEdit_LHDLat->text().count()||ui->lineEdit_LHDLon->text().count()||ui->lineEdit_LHDHgt->text().count()
                                                   ||ui->lineEdit_LHMLat->text().count()||ui->lineEdit_LHSLat->text().count()
                                                   ||ui->lineEdit_LHMLon->text().count()||ui->lineEdit_LHSLon->text().count()))
    {
        double pos[3]={0};
        double ecef[3]={0};
        pos[0] = (ui->lineEdit_LHDLat->text().toDouble()+(ui->lineEdit_LHMLat->text().toDouble()+ui->lineEdit_LHSLat->text().toDouble()/60)/60)*D2R;
        pos[1] = (ui->lineEdit_LHDLon->text().toDouble()+(ui->lineEdit_LHMLon->text().toDouble()+ui->lineEdit_LHSLon->text().toDouble()/60)/60)*D2R;
        pos[2] = ui->lineEdit_LHDHgt->text().toDouble();
        pos2ecef(pos, ecef);
        ui->lineEdit_X->setText(QString::number(ecef[0], 'f', 2));
        ui->lineEdit_Y->setText(QString::number(ecef[1], 'f', 2));
        ui->lineEdit_Z->setText(QString::number(ecef[2], 'f', 2));
        ui->lineEdit_DLat->setText(QString::number((ui->lineEdit_LHDLat->text().toDouble()+(ui->lineEdit_LHMLat->text().toDouble()+ui->lineEdit_LHSLat->text().toDouble()/60)/60), 'f', 8));
        ui->lineEdit_DLon->setText(QString::number((ui->lineEdit_LHDLon->text().toDouble()+(ui->lineEdit_LHMLon->text().toDouble()+ui->lineEdit_LHSLon->text().toDouble()/60)/60), 'f', 8));
        ui->lineEdit_DHgt->setText(ui->lineEdit_LHDHgt->text());
        ui->lineEdit_LDLat->setText(ui->lineEdit_LHDLat->text());
        ui->lineEdit_LMLat->setText(QString::number((ui->lineEdit_LHMLat->text().toDouble()+ui->lineEdit_LHSLat->text().toDouble()/60), 'f', 8));
        ui->lineEdit_LDLon->setText(ui->lineEdit_LHDLon->text());
        ui->lineEdit_LMLon->setText(QString::number((ui->lineEdit_LHMLon->text().toDouble()+ui->lineEdit_LHSLon->text().toDouble()/60), 'f', 8));
        ui->lineEdit_LDHgt->setText(ui->lineEdit_LHDHgt->text());

    }else if(ui->radioButton_ECEF->isChecked()&&(ui->lineEdit_X->text().count()||ui->lineEdit_Y->text().count()||ui->lineEdit_Z->text().count()))
    {
        double pos[3]={0};
        double ecef[3]={0};
        ecef[0] = ui->lineEdit_X->text().toDouble();
        ecef[1] = ui->lineEdit_Y->text().toDouble();
        ecef[2] = ui->lineEdit_Z->text().toDouble();
        ecef2pos(ecef,pos);
        ui->lineEdit_DLat->setText(QString::number(pos[0]*R2D,'f',8));
        ui->lineEdit_DLon->setText(QString::number(pos[1]*R2D,'f',8));
        ui->lineEdit_DHgt->setText(QString::number(pos[2],'f',2));
        ui->lineEdit_LDLat ->setText(QString::number((int)ui->lineEdit_DLat->text().toDouble()));
        ui->lineEdit_LMLat ->setText(QString::number((ui->lineEdit_DLat->text().toDouble()-(int)ui->lineEdit_DLat->text().toDouble())*60, 'f', 8));
        ui->lineEdit_LDLon ->setText(QString::number((int)ui->lineEdit_DLon->text().toDouble()));
        ui->lineEdit_LMLon ->setText(QString::number((ui->lineEdit_DLon->text().toDouble()-(int)ui->lineEdit_DLon->text().toDouble())*60, 'f', 8));
        ui->lineEdit_LDHgt ->setText(ui->lineEdit_DHgt ->text());
        ui->lineEdit_LHDLat->setText(ui->lineEdit_LDLat->text());
        ui->lineEdit_LHMLat->setText(QString::number((int)ui->lineEdit_LMLat->text().toDouble()));
        ui->lineEdit_LHSLat->setText(QString::number(((ui->lineEdit_LMLat->text().toDouble()-(int)ui->lineEdit_LMLat->text().toDouble()))*60, 'f', 8));
        ui->lineEdit_LHDLon->setText(ui->lineEdit_LDLon->text());
        ui->lineEdit_LHMLon->setText(QString::number((int)ui->lineEdit_LMLon->text().toDouble()));
        ui->lineEdit_LHSLon->setText(QString::number(((ui->lineEdit_LMLon->text().toDouble()-(int)ui->lineEdit_LMLon->text().toDouble()))*60, 'f', 8));
        ui->lineEdit_LHDHgt->setText(ui->lineEdit_DHgt->text());
    }
    if(ui->lineEdit_DLat->text().count()||ui->lineEdit_DLon->text().count()||ui->lineEdit_DHgt->text().count()
            || ui->lineEdit_LDLat->text().count()||ui->lineEdit_LDLon->text().count()||ui->lineEdit_LDHgt->text().count()||ui->lineEdit_LDLon->text().count()||ui->lineEdit_LDHgt->text().count()
            || ui->lineEdit_LHDLat->text().count()||ui->lineEdit_LHDLon->text().count()||ui->lineEdit_LHDHgt->text().count()
            ||ui->lineEdit_LHMLat->text().count()||ui->lineEdit_LHSLat->text().count()
            ||ui->lineEdit_LHMLon->text().count()||ui->lineEdit_LHSLon->text().count()
            || ui->lineEdit_X->text().count()||ui->lineEdit_Y->text().count()||ui->lineEdit_Z->text().count())
    {
        ui->lineEdit_DLat  ->setEnabled(true);
        ui->lineEdit_DLon  ->setEnabled(true);
        ui->lineEdit_DHgt  ->setEnabled(true);
        ui->lineEdit_LDLat ->setEnabled(true);
        ui->lineEdit_LDLon ->setEnabled(true);
        ui->lineEdit_LDHgt ->setEnabled(true);
        ui->lineEdit_LMLat ->setEnabled(true);
        ui->lineEdit_LMLon ->setEnabled(true);
        ui->lineEdit_LHDLat->setEnabled(true);
        ui->lineEdit_LHDLon->setEnabled(true);
        ui->lineEdit_LHDHgt->setEnabled(true);
        ui->lineEdit_LHMLat->setEnabled(true);
        ui->lineEdit_LHMLon->setEnabled(true);
        ui->lineEdit_LHSLat->setEnabled(true);
        ui->lineEdit_LHSLon->setEnabled(true);
        ui->lineEdit_X     ->setEnabled(true);
        ui->lineEdit_Y     ->setEnabled(true);
        ui->lineEdit_Z     ->setEnabled(true);
    }
}

void CoordinateConverterDialog::on_pushButton_Clear_clicked()
{
    ui->lineEdit_DLat  ->clear();
    ui->lineEdit_DLon  ->clear();
    ui->lineEdit_DHgt  ->clear();
    ui->lineEdit_LDLat ->clear();
    ui->lineEdit_LDLon ->clear();
    ui->lineEdit_LDHgt ->clear();
    ui->lineEdit_LMLat ->clear();
    ui->lineEdit_LMLon ->clear();
    ui->lineEdit_LHDLat->clear();
    ui->lineEdit_LHDLon->clear();
    ui->lineEdit_LHDHgt->clear();
    ui->lineEdit_LHMLat->clear();
    ui->lineEdit_LHMLon->clear();
    ui->lineEdit_LHSLat->clear();
    ui->lineEdit_LHSLon->clear();
    ui->lineEdit_X     ->clear();
    ui->lineEdit_Y     ->clear();
    ui->lineEdit_Z     ->clear();
}

void CoordinateConverterDialog::on_radioButton_LLHD_clicked()
{
    enableRadioButtonStstus();
}

void CoordinateConverterDialog::on_radioButton_LLHDM_clicked()
{
    enableRadioButtonStstus();
}

void CoordinateConverterDialog::on_radioButton_ECEF_clicked()
{
    enableRadioButtonStstus();
}

void CoordinateConverterDialog::on_radioButton_LLHDMS_clicked()
{
    enableRadioButtonStstus();
}

void CoordinateConverterDialog::enableRadioButtonStstus()
{
    if(ui->radioButton_LLHD->isChecked())
    {
        ui->lineEdit_DLat  ->setEnabled(true) ;
        ui->lineEdit_DLon  ->setEnabled(true) ;
        ui->lineEdit_DHgt  ->setEnabled(true) ;
        ui->lineEdit_LDLat ->setEnabled(false);
        ui->lineEdit_LDLon ->setEnabled(false);
        ui->lineEdit_LDHgt ->setEnabled(false);
        ui->lineEdit_LMLat ->setEnabled(false);
        ui->lineEdit_LMLon ->setEnabled(false);
        ui->lineEdit_LHDLat->setEnabled(false);
        ui->lineEdit_LHDLon->setEnabled(false);
        ui->lineEdit_LHDHgt->setEnabled(false);
        ui->lineEdit_LHMLat->setEnabled(false);
        ui->lineEdit_LHMLon->setEnabled(false);
        ui->lineEdit_LHSLat->setEnabled(false);
        ui->lineEdit_LHSLon->setEnabled(false);
        ui->lineEdit_X     ->setEnabled(false);
        ui->lineEdit_Y     ->setEnabled(false);
        ui->lineEdit_Z     ->setEnabled(false);

    }else if(ui->radioButton_LLHDM->isChecked())
    {
        ui->lineEdit_DLat  ->setEnabled(false);
        ui->lineEdit_DLon  ->setEnabled(false);
        ui->lineEdit_DHgt  ->setEnabled(false);
        ui->lineEdit_LDLat ->setEnabled(true) ;
        ui->lineEdit_LDLon ->setEnabled(true) ;
        ui->lineEdit_LDHgt ->setEnabled(true) ;
        ui->lineEdit_LMLat ->setEnabled(true) ;
        ui->lineEdit_LMLon ->setEnabled(true) ;
        ui->lineEdit_LHDLat->setEnabled(false);
        ui->lineEdit_LHDLon->setEnabled(false);
        ui->lineEdit_LHDHgt->setEnabled(false);
        ui->lineEdit_LHMLat->setEnabled(false);
        ui->lineEdit_LHMLon->setEnabled(false);
        ui->lineEdit_LHSLat->setEnabled(false);
        ui->lineEdit_LHSLon->setEnabled(false);
        ui->lineEdit_X     ->setEnabled(false);
        ui->lineEdit_Y     ->setEnabled(false);
        ui->lineEdit_Z     ->setEnabled(false);
    }else if(ui->radioButton_LLHDMS->isChecked())
    {
        ui->lineEdit_DLat  ->setEnabled(false);
        ui->lineEdit_DLon  ->setEnabled(false);
        ui->lineEdit_DHgt  ->setEnabled(false);
        ui->lineEdit_LDLat ->setEnabled(false);
        ui->lineEdit_LDLon ->setEnabled(false);
        ui->lineEdit_LDHgt ->setEnabled(false);
        ui->lineEdit_LMLat ->setEnabled(false);
        ui->lineEdit_LMLon ->setEnabled(false);
        ui->lineEdit_LHDLat->setEnabled(true) ;
        ui->lineEdit_LHDLon->setEnabled(true) ;
        ui->lineEdit_LHDHgt->setEnabled(true) ;
        ui->lineEdit_LHMLat->setEnabled(true) ;
        ui->lineEdit_LHMLon->setEnabled(true) ;
        ui->lineEdit_LHSLat->setEnabled(true) ;
        ui->lineEdit_LHSLon->setEnabled(true) ;
        ui->lineEdit_X     ->setEnabled(false);
        ui->lineEdit_Y     ->setEnabled(false);
        ui->lineEdit_Z     ->setEnabled(false);
    }else if(ui->radioButton_ECEF->isChecked())
    {
        ui->lineEdit_DLat  ->setEnabled(false);
        ui->lineEdit_DLon  ->setEnabled(false);
        ui->lineEdit_DHgt  ->setEnabled(false);
        ui->lineEdit_LDLat ->setEnabled(false);
        ui->lineEdit_LDLon ->setEnabled(false);
        ui->lineEdit_LDHgt ->setEnabled(false);
        ui->lineEdit_LMLat ->setEnabled(false);
        ui->lineEdit_LMLon ->setEnabled(false);
        ui->lineEdit_LHDLat->setEnabled(false);
        ui->lineEdit_LHDLon->setEnabled(false);
        ui->lineEdit_LHDHgt->setEnabled(false);
        ui->lineEdit_LHMLat->setEnabled(false);
        ui->lineEdit_LHMLon->setEnabled(false);
        ui->lineEdit_LHSLat->setEnabled(false);
        ui->lineEdit_LHSLon->setEnabled(false);
        ui->lineEdit_X     ->setEnabled(true) ;
        ui->lineEdit_Y     ->setEnabled(true) ;
        ui->lineEdit_Z     ->setEnabled(true) ;
    }
}
