/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         goke_base.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-11      victor.gong
* ***************************************************************************/

#ifndef GOKE_BASE_H
#define GOKE_BASE_H
#include "brom_base.h"
#include <array>
class QGNSS_BROM_EXPORT Goke_Base: public BROM_BASE
{
  Q_OBJECT
  Q_PROPERTY(QMap_variant flashInfo READ flashInfo WRITE setFlashInfo NOTIFY flashInfoChanged)
 public:
  explicit Goke_Base(QObject *parent=nullptr);
  virtual ~Goke_Base();
  typedef   QMap<QString,QVariant> QMap_variant;
  Q_INVOKABLE virtual void init_IO(IODvice *io)=0;
  Q_INVOKABLE virtual bool download_stop()=0;
  Q_INVOKABLE virtual void reset_buadRate()=0;
  Q_INVOKABLE virtual void qGNSS_SetDownLoadBaudRate(int baudrate)=0;
  Q_INVOKABLE virtual bool qGNSS_DownLoad(QString FIR_Path,int qgnss_model) =0;
  QMap<QString, QVariant> &flashInfo() ;

  uint32_t CRC32(uint8_t *data, uint32_t size);
  uint32_t CRC32(uint32_t initValue,uint8_t *data, uint32_t size);

  uint16_t BE_compute_checksum (uint8_t *buf, uint32_t buf_len)
  {
    uint16_t checksum = 0;
    if (buf == NULL || buf_len == 0) {
      return 0;
    }
    int i = 0;
    for (i = 0; i < buf_len / 2; i++) {
      checksum ^= *(uint16_t *) (buf + i * 2);
    }
    if ((buf_len % 2) == 1) {
      checksum ^= buf[i * 2];
    }
    return checksum;
  }

  uint32_t FW_compute_simple_checksum (uint8_t *buf, uint32_t buf_len) {
    uint32_t checksum = 0;
    if (buf == NULL || buf_len == 0) {
      return 0;
    }
    for (int i = 0; i < buf_len; i++) {
      checksum += *(buf + i);
    }
    return checksum;
  }

 Q_SIGNALS:
 void flashInfoChanged();

 public Q_SLOTS:

  void setFlashInfo(const QMap<QString, QVariant> &newFlashInfo);

 private:
  QMap<QString, QVariant> flashInfo_;
//{921600,921600,460800,230400,115200,57600,38400,19200,14400,9600};
 protected:
  static std::map<int,char> baudrates;
  std::array< int,9> baudrates_int{
      921600,
      460800,
      230400,
      115200,
      57600,
      38400,
      19200,
      14400,
      9600};
};

#endif // GOKE_BASE_H
