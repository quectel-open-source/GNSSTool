/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         base_action.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-13      victor.gong
* ***************************************************************************/
#ifndef BASE_ACTION_H
#define BASE_ACTION_H

#include <QWidget>
#include <QAction>
#include <QEvent>
#include <QDebug>
class Base_Action : public QAction
{
    Q_OBJECT
    Q_PROPERTY(Custom_property* hoveredProperty READ hoveredProperty WRITE setHoveredProperty NOTIFY hoveredPropertyChanged)
public:
    explicit Base_Action(QObject *parent = nullptr);
    explicit Base_Action(const QString &text, QObject *parent = nullptr);
    explicit Base_Action(const QIcon &icon, const QString &text, QObject *parent = nullptr);
    ~Base_Action();
    struct Custom_property{
        QIcon icon;
        QString text;
    };
public:

    Custom_property* hoveredProperty() const;


Q_SIGNALS:

    void hoveredPropertyChanged(Custom_property* hoveredProperty);

public Q_SLOTS:

    void setHoveredProperty(Custom_property* hoveredProperty);

private:
    void RegisterMetaType();

    Custom_property* m_hoveredProperty=nullptr;
};
Q_DECLARE_METATYPE (Base_Action::Custom_property)
Q_DECLARE_METATYPE (Base_Action::Custom_property*)
#endif // BASE_ACTION_H
