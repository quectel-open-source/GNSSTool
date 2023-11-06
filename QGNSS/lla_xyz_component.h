#ifndef LLA_XYZ_COMPONENT_H
#define LLA_XYZ_COMPONENT_H

#include <QWidget>

namespace Ui {
class LLA_XYZ_Component;
}

struct SaveCoordinate
{
    int id;
    QString name;
    QString lla[3];
    QString xyz[3];
};
Q_DECLARE_METATYPE(SaveCoordinate)

class LLA_XYZ_Component : public QWidget
{
    Q_OBJECT

public:
    explicit LLA_XYZ_Component(QWidget *parent = nullptr);
    ~LLA_XYZ_Component();
    void setLineEditReadOnly(bool readOnly);
    void setLineEditEnable(bool enable);
    void setLineLLA_Enable(bool enable);
    void setLineXYZ_Enable(bool enable);
    QString inputPosCheck(int posType);//0:lla  1:xyz
    void fillAllPos(int inputPosType);
    void clearLineEdit();
    void showXYZ_Pos(const double (&p)[3]);
    void showLLA_Pos(const double (&p)[3]);
    double* getPos();
    SaveCoordinate getSaveCoodinate();
    bool setSaveCoodinate(SaveCoordinate c);

    double lla_pos[3];
    double xyz_pos[3];
private:
    Ui::LLA_XYZ_Component *ui;
};

#endif // LLA_XYZ_COMPONENT_H
