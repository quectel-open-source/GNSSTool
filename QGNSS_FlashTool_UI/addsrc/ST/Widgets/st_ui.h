/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_ui.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-08      victor.gong
* ***************************************************************************/
#ifndef ST_UI_H
#define ST_UI_H

#include <QWidget>
#include <QLineEdit>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QToolButton>
#include <QButtonGroup>
#include"st_model.h"
#include"st_base_window.h"
class ST_UI:public ST_Base_Window
{
    Q_OBJECT
public:
    explicit ST_UI(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        ~ST_UI();

        enum cfg_key
        {
            baudrate,
            downmode,
            Downloadpath,
            Upgradepath
        };
    Q_ENUM (cfg_key)
    Q_INVOKABLE virtual  void start() override;
    Q_INVOKABLE virtual  void stop() override;
    Q_INVOKABLE virtual  void ini_IO(IODvice *io) override;
    void open_file() ;
    void load_file() ;
    ST_Model *STModel=nullptr;
Q_SIGNALS:
public Q_SLOTS:

private:
    void showopt_cb(int model);
    QToolButton *downloadbt_=nullptr;
    QToolButton *upgradebt_=nullptr;

public:
    virtual void cfg_parameter() override;
    virtual void cfg_widget() override;
    bool event(QEvent *event) override;
    void wgt_enable(bool Run) override;
};

#endif // ST_UI_H
