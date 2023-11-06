#include "expert_upg_ui.h"
#include "st_base.h"

Expert_UPG_UI::Expert_UPG_UI(ImageOptions *fakeOpt,QWidget *parent) : QDialog(parent)
{
  DefaultOpt_=new ImageOptions;
  *DefaultOpt_=*fakeOpt;
  ReferOpt_=fakeOpt;
  setAttribute ( Qt::WA_DeleteOnClose);

  CreatUI();
  Resert(DefaultOpt_);
}

Expert_UPG_UI::~Expert_UPG_UI()
{
  delete DefaultOpt_;
}
QDebug operator<<(QDebug debug,const ImageOptions &opt)
{
  QDebugStateSaver sav(debug);
  debug.nospace ()<<"eraseNVM "<<opt.eraseNVM
                  <<"programOnly "<<opt.programOnly
                  <<"reserved "<<opt.reserved
                  <<"baudRate "<<opt.baudRate
                  <<"firmwareSize "<<opt.firmwareSize
                  <<"firmwareCRC "<<opt.firmwareCRC
                  <<"nvmAddressOffset "<<opt.nvmAddressOffset
                  <<"nvmSize "<<opt.nvmSize;
  return debug;
}
void Expert_UPG_UI::CreatUI()
{
  QRegExp rx("[0-9A-F]*");
  QRegExpValidator *hexregexp=new QRegExpValidator(rx);
  QGridLayout *grid=new QGridLayout;
  QButtonGroup *btgroup=new QButtonGroup;
  eraseNVM_lb=new QLabel("eraseNVM");
  eraseNVM_ck=new QCheckBox;
  connect (eraseNVM_ck,&QCheckBox::toggled,this,[=](bool ck)
          {
            if(ReferOpt_)
            {
              ReferOpt_->eraseNVM=ck?1:0;
            }
          });
  grid->addWidget (eraseNVM_lb,0,0);
  grid->addWidget (eraseNVM_ck,0,1);

  programOnly_lb=new QLabel("programOnly");
  programOnly_ck=new QCheckBox;
  connect (programOnly_ck,&QCheckBox::toggled,this,[=](bool ck)
          {
            if(ReferOpt_)
            {
              ReferOpt_->programOnly=ck?1:0;
            }
          });
  btgroup->addButton (eraseNVM_ck);btgroup->addButton (programOnly_ck);
  grid->addWidget (programOnly_lb,1,0);
  grid->addWidget (programOnly_ck,1,1);

  firmwareSize_lb=new QLabel("firmwareSize");
  firmwareSize_l=new QLineEdit;firmwareSize_l->setReadOnly (true);
  grid->addWidget (firmwareSize_lb,2,0);
  grid->addWidget (firmwareSize_l,2,1);

  firmwareCRC_lb=new QLabel("firmwareCRC");
  firmwareCRC_l=new QLineEdit;firmwareCRC_l->setReadOnly (true);
  grid->addWidget (firmwareCRC_lb,3,0);
  grid->addWidget (firmwareCRC_l,3,1);

  nvmAddressOffset_lb=new QLabel("nvmAddressOffset");
  nvmAddressOffset_l=new QLineEdit;
  nvmAddressOffset_l->setPlaceholderText ("Hex Format!");
  nvmAddressOffset_l->setValidator (hexregexp);
  connect (nvmAddressOffset_l,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            if(ReferOpt_)
            {
              bool sucss=false;
              int size=str.toUInt (&sucss,16);
              if(sucss)
              {
                ReferOpt_->nvmAddressOffset=size;
              }
              else
              {
                ReferOpt_->nvmAddressOffset=DefaultOpt_->nvmAddressOffset;
              }
            }
          });
  grid->addWidget (nvmAddressOffset_lb,4,0);
  grid->addWidget (nvmAddressOffset_l,4,1);

  nvmSize_lb=new QLabel("nvmSize");
  nvmSize_l=new QLineEdit;
  nvmSize_l->setPlaceholderText ("Hex Format!");
  nvmSize_l->setPlaceholderText ("Hex Format!");
  nvmSize_l->setValidator (hexregexp);
  connect (nvmSize_l,&QLineEdit::textChanged,this,[=](const QString &str)
          {
            if(ReferOpt_)
            {
              bool sucss=false;
              int size=str.toUInt (&sucss,16);
              if(sucss)
              {
                ReferOpt_->nvmSize=size;
              }
              else
              {
                ReferOpt_->nvmSize=DefaultOpt_->nvmSize;
              }
            }
          });
  grid->addWidget (nvmSize_lb,5,0);
  grid->addWidget (nvmSize_l,5,1);

  OK_=new QPushButton("Ok");
  connect (OK_,&QPushButton::clicked,this,[&](bool ck)
          {
            this->close ();
          });
  Reset_=new QPushButton("Reset");
  connect (Reset_,&QPushButton::clicked,this,[&](bool ck)
          {
            Resert(DefaultOpt_);
          });
  grid->addWidget (nvmSize_lb,6,0);
  grid->addWidget (nvmSize_l,6,1);

  grid->addWidget (OK_,6,0);
  grid->addWidget (Reset_,6,1);
  setLayout (grid);
}

void Expert_UPG_UI::Resert(ImageOptions *fakeOpt)
{
  eraseNVM_ck->setChecked (fakeOpt->eraseNVM==1);
  programOnly_ck->setChecked (fakeOpt->programOnly==1);
  firmwareSize_l->setText (QString::number (fakeOpt->firmwareSize));
  firmwareCRC_l->setText (QString::number (fakeOpt->firmwareCRC,16).toUpper ());

  nvmAddressOffset_l->setText (QString::number (fakeOpt->nvmAddressOffset,16).toUpper ());
  nvmSize_l->setText (QString::number (fakeOpt->nvmSize,16).toUpper ());
}

