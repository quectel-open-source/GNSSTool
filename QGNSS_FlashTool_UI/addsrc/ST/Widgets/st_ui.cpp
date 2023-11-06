/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_ui.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-08      victor.gong
* ***************************************************************************/
#include "st_ui.h"

#include "expert_upg_ui.h"
#include "expert_download_ui.h"
ST_UI::ST_UI(QWidget *parent, Qt::WindowFlags flags)
    :ST_Base_Window(parent,flags)
{
  downloadbt_=new QToolButton;
  upgradebt_=new QToolButton;

  cfg_parameter();
  cfg_widget();
  STModel=new ST_Model();
  cfg_model (STModel);
  connect (STModel,&ST_Model::showOPTCFG,this,[=](ImageOptions *opt)
          {
            Expert_UPG_UI *upgui=new Expert_UPG_UI(opt,this);
            upgui->exec ();
          });

  connect (STModel,&ST_Model::showDWNOPTCFG ,this,[=](xldr_binimg_info_t *opt)
          {
            Expert_Download_UI *upgui=new Expert_Download_UI(opt,this);
            upgui->exec ();
          });
}

ST_UI::~ST_UI()
{
#ifdef DEBUG_MSG
  qDebug()<<Q_FUNC_INFO;
#endif
  STModel->deleteLater ();
}

void ST_UI::start()
{
  wgt_enable(true);
  statuswgt_->setBt_open(false);
  int  baudrate= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[0]).toInt ();
  ImageOptions opt;
  opt.eraseNVM=cb_eraseNVM_->isChecked ()?1:0;
  stBase->set_Recovery (cb_Recovery_->isChecked ());
  opt.baudRate= currentBaudrate();
  //if(c_2m_box->isChecked ())
  switch (QGNSS_FLASHTOOL_HELP::get_instance ()->model ())
  {
    case quectel_gnss::LG69TAA:
    case quectel_gnss::LG69TAD:
    case quectel_gnss::LG69TAI:
    case quectel_gnss::LG69TAJ:
    case quectel_gnss::LG69TAK:
      opt.nvmAddressOffset=0x000FF000;
      opt.nvmSize=1028*1024;
      break;

    case quectel_gnss::LG69TAH:
    case quectel_gnss::LC29TAA:
    case quectel_gnss::LC99TIA:
      {
        opt.nvmAddressOffset=0x001FF000;
        opt.nvmSize=1028*1024;
      }
//    break;
//      default:
//        opt.nvmAddressOffset=0x000FF000;
//        opt.nvmSize=1028*1024;
//        break;
  }


  int  mode=     downmode_.indexOf (CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]).toString ());
  stBase->qGNSS_SetDownLoadBaudRate (baudrate);
  QString dwmd=IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)mode);
  statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)});
  if(STModel->start_download(edit_firmware_path_->text (),qgnss_help_->currentmodel ()->value (),mode,opt,downloadOpt_ac_->isChecked ()))
  {
    statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)+": Success"});
  }
  statuswgt_->setBt_open(true);
  wgt_enable(false);
}

void ST_UI::stop()
{
  stBase->download_stop ();
}

void ST_UI::ini_IO(IODvice *io)
{
  stBase->init_IO (io);
}

    void ST_UI::cfg_parameter()
{
  baudrates_=
      {
          "57600",
          "115200",
          "230400",
          //"460800",
         // "921600",
          };

      downmode_=
          {
              "Download",
              "upgrade",
              };

}

void ST_UI::cfg_widget()
{
  menu_setting_=     menuBar ()->addMenu (tr("Settings"));
  menu_baudrate_= menu_setting_->addMenu (tr("Download Baudrate"));
  //menu_downmode_= menu_setting_->addMenu (tr("Mode"));
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
  downloadbt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
  downloadbt_->setText (tr("Download"));
  downloadbt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  downloadbt_->setStyleSheet (boxstyle);
  downloadbt_->setCheckable (true);

  // upgradebt_->setVisible (true);
  upgradebt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
  upgradebt_->setText (tr("Upgrade"));
  upgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  upgradebt_->setStyleSheet (boxstyle);
  upgradebt_->setCheckable (true);

  btgroup->addButton (downloadbt_);
  btgroup->addButton (upgradebt_);
  toolbar_->addWidget (downloadbt_);
  toolbar_->addWidget (upgradebt_);
  addToolBar (toolbar_);
  //[0]menubar
  connect (this,&ST_UI::currentBaudrateChanged,this,[&](int index){
    CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[index]);
  });
  connect (this,&ST_UI::currentDownmodeChanged,this,[&](int index){
    CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[index]);
  });
  connect (menu_baudrate_,&QMenu::triggered,this,[&](QAction *action){
    setCurrentBaudrate (baudrates_.indexOf (action->text ()));
  });
  connect (downloadbt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    downloadbt_->setChecked (true);
    setCurrentDownmode (0);
    showopt_cb(0);
    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::Downloadpath,edit_firmware_path_->text ()).toString ());
    uint filesize=0;
    QString crc_txt="";
    GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
    filesize_txt->setText (QString::number (filesize));
    filecrc_txt->setText (crc_txt);
  });
  connect (upgradebt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    upgradebt_->setChecked (true);
    setCurrentDownmode (1);
    showopt_cb(1);
    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::Upgradepath,edit_firmware_path_->text ()).toString ());
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
    if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[2]))
    {
      action->setChecked (true);
      setCurrentBaudrate (baudrates_.indexOf (action->text ()));
    }
  }

  QString  mode= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]).toString ();
  setCurrentDownmode (downmode_.indexOf (mode));
  showopt_cb(downmode_.indexOf (mode));
  if(downmode_.indexOf (mode))
  {
    upgradebt_->click ();
  }else
  {
    downloadbt_->click ();
  }

  //[2]mainVlayout_
  mainVlayout_->addLayout (dafileHlayout_);
  mainVlayout_->addLayout (optHlayout_);
  mainVlayout_->addLayout (crclayout_);
  label_firmware_name_->setText (tr("Firmware File:"));
  dafileHlayout_->addWidget (label_firmware_name_);
  dafileHlayout_->addWidget (edit_firmware_path_);
  dafileHlayout_->addWidget (openfileBT_);

  firmware_opt_->setText (tr("Firmware Options:"));
  cb_eraseNVM_->setText (tr("EraseNVM"));
  cb_Recovery_->setText (tr("Recovery"));

  //formatsizegrp->addButton (c_2m_box);
  //formatsizegrp->addButton (c_4m_box);

  //cb_eraseLTO_->setText (tr("EraseLTO"));
  optHlayout_->addWidget (firmware_opt_);
  optHlayout_->addWidget (cb_eraseNVM_);
  optHlayout_->addWidget (cb_Recovery_);
  //optHlayout_->addWidget (c_2m_box);
  //optHlayout_->addWidget (c_4m_box);
  //optHlayout_->addWidget (cb_eraseLTO_);
  optHlayout_->addStretch ();

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

  downrate_->setMinimumWidth (250);
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

#ifdef TEST_ENVIR
  typedef BROM_BASE::QGNSS_Model quectel_gnss;
#endif
  if( QGNSS_FLASHTOOL_HELP::get_instance ()->model ()==quectel_gnss::LG69TAB)
  {
    upgradebt_->setEnabled (false);
    upgradebt_->setVisible (false);
  }
}

bool ST_UI::event(QEvent *event)
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

void ST_UI::wgt_enable(bool Run)
{
  if(Run)
  {
    menubar_    ->setEnabled (false);
    downloadbt_ ->setEnabled (false);
    upgradebt_  ->setEnabled (false);
    cb_eraseNVM_->setEnabled (false);
    cb_Recovery_->setEnabled (false);
    //c_2m_box->setEnabled (false);
    //c_4m_box->setEnabled (false);
    openfileBT_ ->setEnabled (false);

    edit_firmware_path_->setEnabled (false);
  }
  else
  {
    menubar_    ->setEnabled (true);
    downloadbt_ ->setEnabled (true);
    if( QGNSS_FLASHTOOL_HELP::get_instance ()->model ()==quectel_gnss::LG69TAB)
    {
      upgradebt_->setEnabled (false);
      upgradebt_->setVisible (false);
    }
    else
    {
      upgradebt_  ->setEnabled (true);
    }
    cb_eraseNVM_->setEnabled (false);
    cb_eraseNVM_->setVisible (false);
    cb_Recovery_->setEnabled (true);
    openfileBT_ ->setEnabled (true);
    edit_firmware_path_->setEnabled (true);
    //c_2m_box->setEnabled (true);
    //c_4m_box->setEnabled (true);
  }
}

void ST_UI::open_file()
{
  QString fileName;
  QString filter;
  filter=tr("(*.bin )");
  fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select File"), nullptr,filter );
  if(fileName.isEmpty ())
  {
    return;
  }
  switch ((ST_Upgrade::ST_downmode)currentDownmode ())
  {
    case ST_Upgrade::ST_downmode::download:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downloadpath,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downloadpath,fileName).toString ());
      break;
    case ST_Upgrade::ST_downmode::upgrade:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Upgradepath,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Upgradepath,fileName).toString ());
      break;

  }
  uint filesize=0;
  QString crc_txt="";
  GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
  filesize_txt->setText (QString::number (filesize));
  filecrc_txt->setText (crc_txt);
}

void ST_UI::load_file()
{
  switch ((ST_Upgrade::ST_downmode)currentDownmode ())
  {
    case ST_Upgrade::ST_downmode::download:
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                        cfg_key::Downloadpath,edit_firmware_path_->text ()).toString ());
      break;
    case ST_Upgrade::ST_downmode::upgrade:
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                        cfg_key::Upgradepath,edit_firmware_path_->text ()).toString ());
      break;
  }
  uint filesize=0;
  QString crc_txt="";
  GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
  filesize_txt->setText (QString::number (filesize));
  filecrc_txt->setText (crc_txt);
}

void ST_UI::showopt_cb(int model)
{
  if(model==0)
  {
    cb_Recovery_->setVisible (false);
    cb_eraseNVM_->setVisible (false);
    cb_eraseNVM_->setChecked (true);
    firmware_opt_->setEnabled (false);
    firmware_opt_->setVisible (false);
    //c_2m_box->setVisible (false);
    //c_4m_box->setVisible (false);
  }
  else
  {
    cb_Recovery_->setVisible (true);
    cb_Recovery_->setChecked (true);
    cb_eraseNVM_->setVisible (false);
    cb_eraseNVM_->setChecked (true);
    firmware_opt_->setVisible (true);

    //c_2m_box->setVisible (true);
    //c_4m_box->setVisible (true);
    //c_2m_box->setChecked (true);
  }
  //firmware_opt_->setVisible (true);
}
