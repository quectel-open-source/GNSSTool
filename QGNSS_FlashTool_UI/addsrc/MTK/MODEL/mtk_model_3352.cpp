#include "mtk_model_3352.h"

MTK_MODEL_3352::MTK_MODEL_3352(MTK_BROM_3352 *parent)
    : MTK_BROM_3352{parent}
{
    auto datacenter=MTK_DataCenter::get_instance ();
    connect (this,&MTK_MODEL_3352::rw_InfoChanged,datacenter.data (),&MTK_DataCenter::setRw_Info);
}



MTK_MODEL_3352::~MTK_MODEL_3352()
{

}

bool MTK_MODEL_3352::start_download(QString DA_Path, QString CFG_Path, int mtkmodel, int mode)
{

      DA_Path= DA_Path.replace ("file:///","");
    CFG_Path= CFG_Path.replace ("file:///","");
    if(DA_Path.isEmpty ()||CFG_Path.isEmpty ())
    {
        Misc::Utilities::showMessageBox(
                    tr("DA file or Rom file is empty!")
                    ,tr("select a DA file and Rom file!")
                    ,tr("file empty!")
                    ,QMessageBox::Ok
                    );
        return false;
    }
  return  qGNSS_mTK_DownLoad(DA_Path,CFG_Path,mtkmodel,mode);

}

bool MTK_MODEL_3352::start_format(QString DA_Path, QString CFG_Path, int mtkmodel, MTK_BROM_3352::Format_mode mode, QString hexbegin_address, QString hexLength)
{
    DA_Path= DA_Path.replace ("file:///","");
    CFG_Path= CFG_Path.replace ("file:///","");
    if(DA_Path.isEmpty ()||CFG_Path.isEmpty ())
    {
        Misc::Utilities::showMessageBox(
            tr("DA file or Rom file is empty!")
                ,tr("select a DA file and Rom file!")
                ,tr("file empty!")
                ,QMessageBox::Ok
            );
        return false;
    }
     return  qGNSS_mTK_Format(DA_Path,CFG_Path,mtkmodel,mode,hexbegin_address,hexLength);
}
