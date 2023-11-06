/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         goke_model.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-13      victor.gong
* ***************************************************************************/

#include "goke_model.h"



Goke_Model::Goke_Model(Goke_upgrade *parent): Goke_upgrade (parent)
{

}

Goke_Model::~Goke_Model()
{

}

bool Goke_Model::start_download( QString FIR_Path, int model)
{

  FIR_Path= FIR_Path.replace ("file:///","");
  if(FIR_Path.isEmpty ())
  {
    Misc::Utilities::showMessageBox(
        tr("BE file or Rom file is empty!")
            ,tr("select BE file and FIRE file!")
            ,tr("file empty!")
            ,QMessageBox::Ok
        );
    return false;
  }
  QString key=IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model>((BROM_BASE::QGNSS_Model)model);

  return qGNSS_DownLoad(FIR_Path,model);
}
