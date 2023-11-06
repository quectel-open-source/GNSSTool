/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk3335_readback_ui.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-22      victor.gong
* ***************************************************************************/

#ifndef MTK3335_READBACK_UI_H
#define MTK3335_READBACK_UI_H

#include <QRadioButton>
#include <QDebug>
#include "qgnss_flashtool_help.h"
#include "base_groupbox.h"
#include "setting_cfg.h"
#include "mtk_brom_3335.h"

#include "mtk3335_readback_tablewidgets.h"
class MTK3335_ReadBack_Ui:public Base_GroupBox
{
    Q_OBJECT
public:
    explicit MTK3335_ReadBack_Ui(QWidget *parent = nullptr);
    explicit MTK3335_ReadBack_Ui(const QString &title, QWidget *parent = nullptr);
    void cfgTableWidgets();
    QVector< MTK_BROM_3335::Struct_ReadBack> Get_Struct_ReadBack();

Q_SIGNALS:

public Q_SLOTS:

private:
    MTK3335_ReadBack_TableWidgets *tablewidgets_=nullptr;
};
#endif // MTK3335_READBACK_UI_H
