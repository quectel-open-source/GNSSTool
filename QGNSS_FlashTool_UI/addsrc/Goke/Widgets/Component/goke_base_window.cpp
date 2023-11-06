#include "goke_base_window.h"


Goke_base_window::Goke_base_window(QWidget *parent, Qt::WindowFlags flags)
    :Base_MainWindow (parent,flags)
{
  construct_ui();
}

Goke_base_window::~Goke_base_window()
{
}

void Goke_base_window::construct_ui()
{
  toolbar_=new QToolBar;
  menubar_=new QMenuBar;
  statusbar_=new QStatusBar;
  baudrate_group_=new QActionGroup(this);
 // downmode_group_=new QActionGroup(this);
  content_=new QWidget(this);

  mainVlayout_=new QVBoxLayout(content_);

  groupbox_=new QGroupBox(content_);
  groupVlayout_=new QVBoxLayout(groupbox_);
  rom_table_    =new MTK_TableWidget(groupbox_);

  statuswgt_=new Status_WGT;
  romlb_=new QPushButton(tr("Flash Img"));
  beginitem=new QTableWidgetItem;
  enditem=new QTableWidgetItem;
  loaclitem=new QTableWidgetItem;

  downrate_=new QLabel;
  portName_=new QLabel;
  downbuadrate_=new QLabel;
  downmodetxt_=new QLabel;
  downloadtime_=new QLabel;


  setMenuWidget (menubar_);
  setStatusBar (statusbar_);
}

void Goke_base_window::cfg_model(Goke_Base *Base)
{
  //[4] status widget
  base=Base;
  qgnss_help_=QGNSS_FLASHTOOL_HELP::get_instance ();

  connect (base,&MTK_Base::download_percentageChanged,this,[&](int value){

    statuswgt_->setProgress (value,base->statekeyvalue ()->key ());
  });
  connect (base,&MTK_Base::erroStringChanged,this,[&](){
    Status_WGT::MSG_str str{Status_WGT::Error,base->erroString ()};
    statuswgt_->set_widget (str);
  });
  connect (base,&MTK_Base::bbmsgStrChanged,this,[&](const BROM_BASE::BROM_MSG &msgstr){
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
  connect (base,&Goke_Base::statekeyvalueChanged ,this,[&](){
    Status_WGT::MSG_str str{Status_WGT::DownloadStep,base->statekeyvalue ()->key ()};
    statuswgt_->set_widget (str);
  });
  connect (base,&Goke_Base::flashInfoChanged ,this,[&](){


    downrate_->setText (base->flashInfo ()["DownloadRate"].toString ());

    portName_->setText (base->flashInfo ()["PORTNAME"].toString ());
    downbuadrate_->setText (tr("Download Baudrate:")+base->flashInfo ()["DOWNLOADBAUDRATE"].toString ());
    downmodetxt_->setText (base->flashInfo ()["DownLoadMode"].toString ());

    downloadtime_->setText (tr("Download Time:")+base->flashInfo ()["DownLoadTotaltime"].toString ());

    QString flashinfo=  base->flashInfo ()["TooltipTXT"].toString ();
    statusbar_->setToolTip (flashinfo);
  });
}

void Goke_base_window::cfg_parameter()
{

      baudrates_=
          {
              "921600",
              "460800",
              "230400",
              "115200",
              "57600",
              "38400",
              "19200",
              "14400",
              "9600",
              };
  downmode_=
      {
          "Download",
          "Total format before download",
          "Region format before download",
          };
  tabelheaderlabels_={
      "Name",
      "Begin Address",
      "End Address",
      "Location"
  };

}

/*******************************************property********************/
int Goke_base_window::currentDownmode() const
{
  return m_currentDownmode;
}

int Goke_base_window::currentBaudrate() const
{
  return m_currentBaudrate;
}

void Goke_base_window::setCurrentDownmode(int currentDownmode)
{

  m_currentDownmode = currentDownmode;
  emit currentDownmodeChanged(m_currentDownmode);
}

void Goke_base_window::setCurrentBaudrate(int currentBaudrate)
{

  m_currentBaudrate = currentBaudrate;
  emit currentBaudrateChanged(m_currentBaudrate);
}

