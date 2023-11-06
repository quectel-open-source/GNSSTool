#include "st_mqsp_model.h"

ST_MQSP_Model::ST_MQSP_Model(QObject *parent)
    :ST_MQSP_Upgrade (parent)
{

}

bool ST_MQSP_Model::start_download(QString firmware, int qgnss_model, int downmode,bool showexpert)
{
    if(firmware.isEmpty ())
    {
        Misc::Utilities::showMessageBox(
            tr("file is empty!")
                ,tr("select a firmware!")
                ,tr("file empty!")
                ,QMessageBox::Ok
            );
        return false;
    }
    return qGNSS_ST_DownLoad(firmware,qgnss_model,downmode,showexpert);
}
