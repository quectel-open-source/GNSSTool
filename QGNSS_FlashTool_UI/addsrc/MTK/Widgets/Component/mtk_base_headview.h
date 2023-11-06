/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_base_headview.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-09      victor.gong
* ***************************************************************************/
#ifndef MTK_BASE_HEADVIEW_H
#define MTK_BASE_HEADVIEW_H
#include "base_headview.h"

class MTK_base_HeadView:public Base_HeadView
{
public:
    explicit MTK_base_HeadView(Qt::Orientation orientation, QWidget *parent = nullptr);
             ~MTK_base_HeadView();
};

#endif // MTK_BASE_HEADVIEW_H
