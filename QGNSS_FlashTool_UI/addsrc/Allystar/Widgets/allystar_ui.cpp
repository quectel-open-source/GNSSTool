/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         allystar_ui.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-30      victor.gong
* ***************************************************************************/
#include "allystar_ui.h"

AllyStar_UI::AllyStar_UI(QWidget *parent, Qt::WindowFlags flags)
    :AllyStar_base_window(parent,flags)
{
  downloadbt_=new QToolButton;
  upgradebt_=new QToolButton;
  cfg_parameter();
  cfg_widget();
  model=new AllyStar_Model();
  cfg_model (model);
}

AllyStar_UI::~AllyStar_UI()
{
  model->deleteLater ();
}

void AllyStar_UI::start()
{
  wgt_enable(true);
  statuswgt_->setBt_open(false);
  int  baudrate= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[0]).toInt ();

  int  mode=     downmode_.indexOf (CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]).toString ());
  AllyStarBase ->qGNSS_SetDownLoadBaudRate (baudrate);
  QString dwmd=IODvice::Enum_Value2Key<AllyStar_Upgrade::DownLoadMode>((AllyStar_Upgrade::DownLoadMode)mode);
  statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)});
  if(model->start_download(edit_firmware_path_->text (),qgnss_help_->currentmodel ()->value (),mode))
  {
    statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)+": Success"});
  }
  statuswgt_->setBt_open(true);
  wgt_enable(false);
}
void AllyStar_UI::stop()
{
  AllyStarBase->download_stop ();
}

void AllyStar_UI::ini_IO(IODvice *io)
{
  AllyStarBase->init_IO (io);
}

    void AllyStar_UI::cfg_parameter()
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
              //              "BOOT",
              //              "USER",
              "Download",
              "upgrade",
              };
}

void AllyStar_UI::cfg_widget()
{
  menu_setting_=     menuBar ()->addMenu (tr("Settings"));
  menu_baudrate_= menu_setting_->addMenu (tr("Download Baudrate"));
  //menu_downmode_= menu_setting_->addMenu (tr("Mode"));
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

  upgradebt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
  upgradebt_->setText (tr("upgrade"));
  upgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
  upgradebt_->setStyleSheet (boxstyle);
  upgradebt_->setCheckable (true);

  btgroup->addButton (downloadbt_);
  btgroup->addButton (upgradebt_);
  toolbar_->addWidget (downloadbt_);
  toolbar_->addWidget (upgradebt_);
  addToolBar (toolbar_);
  //[0]menubar
  connect (this,&AllyStar_UI::currentBaudrateChanged,this,[&](int index){
    CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[index]);
  });
  connect (this,&AllyStar_UI::currentDownmodeChanged,this,[&](int index){
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
                                                                      cfg_key::path,edit_firmware_path_->text ()).toString ());
  });
  connect (upgradebt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    upgradebt_->setChecked (true);
    setCurrentDownmode (1);
    showopt_cb(1);
    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::path,edit_firmware_path_->text ()).toString ());

  });
  for (auto &act :baudrates_)
  {
    QAction *action=  menu_baudrate_->addAction (act);
    action->setCheckable (true);
    action->setEnabled (false);
    action->setActionGroup (baudrate_group_);
    if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,460800/*baudrates_[4]*/))
    {
      action->setChecked (true);
      action->setEnabled (true);
      setCurrentBaudrate (baudrates_.indexOf (action->text ()));
    }
  }

  QString  mode= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]).toString ();
  setCurrentDownmode (downmode_.indexOf (mode));
  showopt_cb(downmode_.indexOf (mode));
  if(downmode_.indexOf (mode))
  {
    upgradebt_->click ();
  }
  else
  {
    downloadbt_->click ();
  }

  //[2]mainVlayout_
  mainVlayout_->addLayout (dafileHlayout_);
  label_firmware_name_->setText (tr("Firmware File:"));
  dafileHlayout_->addWidget (label_firmware_name_);
  dafileHlayout_->addWidget (edit_firmware_path_);
  dafileHlayout_->addWidget (openfileBT_);

  connect (openfileBT_,&QPushButton::clicked,this,[&](){
    open_file();
  });
  load_file();
  mainVlayout_->addWidget (statuswgt_);

  connect (statuswgt_,&Status_WGT::bt_clickChanged ,this,[&]()
          {
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
}

bool AllyStar_UI::event(QEvent *event)
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
      return AllyStar_base_window::event (event);
  }
  return AllyStar_base_window::event (event);
}

void AllyStar_UI::wgt_enable(bool Run)
{
  if(Run)
  {
    menubar_    ->setEnabled (false);
    downloadbt_ ->setEnabled (false);
    upgradebt_  ->setEnabled (false);
    openfileBT_ ->setEnabled (false);
    edit_firmware_path_->setEnabled (false);
  }
  else
  {
    menubar_    ->setEnabled (true);
    downloadbt_ ->setEnabled (true);
    upgradebt_  ->setEnabled (true);
    openfileBT_ ->setEnabled (true);
    edit_firmware_path_->setEnabled (true);
  }
}

void AllyStar_UI::open_file()
{
  QString fileName;
  QString filter;
  filter=tr("(*.cyfm)");
  fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select File"), nullptr,filter );
  if(fileName.isEmpty ())
  {
    return;
  }
  switch ((AllyStar_Upgrade::DownLoadMode)currentDownmode ())
  {
    case AllyStar_Upgrade::DownLoadMode::BOOT:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::path,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::path,fileName).toString ());
      break;
    case AllyStar_Upgrade::DownLoadMode::USER:
      CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::path,fileName);
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::path,fileName).toString ());
      break;
  }
}

void AllyStar_UI::load_file()
{
  switch ((AllyStar_Upgrade::DownLoadMode)currentDownmode ())
  {
    case AllyStar_Upgrade::DownLoadMode::BOOT:
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                        cfg_key::path,edit_firmware_path_->text ()).toString ());
      break;
    case AllyStar_Upgrade::DownLoadMode::USER:
      edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                        cfg_key::path,edit_firmware_path_->text ()).toString ());
      break;
  }
}

void AllyStar_UI::showopt_cb(int model)
{

}
