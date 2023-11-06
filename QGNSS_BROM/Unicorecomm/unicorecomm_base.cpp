/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         unicorecomm_base.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-09      victor.gong
* ***************************************************************************/
#include "unicorecomm_base.h"

Unicorecomm_Base::Unicorecomm_Base(QObject *parent)
                :BROM_BASE {parent}
{

}

Unicorecomm_Base::~Unicorecomm_Base()
{

}

QMap<QString, QVariant> &Unicorecomm_Base::flashInfo()
{
  return flashInfo_;
}

void Unicorecomm_Base::setFlashInfo(const QMap<QString, QVariant> &newFlashInfo)
{
  if (flashInfo_ == newFlashInfo)
    return;
  flashInfo_ = newFlashInfo;
  emit flashInfoChanged();
}
