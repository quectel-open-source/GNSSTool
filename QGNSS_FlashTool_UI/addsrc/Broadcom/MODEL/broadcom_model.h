/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_model.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-01      victor.gong
* ***************************************************************************/
#ifndef BROADCOM_MODEL_H
#define BROADCOM_MODEL_H

#include <QObject>
#include "myUtilities.h"
#include "broadcom_upgrade.h"
class Broadcom_Model : public Broadcom_upgrade
{
    Q_OBJECT
public:
    explicit Broadcom_Model(QObject *parent = nullptr);
    ~Broadcom_Model();
    Q_INVOKABLE bool start_download(QString firmware,int qgnss_model,int downmode,const ImageOptions &p=ImageOptions());

Q_SIGNALS:
public Q_SLOTS:

private:
};

#endif // BROADCOM_MODEL_H
