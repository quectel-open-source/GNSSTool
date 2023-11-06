/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mt3335_cfg_ui.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-06      victor.gong
* ***************************************************************************/
#ifndef MT3335_CFG_UI_H
#define MT3335_CFG_UI_H

#include <QWidget>
//#include <QSettings>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDebug>
#include "setting_cfg.h"
#include "base_dialog.h"
class MT3335_cfg_UI : public Base_Dialog
{
    Q_OBJECT
    Q_PROPERTY(QString da921600path READ da921600path WRITE setDa921600path NOTIFY da921600pathChanged)
    Q_PROPERTY(QString da115200path READ da115200path WRITE setDa115200path NOTIFY da115200pathChanged)
public:
    explicit MT3335_cfg_UI(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
             ~MT3335_cfg_UI();

    void cfg_parameter();
    void cfg_widget();
    void construct_ui();
    void connect_ui();
    void load_pth(BROM_BASE::QGNSS_Model md);
    QString da921600path() const;
    QString da115200path() const;

Q_SIGNALS:

    void da921600pathChanged(QString da921600path);
    void da115200pathChanged(QString da115200path);

public Q_SLOTS:

    void setDa921600path(QString da921600path);
    void setDa115200path(QString da115200path);

private:
    //setting
        //QSettings setting_;
    //layout
    QVBoxLayout *main_vlayout=nullptr;
    QGridLayout *girdlayout_=nullptr;
    QHBoxLayout *bt_Hlayout=nullptr;
    //two label
    QLabel *label1_=nullptr;
    QLabel *label2_=nullptr;
    //two da file path 921600 115200
    QLineEdit *line921600_=nullptr;
    QLineEdit *line115200_=nullptr;
    //two bt
    QPushButton * bt921600DA_=nullptr;
    QPushButton * bt115200DA_=nullptr;

    //ok cancel apply
    QPushButton * okbt_=nullptr;
    QPushButton * cancelbt_=nullptr;
    QPushButton * applybt_=nullptr;
    BROM_BASE::QGNSS_Model model_;


protected:

    void save_pth();
private:
    QString m_da921600path;
    QString m_da115200path;
};

#endif // MT3335_CFG_UI_H
