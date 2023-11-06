/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         qgnss_flashtool.h
* Description:  flashtool control provide a QGNSS_Model show right platform to download firware
* ***********************example***********************************
* auto flashtool =QGNSS_FLASHTOOL::get_instance();
* flashtool_get_io_instance(serial_instance)
* flashtool_show(QGNSS_Model)
*
* History:
* Version Date                           Author          Description
*         2022-04-25      victor.gong
* ***************************************************************************/
#ifndef QGNSS_FLASHTOOL_H
#define QGNSS_FLASHTOOL_H

#include "myUtilities.h"
#include "ioserial.h"
#include "qgnss_flashtool_help.h"
#include "AppInfo.h"

#include "mt3333_ui.h"
#include "mt3335_ui.h"
#include "mt3352_ui.h"

#include "mtk_model_3333.h"
#include "mtk_model_3335.h"
#include "mtk_model_3352.h"

#include "broadcom_ui.h"
#include "broadcom_model.h"

#include "st_ui.h"
#include "st_model.h"

#include "st_mqsp_ui.h"
#include "st_pm_discardqt_ui.h"
#include "st_mqsp_model.h"

#include "goke_ui.h"
#include "goke_model.h"


#include "allystar_ui.h"
#include "allystar_model.h"

#include "uc_ui.h"
#include "uc_model.h"
/**
 * @brief The QGNSS_FLASHTOOL class  Control Flashtool
 */
class QGNSS_FLASHTOOL : public QObject
{
    Q_OBJECT
public:
    explicit QGNSS_FLASHTOOL(QObject *parent = nullptr);
    ~QGNSS_FLASHTOOL();

    static  QSharedPointer<QGNSS_FLASHTOOL> get_instance();
    typedef  MTK_BROM::QGNSS_Model   QGNSSMODEL;
    typedef  MTK_BROM::QGNSS_Platform QGNSSPALTFORM;

    /**
     * @brief flashtool_get_io_instance  get instance from QGNSS QSerialPort
     * @param serial
     * @return
     */
    Q_INVOKABLE bool flashtool_get_io_instance(QSerialPort *serial);
    Q_INVOKABLE void flashtool_show(QGNSSMODEL qgnssmodel,QWidget *parent=nullptr);
    /**
     * @brief isopen  whether the flashtool dialog  is open true open false destory
     *  inorder to control QGNSS readdata() func not call readall()
     * @return
     */
    Q_INVOKABLE bool isopen();
Q_SIGNALS:

    void openChanged(bool open);

public Q_SLOTS:
    void setopen(int status);
private:
    void choose_model  (QGNSSMODEL md,QWidget *parent=nullptr);

private:
    QSharedPointer<Misc::Utilities> miscUtilities;
    QSharedPointer<MTK_DataCenter> mtk_center;
    QPointer<QSerialPort> qio_serial_;
    QPointer<IOserial>    io_serial_;

    MT3333_UI   *mt3333ui=nullptr;
    MT3335_UI   *mt3335ui=nullptr;
    MT3352_UI   *mt3352ui=nullptr;

    Broadcom_UI *broadcomui=nullptr;
    ST_UI *stui=nullptr;
    ST_MQSP_UI *stmqsp=nullptr;
    ST_PM_discardqt_ui *st_apam_=nullptr;
    Goke_UI *gokeui=nullptr;

    AllyStar_UI *allystarui=nullptr;

    UC_UI *uc_ui=nullptr;

    QSharedPointer<QGNSS_FLASHTOOL_HELP> qgnss_help_;
    BROM_BASE brom_base;
    QAtomicInt status_=0;
};

#endif // QGNSS_FLASHTOOL_H
