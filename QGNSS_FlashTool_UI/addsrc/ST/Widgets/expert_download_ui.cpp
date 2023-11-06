#include "expert_download_ui.h"
#include "st_upgrade.h"

Expert_Download_UI::Expert_Download_UI(xldr_binimg_info_t *xldr,QWidget *parent)
    : QDialog(parent)
{
  defaultxldr_=new xldr_binimg_info_t;
  *defaultxldr_=*xldr;
  ptrxldr_=xldr;
  setAttribute ( Qt::WA_DeleteOnClose);

  CreatUI();
  Resert(defaultxldr_);
}

Expert_Download_UI::~Expert_Download_UI()
{
  delete defaultxldr_;
}
//int  fileSize;
//uint32_t  bootMode; //OUTPUT_SRAM OUTPUT_SQI OUTPUT_NOR OUTPUT_ITCM
//uint32_t  crc32;
//int  destinationAddress;
//int  entryPoint;
//uint8_t   eraseNVM;//eraseOption
//uint8_t   eraseOnly;
//uint8_t   programOnly;
//uint8_t   subSector;
//uint8_t   sta8090fg;
//uint8_t   res1;
//uint8_t   res2;
//uint8_t   res3;
//int       NVMOffset;
//int       NVMSize;
//uint32_t  debug;
//uint32_t  debugAction;
//uint32_t  debugAddress;
//int  debugSize;
//uint32_t  debugData;
void Expert_Download_UI::CreatUI()
{
  QRegExp rx("[0-9A-F]*");
  QRegExpValidator *hexregexp=new QRegExpValidator(rx);
  QGridLayout *grid=new QGridLayout;

  QLabel *fileSize_lb=new QLabel("fileSize");
  fileSize_edt=new QLineEdit;
  fileSize_edt->setReadOnly (true);
  int row=0;
  grid->addWidget (fileSize_lb,row,0);
  grid->addWidget (fileSize_edt,row,1);

  QLabel *bootMode_lb=new QLabel("bootMode");
  bootMode_cbx=new QComboBox;
  bootMode_cbx->addItems ({"SRAM","SQI","NOR","ITCM"});
  connect (bootMode_cbx,QOverload<int>().of(&QComboBox::currentIndexChanged),this,[=](int idx)
          {
            ptrxldr_->bootMode=idx;
          });
  grid->addWidget (bootMode_lb,++row,0);
  grid->addWidget (bootMode_cbx,row,1);

  QLabel *crc32_lb=new QLabel("crc32");
  crc32_edt=new QLineEdit;
  crc32_edt->setReadOnly (true);
  grid->addWidget (crc32_lb,++row,0);
  grid->addWidget (crc32_edt,row,1);

  QLabel *destinationAddress_lb=new QLabel("destinationAddress");
  destinationAddress_edt=new QLineEdit;
  destinationAddress_edt->setValidator (hexregexp);
  connect (destinationAddress_edt,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            ptrxldr_->destinationAddress=str.toUInt (nullptr,16);
          });
  grid->addWidget (destinationAddress_lb,++row,0);
  grid->addWidget (destinationAddress_edt,row,1);

  QLabel *entryPoint_lb=new QLabel("entryPoint");
  entryPoint_edt=new QLineEdit;
  entryPoint_edt->setValidator (hexregexp);
  connect (entryPoint_edt,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            ptrxldr_->entryPoint=str.toUInt (nullptr,16);
          });
  grid->addWidget (entryPoint_lb,++row,0);
  grid->addWidget (entryPoint_edt,row,1);

  QLabel *eraseOption_lb=new QLabel("eraseOption");
  eraseOption_cbx=new QComboBox;
  eraseOption_cbx->addItems ({"ERASE_DISABLE","ERASE_AREA","ERASE_CHIPSET","ERASE_ALL"});
  connect (eraseOption_cbx,QOverload<int>().of(&QComboBox::currentIndexChanged),this,[=](int idx)
          {
            ptrxldr_->eraseNVM=idx;
          });
  grid->addWidget (eraseOption_lb,++row,0);
  grid->addWidget (eraseOption_cbx,row,1);

  QLabel *eraseOnly_lb=new QLabel("eraseOnly");
  eraseOnly_chk=new QCheckBox;
  connect (eraseOnly_chk,&QCheckBox::toggled ,this,[=](bool chk)
          {
            ptrxldr_->eraseOnly=chk;
          });
  grid->addWidget (eraseOnly_lb,++row,0);
  grid->addWidget (eraseOnly_chk,row,1);

  QLabel *programOnly_lb=new QLabel("programOnly");
  programOnly_chk=new QCheckBox;
  connect (programOnly_chk,&QCheckBox::toggled ,this,[=](bool chk)
          {
            ptrxldr_->programOnly=chk;
          });
  grid->addWidget (programOnly_lb,++row,0);
  grid->addWidget (programOnly_chk,row,1);

  QLabel *subSector_lb=new QLabel("subSector");
  subSector_chk=new QCheckBox;
  connect (subSector_chk,&QCheckBox::toggled ,this,[=](bool chk)
          {
            ptrxldr_->subSector=chk;
          });
  grid->addWidget (subSector_lb,++row,0);
  grid->addWidget (subSector_chk,row,1);

  QLabel *sta8090fg_lb=new QLabel("sta8090fg");
  sta8090fg_chk=new QCheckBox;
  connect (sta8090fg_chk,&QCheckBox::toggled ,this,[=](bool chk)
          {
            ptrxldr_->sta8090fg=chk;
          });
  grid->addWidget (sta8090fg_lb,++row,0);
  grid->addWidget (sta8090fg_chk,row,1);

  QLabel *NVMOffset_lb=new QLabel("NVMOffset");
  NVMOffset_edt=new QLineEdit;
  NVMOffset_edt->setValidator (hexregexp);
  connect (NVMOffset_edt,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            ptrxldr_->NVMOffset=str.toUInt (nullptr,16);
          });
  grid->addWidget (NVMOffset_lb,++row,0);
  grid->addWidget (NVMOffset_edt,row,1);

  QLabel *NVMSize_lb=new QLabel("NVMSize");
  NVMSize_edt=new QLineEdit;
  NVMSize_edt->setValidator (hexregexp);
  connect (NVMSize_edt,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            ptrxldr_->NVMSize=str.toUInt (nullptr,16);
          });
  grid->addWidget (NVMSize_lb,++row,0);
  grid->addWidget (NVMSize_edt,row,1);

  OK_=new QPushButton("Ok");
  connect (OK_,&QPushButton::clicked,this,[&](bool ck)
          {
            this->close ();
          });
  Reset_=new QPushButton("Reset");
  connect (Reset_,&QPushButton::clicked,this,[&](bool ck)
          {
            Resert(defaultxldr_);
          });
  grid->addWidget (OK_,++row,0);
  grid->addWidget (Reset_,row,1);
  setLayout (grid);
}

void Expert_Download_UI::Resert(xldr_binimg_info_t *Opt)
{
  fileSize_edt->setText (QString::number (Opt->fileSize));
  bootMode_cbx->setCurrentIndex (Opt->bootMode);
  crc32_edt->setText (QString::number (Opt->crc32,16).toUpper ());
  destinationAddress_edt->setText (QString::number (Opt->destinationAddress,16).toUpper ());
  entryPoint_edt->setText (QString::number (Opt->entryPoint,16).toUpper ());
  eraseOption_cbx->setCurrentIndex (Opt->eraseNVM);
  eraseOnly_chk->setChecked (Opt->eraseOnly);
  programOnly_chk->setChecked (Opt->programOnly);
  subSector_chk->setChecked (Opt->subSector);
  sta8090fg_chk->setChecked (Opt->sta8090fg);
  NVMOffset_edt->setText (QString::number (Opt->NVMOffset,16).toUpper ());
  NVMSize_edt->setText (QString::number (Opt->NVMSize,16).toUpper ());
}
