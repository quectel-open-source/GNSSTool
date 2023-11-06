#include "mtk3335_readback_cfg_dialog_ui.h"

MTK3335_ReadBack_CFG_Dialog_UI::MTK3335_ReadBack_CFG_Dialog_UI(QWidget *parent,CFG &cfg)
    : QWidget(parent)
{
    cfg_=&cfg;
    INI_UI();
    setWindowTitle (tr("ReadBack File"));
    setWindowFlags (Qt::Dialog);

    setWindowModality (Qt::ApplicationModal);
    qRegisterMetaType<MTK3335_ReadBack_CFG_Dialog_UI::CFG*>();
}

void MTK3335_ReadBack_CFG_Dialog_UI::INI_UI()
{
    QVBoxLayout *vlayout_=new QVBoxLayout(this);
    setLayout (vlayout_);
    QHBoxLayout *path_hlayout_=new QHBoxLayout;
    QGroupBox *address_type_group_=new QGroupBox(tr("Address Type"));
    QGroupBox *Address_group_=new QGroupBox(tr("Address"));

    file_path_=new QLineEdit();
    file_path_->setPlaceholderText (tr("File Path:"));
    QPushButton *push=new QPushButton();//:/assets/images/uEA12-open-file.svg
    push->setIcon (QIcon(":/assets/images/uEA12-open-file.svg"));

    connect (push,&QPushButton::clicked,this,[&](bool checked){
        Q_UNUSED (checked)
        QString path=QFileDialog::getExistingDirectory (this,tr("Save path!"),"");
        file_path_->setText (path);
    });

    path_hlayout_->addWidget (file_path_);
    path_hlayout_->addWidget (push);
    QButtonGroup *group=new QButtonGroup(this);
    logical_box_=new QCheckBox(tr("Logical"));
    physical_box_=new QCheckBox(tr("Physical"));

    group->addButton (logical_box_);
    group->addButton (physical_box_);
    QHBoxLayout *type_Hlayout_=new QHBoxLayout;
    type_Hlayout_->addWidget (logical_box_);
    type_Hlayout_->addWidget (physical_box_);
    physical_box_->setChecked (true);
    address_type_group_->setLayout (type_Hlayout_);

    QGridLayout *gridlayout_=new QGridLayout;
    QLabel *label1=new QLabel(tr("Begin Address"));
    begin_addresstxt_  =new QLineEdit("0x08000000");
    QLabel *label2=new QLabel(tr("ReadBack_Length"));
    readBack_lengthtxt_=new QLineEdit("0x00400000");

    connect (logical_box_,&QCheckBox::clicked,this,[&](bool checked)
            {
                Q_UNUSED (checked)
                begin_addresstxt_->setText("0x00000000");
                readBack_lengthtxt_->setText("0x00400000");
            });
    connect (physical_box_,&QCheckBox::clicked,this,[&](bool checked)
            {
                Q_UNUSED (checked)
                begin_addresstxt_->setText ("0x08000000");
                readBack_lengthtxt_->setText("0x00400000");
            });

    gridlayout_->addWidget (label1,0,0);
    gridlayout_->addWidget (begin_addresstxt_,0,1);
    gridlayout_->setRowStretch (0,2);

    gridlayout_->addWidget (label2,1,0);
    gridlayout_->addWidget (readBack_lengthtxt_,1,1);
    gridlayout_->setRowStretch (1,2);
    Address_group_->setLayout (gridlayout_);

    vlayout_->addLayout (path_hlayout_);
    vlayout_->addWidget (address_type_group_);
    vlayout_->addWidget (Address_group_);


    QPushButton *okbt_    =new QPushButton(tr("OK"));
    QPushButton *cancelbt_=new QPushButton(tr("Cancel"));
    connect (okbt_,&QPushButton::clicked,this,[&](bool checked)
            {
                Q_UNUSED (checked)
                if(file_path_->text ().isEmpty ())
                {
                    return;
                }
                cfg_->FileName=get_Name();
                cfg_->FilePath=file_path_->text ();
                cfg_->ADRType=logical_box_->isChecked ()?AddressType::Logical:cfg_->ADRType=AddressType::Physical;
                cfg_->Length= readBack_lengthtxt_->text ().toUInt ( nullptr,16);
                cfg_->BeginAddress= begin_addresstxt_->text ().toUInt (nullptr,16);
                setCfg_Info(cfg_);
                close ();
            });
    connect (cancelbt_,&QPushButton::clicked,this,[&](bool checked)
            {
                Q_UNUSED (checked)
                close ();
            });
    QHBoxLayout * hboxlayout_=new QHBoxLayout;
    hboxlayout_->addWidget (okbt_);
    hboxlayout_->addWidget (cancelbt_);
    vlayout_->addLayout (hboxlayout_);
}

QString MTK3335_ReadBack_CFG_Dialog_UI::get_Name()
{
    QString fileNME=QString("%1#%2#%3");//module address time
    QString time=QDateTime::currentDateTime ().toString ("hh#mm#ss");
    QString moduleName=QGNSS_FLASHTOOL_HELP::get_instance ()->currentmodel ()->key ();
    QString hexBeginADR=begin_addresstxt_->text ();
    QString endADR     =readBack_lengthtxt_->text ();
    QString Address_=QString("%1-%2")
                           .arg (QString(hexBeginADR))
                           .arg (QString(endADR));

    return fileNME
        .arg (moduleName)
        .arg (Address_)
        .arg (time)
        ;
}

void MTK3335_ReadBack_CFG_Dialog_UI::showEvent(QShowEvent *event)
{
    Q_UNUSED (event)
    switch (cfg_->mode)
    {
    case MTK3335_ReadBack_CFG_Dialog_UI::AddRow:
        break;
    case MTK3335_ReadBack_CFG_Dialog_UI::RemoveRow:
        break;
    case MTK3335_ReadBack_CFG_Dialog_UI::EditRow:
        file_path_->setText ( cfg_->FilePath);
        cfg_->ADRType==AddressType::Logical?logical_box_->setChecked (true):physical_box_->setChecked (true);
        readBack_lengthtxt_->setText ("0x"+QString::number (cfg_->Length,16).rightJustified (8,'0'));
        begin_addresstxt_->setText ("0x"+QString::number (cfg_->BeginAddress,16).rightJustified (8,'0'));
        break;
    }

}

MTK3335_ReadBack_CFG_Dialog_UI::CFG* MTK3335_ReadBack_CFG_Dialog_UI::cfg_Info() const
{
    return m_cfg_Info;
}

void MTK3335_ReadBack_CFG_Dialog_UI::setCfg_Info(CFG* cfg_Info)
{
    m_cfg_Info = cfg_Info;
    emit cfg_InfoChanged(m_cfg_Info);
}
