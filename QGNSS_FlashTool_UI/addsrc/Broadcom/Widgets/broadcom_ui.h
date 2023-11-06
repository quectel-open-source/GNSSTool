/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_ui.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-25      victor.gong
* ***************************************************************************/
#ifndef BROADCOM_UI_H
#define BROADCOM_UI_H

#include <QWidget>
#include <QLineEdit>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QToolButton>
#include <QButtonGroup>
#include"broadcom_base_window.h"
#include"broadcom_model.h"
class Broadcom_UI: public Broadcom_Base_Window
{
    Q_OBJECT
public:
   explicit Broadcom_UI(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    ~Broadcom_UI();

    enum cfg_key{
        baudrate,
        downmode,
        Downloadpath,
        Upgradepath,
    };
     Q_ENUM (cfg_key)
   Q_INVOKABLE virtual  void start() override;
   Q_INVOKABLE virtual  void stop() override;
   Q_INVOKABLE virtual  void ini_IO(IODvice *io) override;
   void open_file() ;
   void load_file() ;
   Broadcom_Model *BroadcomModel=nullptr;
Q_SIGNALS:
public Q_SLOTS:

private:
    void showopt_cb(int model);
    QToolButton *downloadbt_=nullptr;
    QToolButton *upgradebt_=nullptr;
    // Broadcom_Base_Window interface
public:
    virtual void cfg_parameter() override;
    virtual void cfg_widget() override;
    bool event(QEvent *event) override;
};

#endif // BROADCOM_UI_H
