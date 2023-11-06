#ifndef ST_MQSP_MODEL_H
#define ST_MQSP_MODEL_H

#include <QObject>
#include "myUtilities.h"
#include "st_mqsp_upgrade.h"
class ST_MQSP_Model:public ST_MQSP_Upgrade
{
public:
    explicit ST_MQSP_Model(QObject *parent = nullptr);

    Q_INVOKABLE bool start_download(QString firmware,int qgnss_model,int downmode,bool showexpert=false);
};
#endif // ST_MQSP_MODEL_H
