/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_base.h
* Description:  MTK  firmware download virtual func
* 1. init_io :  Qt inherit from QIODevice QSerialPort QTCP ...
* 2. download_stop : virtual func contral download stop
* 3. qGNSS_mTK_DownLoad: input download parameter
* 4. qGNSS_SetDownLoadBaudRate :set download baudrate
* **********************example*********************
* *MTK_init_IO(io)
* *qGNSS_SetDownLoadBaudRate(921600)
* *qGNSS_mTK_DownLoad(DA,ROM,QGNSS_Model,downmodel)
*
* History:
* Version Date                           Author          Description
*         2022-04-10      victor.gong
* ***************************************************************************/
#ifndef MTK_BASE_H
#define MTK_BASE_H
#include "brom_base.h"

class QGNSS_BROM_EXPORT MTK_Base : public BROM_BASE
{
    Q_OBJECT
    Q_PROPERTY(QMap_variant flashInfo READ flashInfo WRITE setFlashInfo NOTIFY flashInfoChanged)

public:
    explicit MTK_Base(QObject *parent=nullptr);
    virtual ~MTK_Base();
    typedef   QMap<QString,QVariant> QMap_variant;
    Q_INVOKABLE virtual void MTK_init_IO(IODvice *io)=0;
    Q_INVOKABLE virtual bool download_stop()=0;
    Q_INVOKABLE virtual void reset_buadRate()=0;
    /**
     * @brief qGNSS_SetDownLoadBaudRate set dlownload baudrate
     * @param baudrate
     */
    Q_INVOKABLE virtual void qGNSS_SetDownLoadBaudRate(int baudrate)=0;
    /**
     * @brief qGNSS_mTK_DownLoad  start download func
     * @param DA_Path :DA file
     * @param ROM_Path:rom file
     * @param qgnss_model :QGNSS_Model
     * @param downmode    :download mode
     */
    Q_INVOKABLE virtual bool qGNSS_mTK_DownLoad(QString DA_Path,QString ROM_Path,int qgnss_model,int downmode) =0;
     QMap<QString, QVariant> &flashInfo() ;

     uint32_t CRC32(uint8_t *data, uint32_t size);
     uint32_t CRC32(uint32_t initValue,uint8_t *data, uint32_t size);
Q_SIGNALS:

    void flashInfoChanged();

public Q_SLOTS:

    void setFlashInfo(const QMap<QString, QVariant> &newFlashInfo);

private:
    QMap<QString, QVariant> flashInfo_;
};

#endif // MTK_BASE_H
