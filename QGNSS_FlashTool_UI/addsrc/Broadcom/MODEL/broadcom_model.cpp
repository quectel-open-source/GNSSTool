/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_model.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-01      victor.gong
* ***************************************************************************/

#include "broadcom_model.h"
Broadcom_Model::Broadcom_Model(QObject *parent)
    : Broadcom_upgrade(parent)
{
#ifdef DEBUG_MSG
    qDebug()<<Q_FUNC_INFO;
#endif
}

Broadcom_Model::~Broadcom_Model()
{
#ifdef DEBUG_MSG
    qDebug()<<Q_FUNC_INFO;
#endif
}


bool Broadcom_Model::start_download(QString firmware, int qgnss_model, int downmode, const Broadcom_Base::ImageOptions &p)
{
    if(firmware.isEmpty ())
    {
        Misc::Utilities::showMessageBox(
            tr("DA file is empty!")
                ,tr("select a firmware!")
                ,tr("file empty!")
                ,QMessageBox::Ok
            );
        return false;
    }
    return qGNSS_broadcom_DownLoad(firmware,qgnss_model,downmode,p);
}
