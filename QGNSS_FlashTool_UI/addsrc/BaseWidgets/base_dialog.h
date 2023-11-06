/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_dialog.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-07      victor.gong
* ***************************************************************************/
#ifndef BASE_DIALOG_H
#define BASE_DIALOG_H


#include <QDialog>
class Base_Dialog:public QDialog
{
    Q_OBJECT
public:
    explicit Base_Dialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual   ~Base_Dialog();

Q_SIGNALS:

public Q_SLOTS:

private:
};

#endif // BASE_DIALOG_H
