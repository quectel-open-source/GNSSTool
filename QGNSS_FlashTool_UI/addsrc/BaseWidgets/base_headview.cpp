/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_headview.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-07      victor.gong
* ***************************************************************************/
#include "base_headview.h"

Base_HeadView::Base_HeadView(Qt::Orientation orientation, QWidget *parent)
    :QHeaderView(orientation,parent)
{
    setSectionsClickable (true);
}

Base_HeadView::~Base_HeadView()
{

}

void Base_HeadView::setFirstisCheckable(bool firstisCheckable)
{
    if (m_firstisCheckable == firstisCheckable)
        return;

    m_firstisCheckable = firstisCheckable;
    emit firstisCheckableChanged(m_firstisCheckable);
}

void Base_HeadView::sectionClick(int index)
{
    if(index==0&&firstisCheckable ())
    {
        if(m_firstChecked){setFirstChecked (false);}
        else{setFirstChecked (true);}
        update ();
    }
}

void Base_HeadView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (logicalIndex == 0&&firstisCheckable ())
    {
        QStyleOptionButton option;
        option.rect = QRect(0,3,20,20);
        option.state = QStyle::State_Enabled | QStyle::State_Active;

        if (m_firstChecked)
            option.state |= QStyle::State_On;
        else
            option.state |= QStyle::State_Off;

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);

    }

}

bool Base_HeadView::firstisCheckable() const
{
    return m_firstisCheckable;
}

bool Base_HeadView::firstChecked() const
{
    return m_firstChecked;
}

void Base_HeadView::setFirstChecked(bool firstChecked)
{
    if (m_firstChecked == firstChecked)
        return;

    m_firstChecked = firstChecked;
    emit firstCheckedChanged(m_firstChecked);
}
