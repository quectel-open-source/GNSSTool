#ifndef MTK_MODEL_3335_H
#define MTK_MODEL_3335_H

#include <QObject>
#include "mtk_brom_3335.h"
#include "myUtilities.h"
#include "mtk_datacenter.h"
class MTK_MODEL_3335 : public MTK_BROM_3335
{
    Q_OBJECT

public:
    explicit MTK_MODEL_3335(MTK_BROM_3335 *parent = nullptr);
    ~MTK_MODEL_3335();
    Q_INVOKABLE bool start_download(QString DA_Path, QString CFG_Path, int mtkmodel, int mode);
    Q_INVOKABLE bool start_format(QString DA_Path, QString CFG_Path, int mtkmodel
                                  ,Format_mode mode,QString hexbegin_address=0,QString hexLength=0);
    bool start_readback(QString DA_Path, QString CFG_Path, int mtkmodel,
                        int mode,const Struct_ReadBack *STRread,int readLen
                        );
Q_SIGNALS:
public Q_SLOTS:
private:
};

#endif // MTK_MODEL_3335_H
