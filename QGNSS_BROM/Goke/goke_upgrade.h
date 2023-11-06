/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         goke_upgrade.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-11      victor.gong
* ***************************************************************************/

#ifndef GOKE_UPGRADE_H
#define GOKE_UPGRADE_H
#include "goke_base.h"

#define HANKSHAKE_TRY_TIMES 60
#define SYNC_TRY_TIMES 50
//#define BELENGTH 2048
#define BEPACKAGESIZE 256

#define QERROR(x) \
    setErroString (x);\
    setDownload_state(Download_State::State_CMD_Fail);

#define QINFO(x) \
    setBbmsgStr ({BROM_BASE::brmsgtype::bb_Info,x});\

class Goke_upgrade:public Goke_Base
{
  Q_OBJECT
  Q_PROPERTY(Download_State download_state READ download_state WRITE setDownload_state NOTIFY download_stateChanged)

 public:
  Goke_upgrade(Goke_Base *parent=nullptr);
  ~Goke_upgrade();

  enum Download_State{
    State_Unstart,
    State_handshake_with_Module,
    State_Send_BE,
    State_jump_to_BE,
    State_sync_with_BE,
    State_send_Firmware,
    State_Reset,
    //**********custom**************//
    State_Finished,
    State_CMD_Fail,
    Custom_DownLoad_Stoping,
    Custom_DownLoad_Stoped
  };
  Q_ENUM (Download_State);
 public:
  const Download_State &download_state() const;
  Q_INVOKABLE void init_IO(IODvice *io) override;
  Q_INVOKABLE bool download_stop() override;
  Q_INVOKABLE void reset_buadRate() override;
  Q_INVOKABLE bool qGNSS_DownLoad(QString ROM_Path,int model) override;
  Q_INVOKABLE void qGNSS_SetDownLoadBaudRate(int baudrate) override;

 Q_SIGNALS:
  void download_stateChanged(const Download_State &state);

 public Q_SLOTS:
  void setDownload_state(const Download_State &newDownload_state);

  void setDownload_stateStr(QString state);

 private:
  bool handshake_with_Module();
  bool Send_BE();
  bool Jump2BE();
  bool SyncBE();
  bool Download_Firmware();
  bool Reset();
  int download_baudRate_=460800;        /*default downloadbaudrate*/
  Download_State download_state_=Download_State::State_Unstart;
  // char BE_data[BELENGTH]={0};
  QByteArray firmwaredata_;
  QByteArray BE_data_;
};

#endif // GOKE_UPGRADE_H
