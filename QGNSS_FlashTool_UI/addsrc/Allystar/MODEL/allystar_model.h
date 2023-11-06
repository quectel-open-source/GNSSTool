/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         allystar_model.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-30      victor.gong
* ***************************************************************************/

#ifndef ALLYSTAR_MODEL_H
#define ALLYSTAR_MODEL_H

#include <QObject>
#include "myUtilities.h"
#include "allystar_upgrade.h"
class AllyStar_Model:public AllyStar_Upgrade
{
 public:
  AllyStar_Model(QObject *parent = nullptr);
  Q_INVOKABLE bool start_download(QString firmware,
                                  int model,int downmode);
};

#endif // ALLYSTAR_MODEL_H
