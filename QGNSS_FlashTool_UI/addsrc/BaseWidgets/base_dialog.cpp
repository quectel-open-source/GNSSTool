/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_dialog.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-07      victor.gong
* ***************************************************************************/
#include "base_dialog.h"

Base_Dialog::Base_Dialog(QWidget *parent , Qt::WindowFlags f)
            :QDialog(parent,f)
{
    this->setWindowModality (Qt::ApplicationModal);//半模态
}

Base_Dialog::~Base_Dialog()
{

}
