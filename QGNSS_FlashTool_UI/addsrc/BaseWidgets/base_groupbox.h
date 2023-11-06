/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_groupbox.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-13      victor.gong
* ***************************************************************************/
#ifndef BASE_GROUPBOX_H
#define BASE_GROUPBOX_H


#include<QGroupBox>

class Base_GroupBox : public QGroupBox
{
    Q_OBJECT
public:

    explicit Base_GroupBox(QWidget *parent = nullptr);
    explicit Base_GroupBox(const QString &title, QWidget *parent = nullptr);
    virtual ~Base_GroupBox();
Q_SIGNALS:

public  Q_SLOTS:

private:
};

#endif // BASE_GROUPBOX_H
