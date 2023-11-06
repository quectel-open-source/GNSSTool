/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         status_wgt.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-30      victor.gong
* ***************************************************************************/
#ifndef STATUS_WGT_H
#define STATUS_WGT_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QPainter>
#include <QIcon>
#include <QToolButton>
#include <QPushButton>
#include <QProgressBar>
#include <QAction>
#include <QDateTime>
#include <QMenu>
#include <QKeySequence>

#include "qgnss_flashtool_help.h"
#include <spdlog/spdlog.h>
#include "spdlog/async.h"
#include "spdlog/common.h"
#include "spdlog/sinks/basic_file_sink.h"
/**
 * @brief The Status_WGT class
 * download status view widgets 1.log widget 2.download bt 3.progressBar and so on
 * download statues:ready warning success fial
 * can set color
 * progressBar
 */
class Status_WGT : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(MSG_str *current_status READ current_status WRITE setCurrent_status NOTIFY current_statusChanged)
    Q_PROPERTY(bool bt_open READ bt_open WRITE setBt_open NOTIFY bt_openChanged)
public:
    explicit Status_WGT(QWidget *parent = nullptr);
    virtual ~Status_WGT();
    enum status{
        Ready,
        Downloadding,
        DownloadStep,
        Warning,
        Error,
        Success,
        Fail
    };
    Q_ENUM (status);
    typedef   struct msg_str{
        status currentstatus;
        QString info_msg;  //detail msg
    }MSG_str;
Q_SIGNALS:
    void current_statusChanged(MSG_str * current_status);
    void bt_openChanged(bool bt_open);
     void bt_clickChanged();
public Q_SLOTS:
void setCurrent_status(MSG_str * current_status);
void setBt_open(bool bt_open);
void bt_click();
void setProgress(int value,QString txt);
private:
    QTextEdit *msg_;
    QGridLayout *glayout_;
    QPushButton *bt_;
    QProgressBar *progressbar_;
    int wgt_width_=85;
    MSG_str * m_current_status;
    bool m_bt_open=true;

    void create_LogFile();
    QString fileName_;
public:
    virtual bool event(QEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    void set_widget(const MSG_str &msgstr);
    MSG_str * current_status() const;
    bool bt_open() const;
};
Q_DECLARE_METATYPE (Status_WGT::MSG_str);
#endif // STATUS_WGT_H
