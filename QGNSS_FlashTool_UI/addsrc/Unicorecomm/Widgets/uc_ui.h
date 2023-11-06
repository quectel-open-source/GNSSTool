/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         uc_ui.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-13      victor.gong
* ***************************************************************************/
#ifndef UC_UI_H
#define UC_UI_H
#include <QWidget>
#include <QLineEdit>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QToolButton>
#include <QButtonGroup>
#include "uc_model.h"
#include "uc_base_window.h"
class UC_UI:public UC_Base_Window
{
  Q_OBJECT
 public:
  explicit UC_UI(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

  ~UC_UI();

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
  UC_Model *UCModel=nullptr;
 Q_SIGNALS:
 public Q_SLOTS:

 private:
  QToolButton *downloadbt_=nullptr;
 public:
  virtual void cfg_parameter() override;
  virtual void cfg_widget() override;
  bool event(QEvent *event) override;
  void wgt_enable(bool Run) override;
};

#endif // UC_UI_H
