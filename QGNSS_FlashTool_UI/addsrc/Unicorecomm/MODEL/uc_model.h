/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         uc_model.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-13      victor.gong
* ***************************************************************************/
#ifndef UC_MODEL_H
#define UC_MODEL_H
#include <QObject>
#include "myUtilities.h"

#include "unicorecomm_upgrade.h"
class UC_Model :public Unicorecomm_Upgrade
{
  Q_OBJECT
 public:
  explicit UC_Model(QObject *parent = nullptr);
  Q_INVOKABLE bool start_download(QString firmware,int qgnss_model);
};

#endif // UC_MODEL_H
