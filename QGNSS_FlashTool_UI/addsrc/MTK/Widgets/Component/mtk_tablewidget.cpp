/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_tablewidget.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-05      victor.gong
* ***************************************************************************/
#include "mtk_tablewidget.h"

MTK_TableWidget::MTK_TableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    this->setEditTriggers (QTableWidget::NoEditTriggers);
    this->setSelectionMode (QTableWidget::NoSelection);
    this->setAlternatingRowColors(true);
    this->verticalHeader ()->hide();

    this->horizontalHeader ()->setSectionResizeMode (QHeaderView::ResizeToContents);
    this->horizontalHeader ()->setMaximumSectionSize (600);
}

MTK_TableWidget::MTK_TableWidget(int rows, int columns, QWidget *parent)
                :QTableWidget(rows,columns,parent)
{

}

MTK_TableWidget::~MTK_TableWidget()
{

}
