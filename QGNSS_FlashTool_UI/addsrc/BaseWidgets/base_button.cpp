/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_button.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-07      victor.gong
* ***************************************************************************/
#include "base_button.h"

Base_Button::Base_Button(QWidget *parent)
            :QPushButton{parent}
{

}

Base_Button::Base_Button(const QString &text, QWidget *parent)
            :QPushButton{text,parent}
{

}

Base_Button::Base_Button(const QIcon &icon, const QString &text, QWidget *parent)
            :QPushButton{icon,text,parent}
{

}

Base_Button::~Base_Button()
{

}
