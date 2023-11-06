/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         uc_ui.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-13      victor.gong
* ***************************************************************************/
#include "uc_ui.h"


UC_UI::UC_UI(QWidget *parent, Qt::WindowFlags flags)
    :UC_Base_Window (parent,flags)
{
  downloadbt_=new QToolButton;
  cfg_parameter();
  cfg_widget();
  UCModel=new UC_Model();
  cfg_model (UCModel);
}

UC_UI::~UC_UI()
{
#ifdef DEBUG_MSG
  qDebug()<<Q_FUNC_INFO;
#endif
  UCModel->deleteLater ();
}

void UC_UI::start()
{
  wgt_enable(true);
  statuswgt_->setBt_open(false);
  int  baudrate= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[0]).toInt ();
  UC_Base->qGNSS_SetDownLoadBaudRate (baudrate);

  statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg ("upgrade")});
  if(UCModel->start_download(edit_firmware_path_->text (),qgnss_help_->currentmodel ()->value ()))
  {
    statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg ("upgrade")+": Success"});
  }
  statuswgt_->setBt_open(true);
  wgt_enable(false);
}

void UC_UI::stop()
{
  UC_Base->download_stop ();
}

void UC_UI::ini_IO(IODvice *io)
{
  UC_Base->init_IO (io);
}

void UC_UI::cfg_parameter()
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
              "Download",
              "upgrade",
              };

}

void UC_UI::cfg_widget()
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

  btgroup->addButton (downloadbt_);
  toolbar_->addWidget (downloadbt_);
  addToolBar (toolbar_);
  //[0]menubar
  connect (this,&UC_UI::currentBaudrateChanged,this,[&](int index){
    CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[index]);
  });

  connect (menu_baudrate_,&QMenu::triggered,this,[&](QAction *action){
    setCurrentBaudrate (baudrates_.indexOf (action->text ()));
  });
  connect (downloadbt_,&QToolButton::clicked,this,[&](bool checked){
    Q_UNUSED (checked)
    downloadbt_->setChecked (true);
    setCurrentDownmode (0);

    edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                      cfg_key::Downloadpath,edit_firmware_path_->text ()).toString ());
  });

  for (auto &act :baudrates_)
  {
//    QAction *action=  menu_baudrate_->addAction (act);
//    action->setCheckable (true);
//    action->setActionGroup (baudrate_group_);
//    if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[4]))
//    {
//      action->setChecked (true);
//      setCurrentBaudrate (baudrates_.indexOf (action->text ()));
//    }
    QAction *action=  menu_baudrate_->addAction (act);
    action->setCheckable (true);
    action->setEnabled (false);
    action->setActionGroup (baudrate_group_);
    if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,115200/*baudrates_[4]*/))
    {
      action->setChecked (true);
      action->setEnabled (true);
      setCurrentBaudrate (baudrates_.indexOf (action->text ()));
    }
  }

  QString  mode= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]).toString ();
  setCurrentDownmode (downmode_.indexOf (mode));

  if(downmode_.indexOf (mode))
  {

  }else
  {
    downloadbt_->click ();
  }

  //[2]mainVlayout_
  mainVlayout_->addLayout (dafileHlayout_);

  label_firmware_name_->setText (tr("Firmware File:"));
  dafileHlayout_->addWidget (label_firmware_name_);
  dafileHlayout_->addWidget (edit_firmware_path_);
  dafileHlayout_->addWidget (openfileBT_);
  connect (openfileBT_,&QPushButton::clicked,this,[&]()
  {
    open_file();
  });

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

}

bool UC_UI::event(QEvent *event)
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
      return UC_Base_Window::event (event);
  }
  return UC_Base_Window::event (event);
}

void UC_UI::wgt_enable(bool Run)
{
  if(Run)
  {
    menubar_    ->setEnabled (false);
    downloadbt_ ->setEnabled (false);
    openfileBT_ ->setEnabled (false);
    edit_firmware_path_->setEnabled (false);
  }
  else
  {
    menubar_    ->setEnabled (true);
    downloadbt_ ->setEnabled (true);
    openfileBT_ ->setEnabled (true);
    edit_firmware_path_->setEnabled (true);
  }
}

void UC_UI::open_file()
{
  QString fileName;
  QString filter;
  filter=tr("(*.pkg)");
  fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select File"), nullptr,filter );
  if(fileName.isEmpty ())
  {
    return;
  }
  CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downloadpath,fileName);
  edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downloadpath,fileName).toString ());
}
