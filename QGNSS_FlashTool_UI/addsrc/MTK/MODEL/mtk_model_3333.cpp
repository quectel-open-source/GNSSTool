/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_model_3333.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-25      victor.gong
* ***************************************************************************/
#include "mtk_model_3333.h"

MTK_MODEL_3333::MTK_MODEL_3333(MTK_BROM *parent)
    : MTK_BROM{parent}
{
    auto datacenter=MTK_DataCenter::get_instance ();
    connect (this,&MTK_MODEL_3333::rw_InfoChanged,datacenter.data (),&MTK_DataCenter::setRw_Info);
}



bool MTK_MODEL_3333::start_download(QString DA_Path, QString ROM_Path, int mtkmodel, int mode)
{
    DA_Path= DA_Path.replace ("file:///","");
    ROM_Path= ROM_Path.replace ("file:///","");
    if(DA_Path.isEmpty ()||ROM_Path.isEmpty ())
    {
        Misc::Utilities::showMessageBox(
            tr("DA file or Rom file is empty!")
                ,tr("select a DA file and Rom file!")
                ,tr("file empty!")
                ,QMessageBox::Ok
            );
        return false;
    }
    QString key=IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model>((BROM_BASE::QGNSS_Model)mtkmodel);
    if(QMessageBox::No==  Misc::Utilities::showMessageBox(
            tr("Please reset the %1 module and click [Yes] button to  download...").arg (key)
                ,tr("Reset the module!")
                ,tr("INFO")
                ,QMessageBox::Yes|QMessageBox::No
            ))
    {
        return false;
    }
    return qGNSS_mTK_DownLoad(DA_Path,ROM_Path,mtkmodel,mode);
}

MTK_MODEL_3333::~MTK_MODEL_3333()
{

}
