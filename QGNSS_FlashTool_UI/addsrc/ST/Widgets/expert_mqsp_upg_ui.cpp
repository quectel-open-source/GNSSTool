#include "expert_mqsp_upg_ui.h"
#include "st_mqsp_upgrade.h"

Expert_MQSP_UPG_UI::Expert_MQSP_UPG_UI(GNSS_FW_INFO *GNSSFWINFO,
                                       QWidget *parent)
: QDialog(parent)
{
  qDebug()<<Q_FUNC_INFO<<GNSSFWINFO;
  setAttribute ( Qt::WA_DeleteOnClose);
  defaultGNSSFWINFO_=new GNSS_FW_INFO;
  memset (defaultGNSSFWINFO_,0,sizeof (GNSS_FW_INFO));
  GNSSFWINFO_=GNSSFWINFO;
  *defaultGNSSFWINFO_=*GNSSFWINFO;
  CreatGNSSUI();
  gnssResert(defaultGNSSFWINFO_);
}

Expert_MQSP_UPG_UI::Expert_MQSP_UPG_UI(APP_FW_INFO *APPFWINFO,
                                       QWidget *parent)
:QDialog(parent)
{
  qDebug()<<Q_FUNC_INFO<<APPFWINFO;
  setAttribute ( Qt::WA_DeleteOnClose);
  defaultAPPFWINFO_=new APP_FW_INFO;
  memset (defaultAPPFWINFO_,0,sizeof (APP_FW_INFO));
  APPFWINFO_=APPFWINFO;
  *defaultAPPFWINFO_=*APPFWINFO;
  CreatAPPUI();
  appResert(defaultAPPFWINFO_);
}

void Expert_MQSP_UPG_UI::CreatGNSSUI()
{
  QRegExp rx("[0-9A-F]*");
  QRegExpValidator *hexregexp=new QRegExpValidator(rx);
  QGridLayout *grid=new QGridLayout;

  firmwareSize_lb=new QLabel("Firmware Size");
  firmwareSize_l=new QLineEdit;firmwareSize_l->setReadOnly (true);
  grid->addWidget (firmwareSize_lb,0,0);
  grid->addWidget (firmwareSize_l,0,1);

  firmwareCRC_lb=new QLabel("Firmware CRC");
  firmwareCRC_l=new QLineEdit;firmwareCRC_l->setReadOnly (true);
  grid->addWidget (firmwareCRC_lb,1,0);
  grid->addWidget (firmwareCRC_l,1,1);

  destaddr_lb=new QLabel("Dest Address");
  destaddr_l=new QLineEdit;
  destaddr_l->setValidator (hexregexp);
  connect (destaddr_l,&QLineEdit::textChanged,this,[=](const QString &str)
          {
           bool sucss=false; int size=str.toUInt (&sucss,16);
           GNSSFWINFO_->GNSS_DestAddr=sucss?size:defaultGNSSFWINFO_->GNSS_DestAddr;
          });
  grid->addWidget (destaddr_lb,2,0);
  grid->addWidget (destaddr_l,2,1);

  EntryPoint_lb=new QLabel("Entry Point");
  EntryPoint_l=new QLineEdit;
  EntryPoint_l->setValidator (hexregexp);
  connect (EntryPoint_l,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            bool sucss=false; int size=str.toUInt (&sucss,16);
            GNSSFWINFO_->GNSS_EntryPoint=sucss?size:defaultGNSSFWINFO_->GNSS_EntryPoint;
          });
  grid->addWidget (EntryPoint_lb,3,0);
  grid->addWidget (EntryPoint_l,3,1);

  NVM_Offset_lb=new QLabel("NVM Offset");
  NVM_Offset_l=new QLineEdit;
  NVM_Offset_l->setValidator (hexregexp);
  connect (NVM_Offset_l,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            bool sucss=false; int size=str.toUInt (&sucss,16);
            GNSSFWINFO_->GNSS_NVM_Offset=sucss?size:defaultGNSSFWINFO_->GNSS_NVM_Offset;
          });
  grid->addWidget (NVM_Offset_lb,4,0);
  grid->addWidget (NVM_Offset_l,4,1);

  NVM_Erase_Size_lb=new QLabel("NVM Erase Size");
  NVM_Erase_Size_l=new QLineEdit;
  NVM_Erase_Size_l->setValidator (hexregexp);
  connect (NVM_Erase_Size_l,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            bool sucss=false; int size=str.toUInt (&sucss,16);
            GNSSFWINFO_->GNSS_NVM_Erase_Size=sucss?size:defaultGNSSFWINFO_->GNSS_NVM_Erase_Size;
          });
  grid->addWidget (NVM_Erase_Size_lb,5,0);
  grid->addWidget (NVM_Erase_Size_l,5,1);

  EraseNVM_lb=new QLabel("Erase NVM");
  EraseNVM_ck=new QCheckBox;
  connect (EraseNVM_ck,&QCheckBox::toggled,this,[=](bool ck)
          {
              GNSSFWINFO_->GNSS_EraseNVM=ck?1:0;
          });
  grid->addWidget (EraseNVM_lb,6,0);
  grid->addWidget (EraseNVM_ck,6,1);

  EraseOnly_lb=new QLabel("Erase Only");
  EraseOnly_ck=new QCheckBox;
  connect (EraseOnly_ck,&QCheckBox::toggled,this,[=](bool ck)
          {
            GNSSFWINFO_->GNSS_EraseOnly=ck?1:0;
          });
  grid->addWidget (EraseOnly_lb,7,0);
  grid->addWidget (EraseOnly_ck,7,1);

  ProgramOnly_lb=new QLabel("Program Only");
  ProgramOnly_ck=new QCheckBox;
  connect (ProgramOnly_ck,&QCheckBox::toggled,this,[=](bool ck)
          {
            GNSSFWINFO_->GNSS_ProgramOnly=ck?1:0;
          });
  grid->addWidget (ProgramOnly_lb,8,0);
  grid->addWidget (ProgramOnly_ck,8,1);

  OK_=new QPushButton("Ok");
  connect (OK_,&QPushButton::clicked,this,[&](bool ck)
          {
            this->close ();
          });
  Reset_=new QPushButton("Reset");
  connect (Reset_,&QPushButton::clicked,this,[&](bool ck)
          {
              gnssResert(defaultGNSSFWINFO_);
          });
  grid->addWidget (OK_,9,0);
  grid->addWidget (Reset_,9,1);
  setLayout (grid);
}

void Expert_MQSP_UPG_UI::CreatAPPUI()
{
  QRegExp rx("[0-9A-F]*");
  QRegExpValidator *hexregexp=new QRegExpValidator(rx);
  QGridLayout *grid=new QGridLayout;
  firmwareSize_lb=new QLabel("Firmware Size");
  firmwareSize_l=new QLineEdit;firmwareSize_l->setReadOnly (true);
  grid->addWidget (firmwareSize_lb,0,0);
  grid->addWidget (firmwareSize_l,0,1);

  firmwareCRC_lb=new QLabel("Firmware CRC");
  firmwareCRC_l=new QLineEdit;firmwareCRC_l->setReadOnly (true);
  grid->addWidget (firmwareCRC_lb,1,0);
  grid->addWidget (firmwareCRC_l,1,1);

  destaddr_lb=new QLabel("Dest Address");
  destaddr_l=new QLineEdit;
  destaddr_l->setValidator (hexregexp);
  connect (destaddr_l,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            bool sucss=false; int size=str.toUInt (&sucss,16);
            APPFWINFO_->APP_DestAddr=sucss?size:defaultAPPFWINFO_->APP_DestAddr;
          });
  grid->addWidget (destaddr_lb,2,0);
  grid->addWidget (destaddr_l,2,1);

  EraseNVM_lb=new QLabel("Erase NVM");
  EraseNVM_ck=new QCheckBox;
  connect (EraseNVM_ck,&QCheckBox::toggled,this,[=](bool ck)
          {
            APPFWINFO_->APP_EraseNVM=ck?1:0;
          });
  grid->addWidget (EraseNVM_lb,3,0);
  grid->addWidget (EraseNVM_ck,3,1);

  OK_=new QPushButton("Ok");
  connect (OK_,&QPushButton::clicked,this,[&](bool ck)
          {
            this->close ();
          });
  Reset_=new QPushButton("Reset");
  connect (Reset_,&QPushButton::clicked,this,[&](bool ck)
          {
            appResert(defaultAPPFWINFO_);
          });
  grid->addWidget (OK_,9,0);
  grid->addWidget (Reset_,9,1);
  setLayout (grid);
}

bool Expert_MQSP_UPG_UI::gnssResert(GNSS_FW_INFO *GNSSFWINFO)
{
  firmwareSize_l->setText (QString::number (GNSSFWINFO->GNSS_FW_Size));
  firmwareCRC_l->setText (QString::number (GNSSFWINFO->GNSS_FW_CRC32,16).toUpper ());
  destaddr_l->setText (QString::number (GNSSFWINFO->GNSS_DestAddr,16).toUpper ());
  EntryPoint_l->setText (QString::number (GNSSFWINFO->GNSS_EntryPoint,16).toUpper ());
  NVM_Offset_l->setText (QString::number (GNSSFWINFO->GNSS_NVM_Offset,16).toUpper ());
  NVM_Erase_Size_l->setText (QString::number (GNSSFWINFO->GNSS_NVM_Erase_Size,16).toUpper ());
  EraseNVM_ck->setChecked (GNSSFWINFO->GNSS_EraseNVM==1?true:false);
  EraseOnly_ck->setChecked (GNSSFWINFO->GNSS_EraseOnly==1?true:false);
  ProgramOnly_ck->setChecked (GNSSFWINFO->GNSS_ProgramOnly==1?true:false);
  return true;
}

bool Expert_MQSP_UPG_UI::appResert(APP_FW_INFO  *APPFWINFO)
{
  firmwareSize_l->setText (QString::number (APPFWINFO->APP_FW_Size));
  firmwareCRC_l->setText (QString::number (APPFWINFO->APP_FW_CRC32,16).toUpper ());
  destaddr_l->setText (QString::number (APPFWINFO->APP_DestAddr,16).toUpper ());
  EraseNVM_ck->setChecked (APPFWINFO->APP_EraseNVM==1?true:false);
  return  true;
}
