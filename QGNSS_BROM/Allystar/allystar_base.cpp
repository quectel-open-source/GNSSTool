/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         allystar_base.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-29      victor.gong
* ***************************************************************************/

#include "allystar_base.h"

AllyStar_Base::AllyStar_Base(QObject *parent)
    :BROM_BASE {parent}
{

}

AllyStar_Base::~AllyStar_Base()
{

}

QMap<QString, QVariant> &AllyStar_Base::flashInfo()
{
  return flashInfo_;
}

void AllyStar_Base::setFlashInfo(const QMap<QString, QVariant> &newFlashInfo)
{
  if (flashInfo_ == newFlashInfo)
    return;
  flashInfo_ = newFlashInfo;
  emit flashInfoChanged();
}
