/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_base_headview.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-09      victor.gong
* ***************************************************************************/
#include "mtk_base_headview.h"

MTK_base_HeadView::MTK_base_HeadView(Qt::Orientation orientation, QWidget *parent)
                  :Base_HeadView(orientation,parent)
{
    connect (this,&Base_HeadView::sectionClicked,this,&Base_HeadView::sectionClick);
/*    this->setStyleSheet ("QHeaderView::section {"
                        " color: Black;"
                        " padding-left: 4px;"
                        " border: 1px solid #6c6c6c;"
                        "}"
                        );
*/
}

MTK_base_HeadView::~MTK_base_HeadView()
{

}
