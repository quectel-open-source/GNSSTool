/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_model.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-08      victor.gong
* ***************************************************************************/
#include "st_model.h"
#include "expert_upg_ui.h"

ST_Model::ST_Model(QObject *parent)
         :ST_Upgrade (parent)
{

}


bool ST_Model::start_download(QString firmware, int qgnss_model, int downmode, const ImageOptions &p,bool showExpert)
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
    return qGNSS_ST_DownLoad(firmware,qgnss_model,downmode,p,showExpert);
}
