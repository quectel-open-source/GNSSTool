/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         allystar_ui.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-30      victor.gong
* ***************************************************************************/
#ifndef ALLYSTAR_UI_H
#define ALLYSTAR_UI_H

#include <QWidget>
#include <QLineEdit>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QToolButton>
#include <QButtonGroup>
#include "allystar_model.h"
#include "allystar_base_window.h"
class AllyStar_UI:public AllyStar_base_window
{
  Q_OBJECT
 public:
  AllyStar_UI(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
  ~AllyStar_UI();
  enum cfg_key
  {
    baudrate,
    downmode,
    path
  };
  Q_ENUM (cfg_key)
  Q_INVOKABLE virtual  void start() override;
  Q_INVOKABLE virtual  void stop() override;
  Q_INVOKABLE virtual  void ini_IO(IODvice *io) override;
  void open_file() ;
  void load_file() ;

  AllyStar_Model *model=nullptr;
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

#endif // ALLYSTAR_UI_H
