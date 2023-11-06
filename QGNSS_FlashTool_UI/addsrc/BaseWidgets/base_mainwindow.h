/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_mainwindow.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-05      victor.gong
* ***************************************************************************/
#ifndef BASE_MAINWINDOW_H
#define BASE_MAINWINDOW_H

#include <QMainWindow>
#include "setting_cfg.h"
/**
 * @brief The Base_MainWindow class
 * every model have a config node node name with model name
 * every model
 */
class Base_MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit Base_MainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    virtual  ~Base_MainWindow();
Q_SIGNALS:

public Q_SLOTS:

private:

};

#endif // BASE_MAINWINDOW_H
