#include "st_pm_discardqt_ui.h"

using namespace std::placeholders;


ST_PM_discardqt_ui::ST_PM_discardqt_ui(QWidget *parent, Qt::WindowFlags flags)
:ST_Base_Window(parent,flags),
      td{this}
{
  QG::set_print_cb (std::bind(&ST_PM_discardqt_ui::print,this,_1,_2,_3));
  GNSSupgradebt_=new QToolButton;
  mcuupgradebt_=new QToolButton;
  mcuDownloadbt_=new QToolButton;
  SMCU_Upgradebt_=new QToolButton;
  SGNSS_Upgradebt_=new QToolButton;
  optHlayout_->setEnabled (false);
  cfg_parameter();
  cfg_widget();
  hp=new Thread_Help;
  hp->moveToThread (&td);
  td.start ();
  connect (this,&ST_PM_discardqt_ui::startt5DwnChanged,hp,&Thread_Help::run_t5dwn);
  connect (this,&ST_PM_discardqt_ui::startt5upgChanged,hp,&Thread_Help::run_t5upg );
  connect (hp,&Thread_Help::resultsChanged ,this,[=](bool success)
          {
            if(success)
              statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)+": Success"});

            statuswgt_->setBt_open(true);
            wgt_enable(false);
            timer.stop ();
          });
  connect (this,&ST_PM_discardqt_ui::printMsgChanged,this,[&](int msgtype, QString msg)
          {
            switch ((QG::MsgType)msgtype)
            {
            case QG::NONE_:
                 break;
            case QG::DEBUG_:
            case QG::INFO_:
              statuswgt_->set_widget ({Status_WGT::DownloadStep,QString("%1").arg (msg)});
              break;
            case QG::WARNING_:
              break;
            case QG::ERROR_:
              statuswgt_->set_widget ({Status_WGT::Error,QString("%1").arg (msg)});
              break;
            case QG::PROGRESS_:
            {
              auto msg_s= msg;
              auto split_s= msg_s.split ("*");
              downbuadrate_->setText (split_s[5]);
              statuswgt_->setProgress (split_s.back ().toInt (), statuswgt_->current_status ()->info_msg);
            }
              break;
            }
          });
}

ST_PM_discardqt_ui::~ST_PM_discardqt_ui()
{
  td.quit ();
  td.wait ();
  hp->deleteLater ();
  if(t5_upg_params.serialportinfo)
  {
    delete  t5_upg_params.serialportinfo;
  }
  if(t5_dwd_params.serialportinfo)
  {
    delete t5_dwd_params.serialportinfo;
  }
  qDebug()<<Q_FUNC_INFO;
}

void ST_PM_discardqt_ui::start()
{
  wgt_enable(true);
  qgnss_help_=QGNSS_FLASHTOOL_HELP::get_instance ();
  statuswgt_->setBt_open(false);
  int  baudrate= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Baudrate,baudrates_[0]).toInt ();
  int  mode=     downmode_.indexOf (CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downmode,downmode_[0]).toString ());

  t5_dwd_params.serialportinfo->baudrate=baudrate;
  t5_dwd_params.serialportinfo->parity=Parity::EvenParity;

  t5_upg_params.serialportinfo->baudrate=baudrate;
  t5_upg_params.serialportinfo->parity=Parity::NoParity;

  dwmd=IODvice::Enum_Value2Key<ST_PM_discardqt_ui::Down_Mode>((ST_PM_discardqt_ui::Down_Mode)mode);
  statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)});


  starttmr=QTime::currentTime ();

  timer.setInterval (500);
  timer.start ();
  connect (&timer,&QTimer::timeout,this,[&](){
    QTime end=QTime::currentTime ();
    int t_times=starttmr.secsTo (end);
    downloadtime_->setText (tr("Download Time:")+QString::number (t_times)+" S");
  });
  portName_->setText (QString::fromStdString (t5_dwd_params.serialportinfo->port_name));
  downmodetxt_->setText (dwmd);

  switch ((ST_PM_discardqt_ui::Down_Mode)mode)
  {
    case ST_PM_discardqt_ui::Down_Mode::MCU_Download:
    {
      t5_dwd_params.file_path=edit_firmware_path_->text ().toLocal8Bit ().toStdString ();
      t5_dwd_.set_params (t5_dwd_params);
      hp->t5dwn=&t5_dwd_;
      Q_EMIT startt5DwnChanged();
    }
      break;
    case ST_PM_discardqt_ui::Down_Mode::MCU_Upgrade:
    {
      t5_upg_params.file_path=edit_firmware_path_->text ().toLocal8Bit ().toStdString ();
      t5_upg_params.classid_=ST8100_wmcu_fwupg::ClassID::APP;
      t5_upg_.set_params (t5_upg_params);
      hp->t5upg=&t5_upg_;
      Q_EMIT startt5upgChanged();
    }
      break;
    case ST_PM_discardqt_ui::Down_Mode::GNSS_Upgrade:
    {
      t5_upg_params.file_path=edit_firmware_path_->text ().toLocal8Bit ().toStdString ();
      t5_upg_params.classid_=ST8100_wmcu_fwupg::ClassID::GNSS;
      t5_upg_.set_params (t5_upg_params);
      hp->t5upg=&t5_upg_;
      Q_EMIT startt5upgChanged();
    }
    break;
    case ST_PM_discardqt_ui::Down_Mode::SMCU_Upgrade:
    {
      t5_upg_params.file_path=edit_firmware_path_->text ().toLocal8Bit ().toStdString ();
      t5_upg_params.classid_=ST8100_wmcu_fwupg::ClassID::SAPP;
      t5_upg_.set_params (t5_upg_params);
      hp->t5upg=&t5_upg_;
      Q_EMIT startt5upgChanged();
    }
    break;
    case ST_PM_discardqt_ui::Down_Mode::SGNSS_Upgrade:
    {
      t5_upg_params.file_path=edit_firmware_path_->text ().toLocal8Bit ().toStdString ();
      t5_upg_params.classid_=ST8100_wmcu_fwupg::ClassID::SGNSS;
      t5_upg_.set_params (t5_upg_params);
      hp->t5upg=&t5_upg_;
      Q_EMIT startt5upgChanged();
    }
    break;
  }

}

void ST_PM_discardqt_ui::stop()
{
  //stBase->download_stop ();
  hp->stop ();
}

void ST_PM_discardqt_ui::ini_IO(IODvice *io)
{
  seril=static_cast<IOserial *>(io);

  t5_upg_params.serialportinfo =new SerialPortInfo;
  t5_dwd_params.serialportinfo =new SerialPortInfo;

  auto t5dwdparam_serialinfo=t5_dwd_params.serialportinfo;
//  t5dwdparam_serialinfo->uart_handle=seril->get_handle ();

  auto t5upgparam_serialinfo=t5_upg_params.serialportinfo;
//       t5upgparam_serialinfo->uart_handle=seril->get_handle ();
  auto info =seril->getSerial_INFO ();
  t5dwdparam_serialinfo->port_name=info->portname_.toStdString ();
  t5dwdparam_serialinfo->parity=(Parity)info->Parity_;
  t5dwdparam_serialinfo->baudrate=info->baudRate_;
  t5dwdparam_serialinfo->databits=(DataBits)info->DataBits_;
  t5dwdparam_serialinfo->stopbits=(StopBits)info->StopBits_;
  t5dwdparam_serialinfo->flowcontrol=(FlowControl)info->FlowControl_;
  t5dwdparam_serialinfo->uart_handle=INVALID_HANDLE_VALUE;

  t5upgparam_serialinfo->port_name=info->portname_.toStdString ();
  t5upgparam_serialinfo->parity=(Parity)info->Parity_;
  t5upgparam_serialinfo->baudrate=info->baudRate_;
  t5upgparam_serialinfo->databits=(DataBits)info->DataBits_;
  t5upgparam_serialinfo->stopbits=(StopBits)info->StopBits_;
  t5upgparam_serialinfo->flowcontrol=(FlowControl)info->FlowControl_;
  t5upgparam_serialinfo->uart_handle=INVALID_HANDLE_VALUE;
}

void ST_PM_discardqt_ui::cfg_parameter()
{
  baudrates_=
          {
          "57600",
          "115200",
          "230400",
          "460800",
          "921600",
          };

  downmode_=
      {
          "MCU_Download",
          "MCU_upgrade",
          "GNSS_upgrade",
          "SMCU_Upgrade",
          "SGNSS_Upgrade"
      };
}

void ST_PM_discardqt_ui::cfg_widget()
{
  menu_setting_=     menuBar ()->addMenu (tr("Settings"));
  menu_baudrate_= menu_setting_->addMenu (tr("Download Baudrate"));

  showExpertMenu_=new QAction;
  addAction (showExpertMenu_);
  showExpertMenu_->setShortcut ( QKeySequence("Ctrl+Shift+E") );
  showExpert_= menuBar ()->addMenu (tr("Expert"));
  connect (showExpertMenu_,&QAction::triggered,this,[=](bool chk)
          {
            qDebug()<<chk;
            if(showExpert_->menuAction()->isVisible ())
            {
              showExpert_->menuAction ()->setVisible (false);
            }
            else
            {
              showExpert_->menuAction ()->setVisible (true);
            }
          });
  showExpert_->menuAction ()->setVisible (false);
  downloadOpt_ac_=new QAction(QIcon(),"EnableDownloadOpt");
  downloadOpt_ac_->setCheckable (true);
  downloadOpt_ac_->setChecked (false);
  showExpert_->addAction (downloadOpt_ac_);
  //[1]toolbar
  toolbar_->setFloatable (false);
  toolbar_->setMovable (false);
  toolbar_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  QButtonGroup *btgroup=new QButtonGroup(this);
  QString boxstyle="QToolButton:hover{"
      "background:#ffffff;"
      "}"
      "QToolButton:checked{"
      "background:#ffffff;"
      "}";
  mcuDownloadbt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
  mcuDownloadbt_->setText (tr("MCU_Download"));
  mcuDownloadbt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  mcuDownloadbt_->setStyleSheet (boxstyle);
  mcuDownloadbt_->setCheckable (true);

  mcuupgradebt_->setVisible (true);
  mcuupgradebt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
  mcuupgradebt_->setText (tr("MCU_Upgrade"));
  mcuupgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  mcuupgradebt_->setStyleSheet (boxstyle);
  mcuupgradebt_->setCheckable (true);


  GNSSupgradebt_->setVisible (true);
  GNSSupgradebt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
  GNSSupgradebt_->setText (tr("GNSS_Upgrade"));
  GNSSupgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  GNSSupgradebt_->setStyleSheet (boxstyle);
  GNSSupgradebt_->setCheckable (true);

  SMCU_Upgradebt_->setVisible (true);
  SMCU_Upgradebt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
  SMCU_Upgradebt_->setText (tr("Slave_MCU_Upgrade"));
  SMCU_Upgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  SMCU_Upgradebt_->setStyleSheet (boxstyle);
  SMCU_Upgradebt_->setCheckable (true);

  SGNSS_Upgradebt_->setVisible (true);
  SGNSS_Upgradebt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
  SGNSS_Upgradebt_->setText (tr("Slave_GNSS_Upgrade"));
  SGNSS_Upgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  SGNSS_Upgradebt_->setStyleSheet (boxstyle);
  SGNSS_Upgradebt_->setCheckable (true);

  btgroup->addButton (mcuDownloadbt_);
  btgroup->addButton (mcuupgradebt_);
  btgroup->addButton (GNSSupgradebt_);
  btgroup->addButton (SMCU_Upgradebt_);
  btgroup->addButton (SGNSS_Upgradebt_);

  toolbar_->addWidget (mcuDownloadbt_)  ;
  toolbar_->addWidget (mcuupgradebt_)   ;
  toolbar_->addWidget (GNSSupgradebt_)  ;
  toolbar_->addWidget (SMCU_Upgradebt_) ;
  toolbar_->addWidget (SGNSS_Upgradebt_);
  addToolBar (toolbar_);
  //[0]menubar
  connect (this,&ST_PM_discardqt_ui::currentBaudrateChanged,this,[&](int index){
    CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Baudrate,baudrates_[index]);
  });
  connect (this,&ST_PM_discardqt_ui::currentDownmodeChanged,this,[&](int index){
    CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downmode,downmode_[index]);
  });
  connect (menu_baudrate_,&QMenu::triggered,this,[&](QAction *action){
    setCurrentBaudrate (baudrates_.indexOf (action->text ()));
  });
  connect (mcuDownloadbt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    mnu_enable(true);
    mcuDownloadbt_->setChecked (true);
    setCurrentDownmode (0);
    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::MCUDownLoadPath,edit_firmware_path_->text ()).toString ());
    uint filesize=0;
    QString crc_txt="";
    GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
    filesize_txt->setText (QString::number (filesize));
    filecrc_txt->setText (crc_txt);
  });
  connect (mcuupgradebt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    mnu_enable(false);
    mcuupgradebt_->setChecked (true);
    setCurrentDownmode (1);
    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::MCUUpgradePath,edit_firmware_path_->text ()).toString ());
    uint filesize=0;
    QString crc_txt="";
    GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
    filesize_txt->setText (QString::number (filesize));
    filecrc_txt->setText (crc_txt);
  });
  connect (GNSSupgradebt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    mnu_enable(false);
    GNSSupgradebt_->setChecked (true);
    setCurrentDownmode (2);
    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::GNSSUpgradePath,edit_firmware_path_->text ()).toString ());
    uint filesize=0;
    QString crc_txt="";
    GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
    filesize_txt->setText (QString::number (filesize));
    filecrc_txt->setText (crc_txt);
  });

  connect (SMCU_Upgradebt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    mnu_enable(false);
    SMCU_Upgradebt_->setChecked (true);
    setCurrentDownmode (3);
    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::SMCU_UpgradePath,edit_firmware_path_->text ()).toString ());
    uint filesize=0;
    QString crc_txt="";
    GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
    filesize_txt->setText (QString::number (filesize));
    filecrc_txt->setText (crc_txt);
  });

  connect (SGNSS_Upgradebt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    mnu_enable(false);
    SGNSS_Upgradebt_->setChecked (true);
    setCurrentDownmode (4);
    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::SGNSS_UpgradePath,edit_firmware_path_->text ()).toString ());
    uint filesize=0;
    QString crc_txt="";
    GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
    filesize_txt->setText (QString::number (filesize));
    filecrc_txt->setText (crc_txt);
  });


  for (auto &act :baudrates_)
  {
    QAction *action=  menu_baudrate_->addAction (act);
    action->setCheckable (true);
    action->setActionGroup (baudrate_group_);
    if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Baudrate,baudrates_[4]))
    {
      action->setChecked (true);
      setCurrentBaudrate (baudrates_.indexOf (action->text ()));
    }
  }

  QString  mode= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downmode,downmode_[0]).toString ();
  int index=downmode_.indexOf (mode);//(ST_MQSP_Upgrade::Down_Mode)
  if(index<0)
  {
    index=0;
  }
  setCurrentDownmode (index);
  switch ((ST_PM_discardqt_ui::Down_Mode)index)
  {
    case ST_PM_discardqt_ui::Down_Mode::MCU_Download:
      mcuDownloadbt_->click ();
      break;
    case ST_PM_discardqt_ui::Down_Mode::MCU_Upgrade:
      mcuupgradebt_->click ();
      break;
    case ST_PM_discardqt_ui::Down_Mode::GNSS_Upgrade:
      GNSSupgradebt_->click ();
      break;
    case ST_PM_discardqt_ui::Down_Mode::SGNSS_Upgrade:
      SMCU_Upgradebt_->click ();
      break;
    case ST_PM_discardqt_ui::Down_Mode::SMCU_Upgrade:
      SGNSS_Upgradebt_->click ();
      break;
  }

  //[2]mainVlayout_
  mainVlayout_->addLayout (dafileHlayout_);
  mainVlayout_->addLayout (crclayout_);
  label_firmware_name_->setText (tr("Firmware File:"));
  dafileHlayout_->addWidget (label_firmware_name_);
  dafileHlayout_->addWidget (edit_firmware_path_);
  dafileHlayout_->addWidget (openfileBT_);

  crclayout_->setColumnStretch (0,1);
  crclayout_->addWidget (size_label,0,1);
  crclayout_->addWidget (filesize_txt,0,2);
  crclayout_->addWidget (crc_label,1,1);
  crclayout_->addWidget (filecrc_txt,1,2);

  connect (openfileBT_,&QPushButton::clicked,this,[&](){
    open_file();
  });

  load_file();
  mainVlayout_->addWidget (statuswgt_);
  connect (statuswgt_,&Status_WGT::bt_clickChanged ,this,[&](){
    if(!statuswgt_->bt_open ())
    {
      stop();
    }
    else
    {
      start ();
    }
  });
  //[5] status toolbar
  statusbar_->addWidget (downrate_);
  statusbar_->addWidget (portName_);
  statusbar_->addWidget (downbuadrate_);
  statusbar_->addWidget (downmodetxt_);
  statusbar_->addWidget (downloadtime_);



  downrate_->setMinimumWidth (180);
  downrate_->setAlignment(Qt::AlignHCenter);
  portName_->setMinimumSize(portName_->sizeHint());
  portName_->setAlignment(Qt::AlignHCenter);
  downbuadrate_->setMinimumSize(downbuadrate_->sizeHint());
  downbuadrate_->setAlignment(Qt::AlignHCenter);
  downloadtime_->setMinimumSize(downloadtime_->sizeHint());
  downloadtime_->setAlignment(Qt::AlignHCenter);
  //end
  setCentralWidget (content_);
  content_->setLayout (mainVlayout_);
  this->setMinimumSize ({840,450});
}

bool ST_PM_discardqt_ui::event(QEvent *event)
{
  switch (event->type ()) {
    case QEvent::Close:
      if(!statuswgt_->bt_open ())
      {
        Misc::Utilities::showMessageBox(
            tr("Downloading !")
                ,tr("Close while download finished or stoped!")
                ,tr("Warning!")
                ,QMessageBox::Ok
            );
        event->ignore();
        return true;
      }
      this->close ();
      this->deleteLater ();

      break;
    default:
      return ST_Base_Window::event (event);
  }
  return ST_Base_Window::event (event);
}

void ST_PM_discardqt_ui::open_file()
{
  QString fileName;
  QString filter;
  filter=tr(" (*.bin )");
  fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select File"), nullptr,filter );
  if(fileName.isEmpty ()){return;}

  switch ((ST_PM_discardqt_ui::Down_Mode)currentDownmode ())
  {
    case ST_PM_discardqt_ui::Down_Mode::MCU_Download:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUDownLoadPath,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUDownLoadPath,fileName).toString ());
      break;
    case ST_PM_discardqt_ui::Down_Mode::MCU_Upgrade:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUUpgradePath,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUUpgradePath,fileName).toString ());
      break;
    case ST_PM_discardqt_ui::Down_Mode::GNSS_Upgrade:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::GNSSUpgradePath,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::GNSSUpgradePath,fileName).toString ());
      break;

    case ST_PM_discardqt_ui::Down_Mode::SMCU_Upgrade:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::SMCU_UpgradePath,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::SMCU_UpgradePath,fileName).toString ());
      break;
    case ST_PM_discardqt_ui::Down_Mode::SGNSS_Upgrade:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::SGNSS_UpgradePath,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::SGNSS_UpgradePath,fileName).toString ());
      break;
  }
  uint filesize=0;
  QString crc_txt="";
  GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
  filesize_txt->setText (QString::number (filesize));
  filecrc_txt->setText (crc_txt);
}

void ST_PM_discardqt_ui::load_file()
{
  switch ((ST_PM_discardqt_ui::Down_Mode)currentDownmode ())
  {
    case ST_PM_discardqt_ui::Down_Mode::MCU_Download:
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUDownLoadPath,"").toString ());
      break;
    case ST_PM_discardqt_ui::Down_Mode::MCU_Upgrade:
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUUpgradePath,"").toString ());
      break;
    case ST_PM_discardqt_ui::Down_Mode::GNSS_Upgrade:
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::GNSSUpgradePath,"").toString ());
      break;
  }
  uint filesize=0;
  QString crc_txt="";
  GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
  filesize_txt->setText (QString::number (filesize));
  filecrc_txt->setText (crc_txt);
}

void ST_PM_discardqt_ui::wgt_enable(bool Run)
{
  if(Run)
  {
    menubar_       ->setEnabled (false);
    GNSSupgradebt_ ->setEnabled (false);
    mcuupgradebt_  ->setEnabled (false);
    mcuDownloadbt_ ->setEnabled (false);
    SGNSS_Upgradebt_->setEnabled (false);
    SMCU_Upgradebt_->setEnabled (false);
    cb_eraseNVM_   ->setEnabled (false);
    openfileBT_    ->setEnabled (false);
    //c_2m_box->setEnabled (false);
    //c_4m_box->setEnabled (false);
    edit_firmware_path_->setEnabled (false);
  }
  else
  {
    menubar_       ->setEnabled (true);
    GNSSupgradebt_ ->setEnabled (true);
    mcuupgradebt_  ->setEnabled (true);
    mcuDownloadbt_ ->setEnabled (true);
    SGNSS_Upgradebt_->setEnabled (true);
    SMCU_Upgradebt_->setEnabled (true);
    cb_eraseNVM_   ->setEnabled (true);
    //c_2m_box->setEnabled (true);
    //c_4m_box->setEnabled (true);
    openfileBT_    ->setEnabled (true);
    edit_firmware_path_->setEnabled (true);
  }
}

void ST_PM_discardqt_ui::mnu_enable(bool enable,QString baudrate)
{
  for (int i=0;i<menu_baudrate_->actions ().size ();i++)
  {

    if(!enable&&menu_baudrate_->actions ()[i]->text ()==baudrate)
    {
      menu_baudrate_->actions ()[i]->setChecked (true);
      setCurrentBaudrate (baudrates_.indexOf (baudrate));
    }
    else
    {
      menu_baudrate_->actions ()[i]->setEnabled (enable);
    }
  }
}

void ST_PM_discardqt_ui::print(QG::MsgType msgtype, const char *msg, std::size_t size)
{
  Q_EMIT printMsgChanged(msgtype,QString::fromUtf8 (msg,size));
}


Thread_Help::Thread_Help()
{

}

void Thread_Help::run_t5dwn()
{
  qDebug()<<"run_fuc id"<<QThread::currentThreadId ();

  bool result=t5dwn->exec ();
  Q_EMIT resultsChanged(result);

}

void Thread_Help::run_t5upg()
{
  qDebug()<<"run_t5upg id"<<QThread::currentThreadId ();

  bool result=t5upg->exec ();
  Q_EMIT resultsChanged(result);
}

void Thread_Help::stop()
{
  if(t5upg)
  t5upg->stop ();
  if(t5dwn)
  t5dwn->stop ();
}
