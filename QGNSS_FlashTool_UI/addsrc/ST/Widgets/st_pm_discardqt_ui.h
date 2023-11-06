#ifndef ST_MQSP_DISCARDQT_UI_H
#define ST_MQSP_DISCARDQT_UI_H

#include <QObject>
#include <QWidget>
#include "st_base_window.h"
#include "t5_wmcu_upg_hp.h"
#include "t5_wmcu_download_hp.h"
#include "myUtilities.h"

class Thread_Help:public QObject
{
  Q_OBJECT
 public:
  t5_wmcu_download_hp * t5dwn=nullptr;
  t5_wmcu_upg *t5upg=nullptr;
  Thread_Help();


 Q_SIGNALS:
  void resultsChanged(bool);
 public Q_SLOTS:
  void run_t5dwn( );
  void run_t5upg( );
  void stop();

};

class ST_PM_discardqt_ui:public ST_Base_Window
{
  Q_OBJECT
 public:
  explicit ST_PM_discardqt_ui(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
  ~ST_PM_discardqt_ui();
  enum cfg_key
  {
    Baudrate,
    Downmode,
    GNSSUpgradePath,
    MCUUpgradePath,
    MCUDownLoadPath,
    SMCU_UpgradePath,
    SGNSS_UpgradePath
  };
  Q_ENUM (cfg_key)

  enum class Down_Mode:unsigned char
  {
    MCU_Download,
    MCU_Upgrade,
    GNSS_Upgrade,
    //slave upg
    SMCU_Upgrade,
    SGNSS_Upgrade
    };
  Q_ENUM (Down_Mode)

  Q_INVOKABLE virtual  void start() override;
  Q_INVOKABLE virtual  void stop() override;
  Q_INVOKABLE virtual  void ini_IO(IODvice *io) override;

  void open_file() ;
  void load_file() ;
  //ST_MQSP_Model *STModel=nullptr;
  t5_wmcu_download_hp::t5_dwd_params t5_dwd_params;
  t5_wmcu_upg::t5_upg_params         t5_upg_params;

  t5_wmcu_download_hp t5_dwd_;
  t5_wmcu_upg         t5_upg_;
  QThread td;
  Thread_Help *hp;
 Q_SIGNALS:
  void  startt5DwnChanged();
  void  startt5upgChanged();

  void printMsgChanged(int msgtype, QString msg);
 public Q_SLOTS:

 private:

  QToolButton *GNSSupgradebt_  =nullptr;
  QToolButton *mcuupgradebt_   =nullptr;
  QToolButton *mcuDownloadbt_  =nullptr;
  QToolButton *SMCU_Upgradebt_ =nullptr;
  QToolButton *SGNSS_Upgradebt_=nullptr;
  QString dwmd;
 public:
  virtual void cfg_parameter()      override;
  virtual void cfg_widget()         override;
  bool         event(QEvent *event) override;
  void         wgt_enable(bool Run) override;
  void         mnu_enable(bool enable,QString baudrate="460800");
  IOserial *seril;
  void print(QG::MsgType msgtype, const char* msg, std::size_t size);
  QTimer timer;
  QTime starttmr;
};

#endif // ST_MQSP_DISCARDQT_UI_H
