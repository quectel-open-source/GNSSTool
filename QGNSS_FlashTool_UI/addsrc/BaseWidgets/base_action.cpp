/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_action.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-13      victor.gong
* ***************************************************************************/
#include "base_action.h"


Base_Action::Base_Action(QObject *parent)
            :QAction (parent)
{
    RegisterMetaType();
}

Base_Action::Base_Action(const QString &text, QObject *parent)
            :QAction (text,parent)
{
    RegisterMetaType();
}

Base_Action::Base_Action(const QIcon &icon, const QString &text, QObject *parent)
            :QAction (icon,text,parent)
{
    RegisterMetaType();
}

Base_Action::~Base_Action()
{

}

void Base_Action::RegisterMetaType()
{
    qRegisterMetaType<Custom_property>();
    qRegisterMetaType<Custom_property*>();
    connect (this,&Base_Action::hovered,this,[&](){
        qDebug()<<"hovered";
        setText ("hovered");
    });
}

void Base_Action::setHoveredProperty(Custom_property* hoveredProperty)
{
    if (m_hoveredProperty == hoveredProperty)
        return;
    m_hoveredProperty = hoveredProperty;
    emit hoveredPropertyChanged(m_hoveredProperty);
}
Base_Action::Custom_property* Base_Action::hoveredProperty() const
{
    return m_hoveredProperty;
}

