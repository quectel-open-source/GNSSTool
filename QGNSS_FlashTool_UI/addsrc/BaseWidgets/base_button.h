/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_button.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-07      victor.gong
* ***************************************************************************/
#ifndef BASE_BUTTON_H
#define BASE_BUTTON_H

#include <QPushButton>

class Base_Button : public QPushButton
{
    Q_OBJECT
public:
    explicit Base_Button(QWidget *parent = nullptr);
    explicit Base_Button(const QString &text, QWidget *parent = nullptr);
    Base_Button(const QIcon& icon, const QString &text, QWidget *parent = nullptr);
    virtual ~Base_Button();
Q_SIGNALS:
public Q_SLOTS:
private:

};

#endif // BASE_BUTTON_H
