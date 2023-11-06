/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_headview.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-07      victor.gong
* ***************************************************************************/
#ifndef BASE_HEADVIEW_H
#define BASE_HEADVIEW_H

#include <QHeaderView>
#include <QPair>
#include <QPainter>
#include <QDebug>
class Base_HeadView : public QHeaderView
{
    Q_OBJECT
    Q_PROPERTY(bool firstisCheckable READ firstisCheckable WRITE setFirstisCheckable NOTIFY firstisCheckableChanged)
    Q_PROPERTY(bool firstChecked READ firstChecked WRITE setFirstChecked NOTIFY firstCheckedChanged)
public:
    explicit Base_HeadView(Qt::Orientation orientation, QWidget *parent = nullptr);
    virtual ~Base_HeadView();

    bool firstisCheckable() const;

    bool firstChecked() const;

Q_SIGNALS:
    void firstisCheckableChanged(bool firstisCheckable);

    void firstCheckedChanged(bool firstChecked);

public Q_SLOTS:

    void setFirstisCheckable(bool firstisCheckable);

    void sectionClick(int index);

    void setFirstChecked(bool firstChecked);

private:
    bool m_firstisCheckable=false;

    // QHeaderView interface
    bool m_firstChecked=true;

protected:
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
};

#endif // BASE_HEADVIEW_H
