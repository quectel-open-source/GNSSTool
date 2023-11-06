/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_model.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-08      victor.gong
* ***************************************************************************/
#ifndef ST_MODEL_H
#define ST_MODEL_H

#include <QObject>
#include "myUtilities.h"
#include "st_upgrade.h"
class ST_Model:public ST_Upgrade
{
    Q_OBJECT
public:
   explicit ST_Model(QObject *parent = nullptr);
   Q_INVOKABLE bool start_download(QString firmware,int qgnss_model,int downmode,const ImageOptions &p=ImageOptions(),bool showExpert=false);
};
#endif // ST_MODEL_H
