/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         goke_model.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-13      victor.gong
* ***************************************************************************/
#ifndef GOKE_MODEL_H
#define GOKE_MODEL_H

#include "goke_upgrade.h"
#include "myUtilities.h"

class Goke_Model:public Goke_upgrade
{
 public:
  explicit Goke_Model(Goke_upgrade *parent=nullptr);
  ~Goke_Model();
  Q_INVOKABLE bool start_download( QString FIR_Path, int model);
};

#endif // GOKE_MODEL_H
