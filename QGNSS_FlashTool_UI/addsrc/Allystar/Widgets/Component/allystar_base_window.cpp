#include "allystar_base_window.h"


AllyStar_base_window::AllyStar_base_window(QWidget *parent, Qt::WindowFlags flags)
    :Base_MainWindow (parent,flags)
{
  construct_ui();
}

AllyStar_base_window::~AllyStar_base_window()
{
#ifdef DEBUG_MSG
  qDebug()<<Q_FUNC_INFO;
#endif
}
void AllyStar_base_window::cfg_model(AllyStar_Base* Base)
{
  AllyStarBase=Base;

  //[4] status widget
  qgnss_help_=QGNSS_FLASHTOOL_HELP::get_instance ();

  connect (AllyStarBase,&AllyStar_Base::download_percentageChanged,this,[&](int value){

    statuswgt_->setProgress (value,AllyStarBase->statekeyvalue ()->key ());
  });
  connect (AllyStarBase,&AllyStar_Base::erroStringChanged,this,[&](){

     Status_WGT::MSG_str str{Status_WGT::Error,AllyStarBase->erroString ()};
     statuswgt_->set_widget (str);
  });
  connect (AllyStarBase,&AllyStar_Base::bbmsgStrChanged,this,[&](const BROM_BASE::BROM_MSG &msgstr){
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

  connect (AllyStarBase,&AllyStar_Base::statekeyvalueChanged ,this,[&](){
    Status_WGT::MSG_str str{Status_WGT::DownloadStep,AllyStarBase->statekeyvalue ()->key ()};
    statuswgt_->set_widget (str);
  });
  connect (AllyStarBase,&AllyStar_Base::flashInfoChanged ,this,[&](){
    downrate_->setText (AllyStarBase->flashInfo ()["DownloadRate"].toString ());

    portName_->setText (AllyStarBase->flashInfo ()["PORTNAME"].toString ());
    downbuadrate_->setText (tr("Download Baudrate:")+AllyStarBase->flashInfo ()["DOWNLOADBAUDRATE"].toString ());
    downmodetxt_->setText (AllyStarBase->flashInfo ()["DownLoadMode"].toString ());

    downloadtime_->setText (tr("Download Time:")+AllyStarBase->flashInfo ()["DownLoadTotaltime"].toString ());

    QString flashinfo=  AllyStarBase->flashInfo ()["TooltipTXT"].toString ();
    statusbar_->setToolTip (flashinfo);
  });

  connect(statuswgt_,&Status_WGT::bt_openChanged,this,[&](bool opened){

  });

}

void AllyStar_base_window::construct_ui()
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
int AllyStar_base_window::currentDownmode() const
{
  return m_currentDownmode;
}

int AllyStar_base_window::currentBaudrate() const
{
  return m_currentBaudrate;
}

void AllyStar_base_window::setCurrentDownmode(int currentDownmode)
{
  m_currentDownmode = currentDownmode;
  emit currentDownmodeChanged(m_currentDownmode);
}

void AllyStar_base_window::setCurrentBaudrate(int currentBaudrate)
{
  m_currentBaudrate = currentBaudrate;
  emit currentBaudrateChanged(m_currentBaudrate);
}
