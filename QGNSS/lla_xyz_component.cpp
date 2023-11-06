#include "lla_xyz_component.h"
#include "ui_lla_xyz_component.h"
#include "QRegExp"
#include "QRegExpValidator"
#include "RTKLib/rtklib.h"

LLA_XYZ_Component::LLA_XYZ_Component(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LLA_XYZ_Component)
{
    ui->setupUi(this);
    QRegExp exp("[0-9\\.-]+$");
    QValidator *Validator = new QRegExpValidator(exp);
    ui->lineEdit_alt  ->setValidator(Validator);
    ui->lineEdit_lat  ->setValidator(Validator);
    ui->lineEdit_lon  ->setValidator(Validator);
    ui->lineEdit_x    ->setValidator(Validator);
    ui->lineEdit_y    ->setValidator(Validator);
    ui->lineEdit_z    ->setValidator(Validator);
}

LLA_XYZ_Component::~LLA_XYZ_Component()
{
    delete ui;
}

void LLA_XYZ_Component::setLineEditReadOnly(bool readOnly)
{
    auto l = this->findChildren<QLineEdit *>();
    for(auto v = l.begin(); v != l.end(); v++){
        (*v)->setReadOnly(readOnly);
    }
}

void LLA_XYZ_Component::setLineEditEnable(bool enable)
{
    auto l = this->findChildren<QLineEdit *>();
    for(auto v = l.begin(); v != l.end(); v++){
        (*v)->setEnabled(enable);
    }
}

void LLA_XYZ_Component::setLineLLA_Enable(bool enable)
{
    ui->lineEdit_alt->setEnabled(enable);
    ui->lineEdit_lat->setEnabled(enable);
    ui->lineEdit_lon->setEnabled(enable);
}

void LLA_XYZ_Component::setLineXYZ_Enable(bool enable)
{
    ui->lineEdit_x->setEnabled(enable);
    ui->lineEdit_y->setEnabled(enable);
    ui->lineEdit_z->setEnabled(enable);
}

QString LLA_XYZ_Component::inputPosCheck(int posType)
{
    switch (posType) {
    case 0:
        if(ui->lineEdit_alt->text().isEmpty()
                || ui->lineEdit_lat->text().isEmpty()
                ||ui->lineEdit_lon->text().isEmpty())
            return "Please enter the valid coordinates";
        break;
    case 1:
        if(ui->lineEdit_x->text().isEmpty()
                ||ui->lineEdit_y->text().isEmpty()
                ||ui->lineEdit_z->text().isEmpty())
            return "Please enter the valid coordinates.";
        break;
    default:
        return "Unknown";
        break;
    }
    return "";
}

void LLA_XYZ_Component::fillAllPos(int inputPosType)
{
    switch (inputPosType) {
    case 0:{
        lla_pos[0] = ui->lineEdit_lat->text().toDouble();
        lla_pos[1] = ui->lineEdit_lon->text().toDouble();
        lla_pos[2] = ui->lineEdit_alt->text().toDouble();
        showLLA_Pos(lla_pos);
        break;
    }
    case 1:{
        xyz_pos[0] = ui->lineEdit_x->text().toDouble();
        xyz_pos[1] = ui->lineEdit_y->text().toDouble();
        xyz_pos[2] = ui->lineEdit_z->text().toDouble();
        showXYZ_Pos(xyz_pos);
        break;
    }
    }
}

void LLA_XYZ_Component::clearLineEdit()
{
    auto l = this->findChildren<QLineEdit *>();
    for(auto v = l.begin(); v != l.end(); v++){
        (*v)->clear();
    }
}

void LLA_XYZ_Component::showXYZ_Pos(const double (&p)[3])
{
    ui->lineEdit_x->setText(QString::fromStdString(std::to_string(p[0])));
    ui->lineEdit_y->setText(QString::fromStdString(std::to_string(p[1])));
    ui->lineEdit_z->setText(QString::fromStdString(std::to_string(p[2])));
    double pos[3] = {};
    ecef2pos(p,pos);
    ui->lineEdit_lat->setText(QString::fromStdString(std::to_string(pos[0] * R2D)));
    ui->lineEdit_lon->setText(QString::fromStdString(std::to_string(pos[1] * R2D)));
    ui->lineEdit_alt->setText(QString::fromStdString(std::to_string(pos[2])));
}

void LLA_XYZ_Component::showLLA_Pos(const double (&p)[3])
{
    ui->lineEdit_x->setText(QString::fromStdString(std::to_string(p[0])));
    ui->lineEdit_y->setText(QString::fromStdString(std::to_string(p[1])));
    ui->lineEdit_z->setText(QString::fromStdString(std::to_string(p[2])));
    double pos[3] = {};
    pos[0] = p[0]*D2R;
    pos[1] = p[1]*D2R;
    pos[2] = p[2];
    pos2ecef(pos, xyz_pos);
    ui->lineEdit_x->setText(QString::fromStdString(std::to_string(xyz_pos[0])));
    ui->lineEdit_y->setText(QString::fromStdString(std::to_string(xyz_pos[1])));
    ui->lineEdit_z->setText(QString::fromStdString(std::to_string(xyz_pos[2])));
}

SaveCoordinate LLA_XYZ_Component::getSaveCoodinate()
{
    SaveCoordinate sc;
    sc.lla[0] = ui->lineEdit_lat->text();
    sc.lla[1] = ui->lineEdit_lon->text();
    sc.lla[2] = ui->lineEdit_alt->text();
    sc.xyz[0] = ui->lineEdit_x->text();
    sc.xyz[1] = ui->lineEdit_y->text();
    sc.xyz[2] = ui->lineEdit_z->text();
    return sc;
}

bool LLA_XYZ_Component::setSaveCoodinate(SaveCoordinate c)
{
    ui->lineEdit_lat->setText(c.lla[0]);
    ui->lineEdit_lon->setText(c.lla[1]);
    ui->lineEdit_alt->setText(c.lla[2]);
    ui->lineEdit_x->setText(c.xyz[0]);
    ui->lineEdit_y->setText(c.xyz[1]);
    ui->lineEdit_z->setText(c.xyz[2]);
    return true;
}
