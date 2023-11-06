/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_tablewidget.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-05      victor.gong
* ***************************************************************************/
#ifndef MTK_TABLEWIDGET_H
#define MTK_TABLEWIDGET_H

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QCursor>
#include <QToolTip>

class MTK_TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit MTK_TableWidget(QWidget *parent = nullptr);
             MTK_TableWidget(int rows, int columns, QWidget *parent = nullptr);
             virtual ~MTK_TableWidget();
Q_SIGNALS:

public Q_SLOTS:

private:

};

#endif // MTK_TABLEWIDGET_H
