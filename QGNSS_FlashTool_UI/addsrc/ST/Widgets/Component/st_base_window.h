/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_base_window.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-08      victor.gong
* ***************************************************************************/
#ifndef ST_BASE_WINDOW_H
#define ST_BASE_WINDOW_H

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
#include <QButtonGroup>

#include "status_wgt.h"
#include "base_mainwindow.h"
#include "qgnss_flashtool_help.h"
#include "st_base.h"
class ST_Base_Window:public Base_MainWindow
{
    Q_OBJECT
    Q_PROPERTY(int currentBaudrate READ currentBaudrate WRITE setCurrentBaudrate NOTIFY currentBaudrateChanged)
    Q_PROPERTY(int currentDownmode READ currentDownmode WRITE setCurrentDownmode NOTIFY currentDownmodeChanged)
public:
    explicit ST_Base_Window(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    virtual  ~ST_Base_Window();
             void cfg_model(ST_Base* Base);
    virtual  void start()=0;
    virtual  void stop()=0;
    virtual  void ini_IO(IODvice *io) =0;

    virtual void cfg_parameter()=0;
    virtual void cfg_widget()=0;
    virtual void wgt_enable(bool Run)=0;
            void construct_ui();
            bool GetCRCINFO(QString filepath,uint &filesize,QString &crc);

    int currentDownmode() const;
    int currentBaudrate() const;
protected:
    QToolBar *toolbar_;
    QMenuBar *menubar_;
    QMenu *menu_setting_;
    QMenu *menu_baudrate_;
    QMenu *showExpert_;//专家配置项
    QAction *downloadOpt_ac_;//发送配置项是否显示配置窗口
    QAction *showExpertMenu_;//显示专家项

    QActionGroup *baudrate_group_;
    QActionGroup *downmode_group_;
    QWidget      *content_;
    QVBoxLayout  *mainVlayout_;
    QHBoxLayout  *dafileHlayout_;


    QLabel      *label_firmware_name_;
    QLineEdit   *edit_firmware_path_;
    QPushButton *openfileBT_;

    QHBoxLayout   *optHlayout_;
    QGridLayout   *crclayout_;
    QLabel        *firmware_opt_;
    QCheckBox     *cb_eraseNVM_;
    QCheckBox     *cb_Recovery_;

    //QButtonGroup *formatsizegrp;
    //QCheckBox *c_2m_box;
    //QCheckBox *c_4m_box;

    Status_WGT *statuswgt_;
    //statusbar
    QStatusBar *statusbar_;
    QLabel *downrate_;
    QLabel *portName_;
    QLabel *downmodetxt_;
    QLabel *downbuadrate_;
    QLabel *downloadtime_;

    QLabel *size_label;
    QLabel *crc_label;
    QLineEdit *filesize_txt;
    QLineEdit *filecrc_txt;

    ST_Base*stBase=nullptr;
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

#endif // ST_BASE_WINDOW_H
