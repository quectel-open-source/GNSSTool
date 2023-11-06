/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_label.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-07      victor.gong
* ***************************************************************************/
#ifndef BASE_LABEL_H
#define BASE_LABEL_H

#include <QWidget>

class Base_Label : public QWidget
{
    Q_OBJECT
public:
    explicit Base_Label(QWidget *parent = nullptr);
    virtual~Base_Label();
signals:

};

#endif // BASE_LABEL_H
