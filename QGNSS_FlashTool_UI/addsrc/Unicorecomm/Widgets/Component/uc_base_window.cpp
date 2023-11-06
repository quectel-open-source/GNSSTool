/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         uc_base_window.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-13      victor.gong
* ***************************************************************************/
#include "uc_base_window.h"

UC_Base_Window::UC_Base_Window(QWidget *parent, Qt::WindowFlags flags)
    :Base_MainWindow (parent,flags)
{
  construct_ui();
}

UC_Base_Window::~UC_Base_Window()
{
#ifdef DEBUG_MSG
  qDebug()<<Q_FUNC_INFO;
#endif
}

void UC_Base_Window::cfg_model(Unicorecomm_Base* Base)
{
  UC_Base=Base;
  //[4] status widget
  qgnss_help_=QGNSS_FLASHTOOL_HELP::get_instance ();

  connect (UC_Base,&Unicorecomm_Base::download_percentageChanged,this,[&](int value){

    statuswgt_->setProgress (value,UC_Base->statekeyvalue ()->key ());
  });

  connect (UC_Base,&Unicorecomm_Base::bbmsgStrChanged,this,[&](const BROM_BASE::BROM_MSG &msgstr){
    switch (msgstr.first) {
      case BROM_BASE::brmsgtype::bb_Success:
      {
        Status_WGT::MSG_str str{Status_WGT::Success,msgstr.second};
        statuswgt_->set_widget (str);
      }
      break;
      case BROM_BASE::brmsgtype::bb_Fail:
      {
        Status_WGT::MSG_str str{Status_WGT::Fail,msgstr.second};
        statuswgt_->set_widget (str);
      }
      break;
      case BROM_BASE::brmsgtype::bb_Error:
      {
        Status_WGT::MSG_str str{Status_WGT::Error,msgstr.second};
        statuswgt_->set_widget (str);
      }
      break;
      case BROM_BASE::brmsgtype::bb_Warning:
      {
        Status_WGT::MSG_str str{Status_WGT::Warning,msgstr.second};
        statuswgt_->set_widget (str);
      }
      break;
      case BROM_BASE::brmsgtype::bb_Info:
      {
        Status_WGT::MSG_str str{Status_WGT::DownloadStep,msgstr.second};
        statuswgt_->set_widget (str);
      }
      break;
    }

  });

  connect (UC_Base,&Unicorecomm_Base::statekeyvalueChanged ,this,[&](){
    Status_WGT::MSG_str str{Status_WGT::DownloadStep,UC_Base->statekeyvalue ()->key ()};
    statuswgt_->set_widget (str);
  });
  connect (UC_Base,&Unicorecomm_Base::flashInfoChanged ,this,[&](){
    downrate_->setText (UC_Base->flashInfo ()["DownloadRate"].toString ());

    portName_->setText (UC_Base->flashInfo ()["PORTNAME"].toString ());
    downbuadrate_->setText (tr("Download Baudrate:")+UC_Base->flashInfo ()["DOWNLOADBAUDRATE"].toString ());
    downmodetxt_->setText (UC_Base->flashInfo ()["DownLoadMode"].toString ());

    downloadtime_->setText (tr("Download Time:")+UC_Base->flashInfo ()["DownLoadTotaltime"].toString ());

    QString flashinfo=  UC_Base->flashInfo ()["TooltipTXT"].toString ();
    statusbar_->setToolTip (flashinfo);
  });
}

void UC_Base_Window::construct_ui()
{
  toolbar_=new QToolBar;
  menubar_=new QMenuBar;
  statusbar_=new QStatusBar;
  baudrate_group_=new QActionGroup(this);
  downmode_group_=new QActionGroup(this);
  content_=new QWidget(this);

  mainVlayout_=new QVBoxLayout(content_);
  dafileHlayout_=new QHBoxLayout();

  label_firmware_name_=new QLabel(content_);
  edit_firmware_path_=new QLineEdit(content_);
  openfileBT_=new QPushButton(content_);
  openfileBT_->setIcon (QIcon(":/assets/images/uEA12-open-file.svg"));
  openfileBT_->setToolTip (tr("Open"));


  statuswgt_=new Status_WGT;
  downrate_=new QLabel;
  portName_=new QLabel;
  downbuadrate_=new QLabel;
  downmodetxt_=new QLabel;
  downloadtime_=new QLabel;


  setMenuWidget (menubar_);
  setStatusBar (statusbar_);
}

/*******************************************property*************************/
int UC_Base_Window::currentDownmode() const
{
  return m_currentDownmode;
}

int UC_Base_Window::currentBaudrate() const
{
  return m_currentBaudrate;
}

void UC_Base_Window::setCurrentDownmode(int currentDownmode)
{
  m_currentDownmode = currentDownmode;
  emit currentDownmodeChanged(m_currentDownmode);
}

void UC_Base_Window::setCurrentBaudrate(int currentBaudrate)
{
  m_currentBaudrate = currentBaudrate;
  emit currentBaudrateChanged(m_currentBaudrate);
}

