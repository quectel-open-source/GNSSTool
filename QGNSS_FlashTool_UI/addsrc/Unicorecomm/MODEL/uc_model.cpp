/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         uc_model.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-13      victor.gong
* ***************************************************************************/
#include "uc_model.h"

UC_Model::UC_Model(QObject *parent)
         :Unicorecomm_Upgrade (parent)
{

}

bool UC_Model::start_download(QString firmware, int qgnss_model)
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
  return qGNSS_DownLoad(firmware,qgnss_model);
}
