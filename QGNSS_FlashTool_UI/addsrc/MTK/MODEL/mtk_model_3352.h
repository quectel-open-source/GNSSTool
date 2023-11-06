#ifndef MTK_MODEL_3352_H
#define MTK_MODEL_3352_H

#include <QObject>
#include "mtk_brom_3352.h"
#include "myUtilities.h"
#include "mtk_datacenter.h"
class MTK_MODEL_3352 : public MTK_BROM_3352
{
    Q_OBJECT

public:
    explicit MTK_MODEL_3352(MTK_BROM_3352 *parent = nullptr);
    ~MTK_MODEL_3352();
    Q_INVOKABLE bool start_download(QString DA_Path, QString CFG_Path, int mtkmodel, int mode);
    Q_INVOKABLE bool start_format(QString DA_Path, QString CFG_Path, int mtkmodel
                                  ,Format_mode mode,QString hexbegin_address=0,QString hexLength=0);
Q_SIGNALS:
public Q_SLOTS:
private:
};

#endif // MTK_MODEL_3335_H
