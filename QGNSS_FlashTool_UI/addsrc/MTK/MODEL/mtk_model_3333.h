/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_model_3333.h
* Description:  inherit MTK_BROM
* History:
* Version Date                           Author          Description
*         2022-04-24      victor.gong
* ***************************************************************************/

#ifndef MTK_MODEL_3333_H
#define MTK_MODEL_3333_H

#include "mtk_brom.h"
#include "myUtilities.h"
#include "mtk_datacenter.h"
/**
 * @brief The MTK_MODEL_3333 class  provide UI Func class
 * 1. download flag of download fail or success ..
 * 2. different stage have different cloor for progress bar
 */
class MTK_MODEL_3333 : public MTK_BROM
{
    Q_OBJECT
public:
    explicit MTK_MODEL_3333(MTK_BROM *parent = nullptr);
            ~MTK_MODEL_3333();

     Q_INVOKABLE bool start_download(QString DA_Path, QString ROM_Path, int mtkmodel, int mode);

     Q_SIGNALS:

public Q_SLOTS:

private:
 Q_DISABLE_COPY(MTK_MODEL_3333);
};

#endif // MTK_MODEL_3333_H
