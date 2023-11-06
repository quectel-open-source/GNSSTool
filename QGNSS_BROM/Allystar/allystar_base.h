/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         allystar_base.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-29      victor.gong
* ***************************************************************************/
#ifndef ALLYSTAR_BASE_H
#define ALLYSTAR_BASE_H

#include "brom_base.h"
class QGNSS_BROM_EXPORT AllyStar_Base : public BROM_BASE
{
  Q_OBJECT
  Q_PROPERTY(QMap_variant flashInfo READ flashInfo WRITE setFlashInfo NOTIFY flashInfoChanged)
 public:
  explicit AllyStar_Base(QObject *parent=nullptr);
  virtual ~AllyStar_Base();
  typedef   QMap<QString,QVariant> QMap_variant;
  Q_INVOKABLE virtual void init_IO(IODvice *io)=0;
  Q_INVOKABLE virtual bool download_stop()=0;
  Q_INVOKABLE virtual void reset_buadRate()=0;
  Q_INVOKABLE virtual void qGNSS_SetDownLoadBaudRate(int baudrate)=0;
  Q_INVOKABLE virtual bool qGNSS_DownLoad(QString cyfm_Path,int qgnss_model,int downmode) =0;
  QMap<QString, QVariant> &flashInfo();

 Q_SIGNALS:
  void flashInfoChanged();
 public Q_SLOTS:

  void setFlashInfo(const QMap<QString, QVariant> &newFlashInfo);

 private:
  QMap<QString, QVariant> flashInfo_;
};

#endif // ALLYSTAR_BASE_H
