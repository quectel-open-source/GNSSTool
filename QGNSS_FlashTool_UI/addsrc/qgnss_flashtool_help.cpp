/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         qgnss_flashtool_help.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-29      victor.gong
* ***************************************************************************/
#include "qgnss_flashtool_help.h"

QGNSS_FLASHTOOL_HELP::QGNSS_FLASHTOOL_HELP(QObject *parent)
    : QObject{parent}
{

}
QGNSS_FLASHTOOL_HELP::~QGNSS_FLASHTOOL_HELP()
{

}

QSharedPointer<QGNSS_FLASHTOOL_HELP> QGNSS_FLASHTOOL_HELP::get_instance()
{
    static QSharedPointer<QGNSS_FLASHTOOL_HELP> qgnss_help=QSharedPointer<QGNSS_FLASHTOOL_HELP>(new QGNSS_FLASHTOOL_HELP);
    return qgnss_help;
}

const QGNSS_FLASHTOOL_HELP::qgnssplatform &QGNSS_FLASHTOOL_HELP::qgnss_platform() const
{
    return qgnss_platform_;
}

void QGNSS_FLASHTOOL_HELP::setQgnss_platform(const qgnssplatform &newQgnss_platform)
{
    if (qgnss_platform_ == newQgnss_platform)
        return;
    qgnss_platform_ = newQgnss_platform;
    emit qgnss_platformChanged();
}

const QString &QGNSS_FLASHTOOL_HELP::uiUrl() const
{
    return uiUrl_;
}

void QGNSS_FLASHTOOL_HELP::setUiUrl(const QString &newUiUrl)
{
    if (uiUrl_ == newUiUrl)
        return;
    uiUrl_ = newUiUrl;
    emit uiUrlChanged();
}

KeyValue *QGNSS_FLASHTOOL_HELP::currentmodel() const
{
    return currentmodel_;
}

void QGNSS_FLASHTOOL_HELP::setCurrentmodel(KeyValue *newCurrentmodel)
{
    if (currentmodel_ == newCurrentmodel)
        return;
    currentmodel_ = newCurrentmodel;
    emit currentmodelChanged();
}

QGNSS_FLASHTOOL_HELP::qgnssmodel QGNSS_FLASHTOOL_HELP::model() const
{
    return m_model;
}

void QGNSS_FLASHTOOL_HELP::setModel(qgnssmodel model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(m_model);
}
