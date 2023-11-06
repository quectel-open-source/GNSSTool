/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         qgnss_flashtool_help.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-29      victor.gong
* ***************************************************************************/
#ifndef QGNSS_FLASHTOOL_HELP_H
#define QGNSS_FLASHTOOL_HELP_H

#include <QObject>
#include "mtk_base.h"
class QGNSS_FLASHTOOL_HELP : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qgnssplatform qgnss_platform READ qgnss_platform WRITE setQgnss_platform NOTIFY qgnss_platformChanged)
    Q_PROPERTY(QString uiUrl READ uiUrl WRITE setUiUrl NOTIFY uiUrlChanged)
    Q_PROPERTY(KeyValue* currentmodel READ currentmodel WRITE setCurrentmodel NOTIFY currentmodelChanged)
    Q_PROPERTY(qgnssmodel model READ model WRITE setModel NOTIFY modelChanged)
public:

    ~QGNSS_FLASHTOOL_HELP();
    typedef  BROM_BASE::QGNSS_Platform qgnssplatform;
    typedef  BROM_BASE::QGNSS_Model qgnssmodel;
    static QSharedPointer<QGNSS_FLASHTOOL_HELP> get_instance();
public:
    const qgnssplatform &qgnss_platform() const;
          const QString &uiUrl()   const;
        KeyValue *currentmodel() const;

        qgnssmodel model() const;


Q_SIGNALS:
    void qgnss_platformChanged();

    void uiUrlChanged();

    void selectedmodelChanged();

    void currentmodelChanged();

    void modelChanged(qgnssmodel model);

public Q_SLOTS:
    void setQgnss_platform(const qgnssplatform &newQgnss_platform);

    void setUiUrl(const QString &newUiUrl);

    void setCurrentmodel(KeyValue *newCurrentmodel);
    void setModel(qgnssmodel model);


private:
    explicit QGNSS_FLASHTOOL_HELP(QObject *parent = nullptr);
    qgnssplatform qgnss_platform_;
    QString uiUrl_;
    KeyValue *currentmodel_;

    qgnssmodel m_model;
};

#endif // QGNSS_FLASHTOOL_HELP_H
