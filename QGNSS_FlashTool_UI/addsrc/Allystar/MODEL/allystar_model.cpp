/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         allystar_model.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-30      victor.gong
* ***************************************************************************/
#include "allystar_model.h"

AllyStar_Model::AllyStar_Model(QObject *parent)
              :AllyStar_Upgrade (parent)
{

}

bool AllyStar_Model::start_download(QString firmware, int qgnss_model, int downmode)
{
  if(firmware.isEmpty ())
  {
    Misc::Utilities::showMessageBox(
        tr(" file is empty!")
            ,tr("select a firmware!")
            ,tr("file empty!")
            ,QMessageBox::Ok
        );
    return false;
  }
  return qGNSS_DownLoad(firmware,qgnss_model,downmode);
}
