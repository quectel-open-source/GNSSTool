/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_groupbox.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-13      victor.gong
* ***************************************************************************/
#include "base_groupbox.h"

Base_GroupBox::Base_GroupBox(QWidget *parent)
              : QGroupBox(parent)
{

}

Base_GroupBox::Base_GroupBox(const QString &title, QWidget *parent )
            : QGroupBox(title,parent)
{

}

Base_GroupBox::~Base_GroupBox()
{

}
