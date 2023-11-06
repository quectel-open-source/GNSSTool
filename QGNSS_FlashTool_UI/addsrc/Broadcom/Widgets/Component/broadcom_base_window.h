/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_base_window.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-25      victor.gong
* ***************************************************************************/
#ifndef BROADCOM_BASE_WINDOW_H
#define BROADCOM_BASE_WINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QActionGroup>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QCheckBox>
#include <QToolButton>
#include <QProgressBar>
#include <QFileDialog>

#include "status_wgt.h"
#include "base_mainwindow.h"
#include "qgnss_flashtool_help.h"
#include "broadcom_base.h"
class Broadcom_Base_Window:public Base_MainWindow
{
    Q_OBJECT
    Q_PROPERTY(int currentBaudrate READ currentBaudrate WRITE setCurrentBaudrate NOTIFY currentBaudrateChanged)
    Q_PROPERTY(int currentDownmode READ currentDownmode WRITE setCurrentDownmode NOTIFY currentDownmodeChanged)
public:
    explicit Broadcom_Base_Window(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    virtual  ~Broadcom_Base_Window();
   // void cfg_model(QSharedPointer<Broadcom_Base> Base);
    void cfg_model(Broadcom_Base* Base);
    virtual  void start()=0;
    virtual  void stop()=0;
    virtual  void ini_IO(IODvice *io) =0;

    virtual  void cfg_parameter()=0;
    virtual void cfg_widget()=0;
    void construct_ui();

    int currentDownmode() const;
    int currentBaudrate() const;
protected:
    QToolBar *toolbar_;
    QMenuBar *menubar_;
    QMenu *menu_setting_;
    QMenu *menu_baudrate_;
    //QMenu *menu_downmode_;
    QActionGroup *baudrate_group_;
    QActionGroup *downmode_group_;
    QWidget  *content_;
    QVBoxLayout   *mainVlayout_;
    QHBoxLayout   *dafileHlayout_;
    QHBoxLayout   *optHlayout_;

    QLabel  *label_firmware_name_;
    QLineEdit   *edit_firmware_path_;
    QPushButton *openfileBT_;

    QLabel *firmware_opt_;
    QCheckBox *cb_eraseNVM_;
    QCheckBox *cb_eraseLTO_;

    Status_WGT *statuswgt_;
    //statusbar
    QStatusBar *statusbar_;
    QLabel *downrate_;
    QLabel *portName_;
    QLabel *downmodetxt_;
    QLabel *downbuadrate_;
    QLabel *downloadtime_;

    Broadcom_Base*broadcombase_=nullptr;
    QSharedPointer<QGNSS_FLASHTOOL_HELP> qgnss_help_;

    QList<QString> baudrates_;

    QList<QString> downmode_;

    QList<QString> tabelheaderlabels_;

Q_SIGNALS:
    void currentDownmodeChanged(int currentDownmode);
    void currentBaudrateChanged(int currentBaudrate);

public Q_SLOTS:

    void setCurrentDownmode(int currentDownmode);
    void setCurrentBaudrate(int currentBaudrate);
private:
    int m_currentDownmode=0;
    int m_currentBaudrate=0;
};

#endif // BROADCOM_BASE_WINDOW_H
