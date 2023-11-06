#include "mtk3335_format_ui.h"

MTK3335_Format_UI::MTK3335_Format_UI(QWidget *parent)
    : Base_GroupBox(parent)
{
    this->setTitle (tr("Format Information"));

    setup_ui();
    connect_ui();
    cfg_ui ();
}

MTK3335_Format_UI::MTK3335_Format_UI(const QString &title, QWidget *parent)
    : Base_GroupBox(title,parent)
{
    setup_ui();
    connect_ui();
    cfg_ui ();
}

void MTK3335_Format_UI::setup_ui()
{
    //[1]auto format
    QVBoxLayout  *hboxlayout_=new QVBoxLayout(this);
    radio_auto_format_=new QRadioButton(tr("Auto Format"));
    groupbox_autoformat=new Base_GroupBox;
    hboxlayout_->addWidget (radio_auto_format_);
    hboxlayout_->addWidget (groupbox_autoformat);
    QLabel *autoformat_label=new QLabel(tr("FlashTool automatically formats the flash area"));
    QVBoxLayout *totalvblayout_=new QVBoxLayout;
    groupbox_autoformat->setLayout (totalvblayout_);
    totalvblayout_->addWidget (autoformat_label);

    //[2]Total format
    radio_total_format_=new QRadioButton(tr("Total Format"));
    groupbox_totalformat=new Base_GroupBox;
    hboxlayout_->addWidget (radio_total_format_);
    hboxlayout_->addWidget (groupbox_totalformat);
    QLabel *totalformat_label=new QLabel(tr("FlashTool formats the total flash area"));
    QVBoxLayout *vblayout_=new QVBoxLayout;
    groupbox_totalformat->setLayout (vblayout_);
    vblayout_->addWidget (totalformat_label);

    //[3]Manual format
    radio_manual_format_=new QRadioButton(tr("Manual Format"));
    groupbox_manualformat=new Base_GroupBox;
    hboxlayout_->addWidget (radio_manual_format_);
    hboxlayout_->addWidget (groupbox_manualformat);


    formatgroup_=new QButtonGroup(this);
    formatgroup_->addButton (radio_auto_format_);
    formatgroup_->addButton (radio_total_format_);
    formatgroup_->addButton (radio_manual_format_);

    QGridLayout *manualformat_layout=new QGridLayout(groupbox_manualformat);
    QLabel *manualformatlabel_=new QLabel(tr("Specify the begin address and length of the fotmat area"));
    radio_logical_=new QRadioButton(tr("Logical"));
    radio_physical_=new QRadioButton(tr("Physical"));
    QLabel *beginaddress_name_=new QLabel(tr("Begin Address")+":");
    linedit_beginaddress_=new QLineEdit;
    linedit_length_      =new QLineEdit;
    linedit_beginaddress_->setFixedWidth (160);
    linedit_length_      ->setFixedWidth (160);
    QLabel *length_name_=new QLabel(tr("Length")+":");
    length_name_->setFixedWidth (beginaddress_name_->fontMetrics ().width (beginaddress_name_->text ()));
    manualformat_layout->addWidget (manualformatlabel_,0,0);

    QHBoxLayout *hbox1=new  QHBoxLayout;
    QHBoxLayout *hbox2=new  QHBoxLayout;
    QHBoxLayout *hbox3=new  QHBoxLayout;
    hbox1->addWidget (radio_logical_);
    hbox1->addWidget (radio_physical_);
    hbox1->addStretch ();
    manualformat_layout->addLayout (hbox1,1,0);

    hbox2->addWidget (beginaddress_name_);
    hbox2->addWidget (linedit_beginaddress_);
    hbox2->addStretch ();
    manualformat_layout->addLayout (hbox2,2,0);

    hbox3->addWidget (length_name_);
    hbox3->addWidget (linedit_length_);
    hbox3->addStretch ();
    manualformat_layout->addLayout (hbox3,3,0);

    /*
    radio_manual_format_->setEnabled (false);
    enabel_manualformat_=new QAction;
    addAction (enabel_manualformat_);

    QString shortcut= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::ManualFormatEnableShortcut,"").toString ();
    if(shortcut.isEmpty ())
    {
        shortcut= CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::ManualFormatEnableShortcut,"Ctrl+G");
    }
    shortcut=CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::ManualFormatEnableShortcut,"Ctrl+G").toString ();

    enabel_manualformat_->setShortcut (shortcut);
    */

        qRegisterMetaType<format_Info>();
        qRegisterMetaType<format_Info*>();
}

void MTK3335_Format_UI::cfg_ui()
{
    setCurrentmode(Format_mode::Total_Format);
    radio_physical_->setChecked (true);
    QString regexStr="^(0x|0X)[0-9a-zA-Z]{1,8}$";

    linedit_beginaddress_->setValidator (new QRegExpValidator( QRegExp(regexStr),this));
    linedit_length_->setValidator (new QRegExpValidator( QRegExp(regexStr),this));
}

void MTK3335_Format_UI::connect_ui()
{
    connect (radio_auto_format_,&QRadioButton::pressed,this,[&](){
        setCurrentmode(Format_mode::Auto_Format);
    });
    connect (radio_total_format_,&QRadioButton::pressed,this,[&](){
        setCurrentmode(Format_mode::Total_Format);
    });

    connect (radio_manual_format_,&QRadioButton::pressed,this,[&](){
        radio_manual_format_->setChecked (true);
        groupbox_autoformat->setEnabled (false);
        groupbox_totalformat->setEnabled (false);
        groupbox_manualformat->setEnabled (true);
        setCurrentmode(Format_mode::Manual_Format_Physical);
    });

    connect (radio_logical_,&QRadioButton::pressed,this,[&](){
        setCurrentmode(Format_mode::Manual_Format_Logical);
    });
    connect (radio_physical_,&QRadioButton::pressed,this,[&](){
        setCurrentmode(Format_mode::Manual_Format_Physical);
    });
/*
    connect (enabel_manualformat_,&QAction::triggered,this,[&](bool checked){
        Q_UNUSED (checked)
        if(radio_manual_format_->isEnabled ())
        {
            radio_manual_format_->setEnabled (false);
            radio_total_format_->setChecked (true);
        }
        else
        {
            radio_manual_format_->setEnabled (true);
        }
    });
    */

    connect (radio_logical_,&QRadioButton::pressed,this,[&](){
        linedit_beginaddress_->setPlaceholderText ("Min begin:0x00000000");
        linedit_length_->setPlaceholderText (QString("Max length:0x%1").arg (length (),8,16,QChar('0')));
    });
    connect (radio_physical_,&QRadioButton::pressed,this,[&](){
        linedit_beginaddress_->setPlaceholderText ("Min begin:0x08000000");
        linedit_length_->setPlaceholderText (QString("Max length:0x%1").arg (length (),8,16,QChar('0')));
    });

}

MTK3335_Format_UI::~MTK3335_Format_UI()
{

}

MTK3335_Format_UI::Format_mode MTK3335_Format_UI::currentmode() const
{
    return m_currentmode;
}

void MTK3335_Format_UI::setCurrentmode(Format_mode currentmode)
{
    if(currentmode==Format_mode::Auto_Format)
    {
        radio_auto_format_->setChecked (true);
        groupbox_autoformat->setEnabled (true);
        groupbox_totalformat->setEnabled (false);
        groupbox_manualformat->setEnabled (false);
    }
    else if(currentmode==Format_mode::Total_Format)
    {
        radio_total_format_->setChecked (true);
        groupbox_autoformat->setEnabled (false);
        groupbox_totalformat->setEnabled (true);
        groupbox_manualformat->setEnabled (false);
    }
    else
    {
        radio_manual_format_->setChecked (true);
        groupbox_autoformat->setEnabled (false);
        groupbox_totalformat->setEnabled (false);
        groupbox_manualformat->setEnabled (true);
    }
    m_currentmode = currentmode;
    emit currentmodeChanged(m_currentmode);
}

MTK3335_Format_UI::format_Info* MTK3335_Format_UI::formatInfo()
{
    if(!m_formatInfo)
    {
        m_formatInfo=new format_Info();
    }
    m_formatInfo->beginaddress=linedit_beginaddress_->text ();
    m_formatInfo->length=linedit_length_->text ();
    return m_formatInfo;
}

void MTK3335_Format_UI::setFormatInfo(format_Info* formatInfo)
{
    if (m_formatInfo == formatInfo)
        return;

    m_formatInfo = formatInfo;
    emit formatInfoChanged(m_formatInfo);
}

uint MTK3335_Format_UI::length() const
{
    return m_length;
}

void MTK3335_Format_UI::setLength(uint length)
{
    if(linedit_beginaddress_)
    {
        linedit_beginaddress_->setPlaceholderText ("Min begin:0x08000000");
    }
    if(linedit_length_)
    {
        linedit_length_->setPlaceholderText (QString("Max length:0x%1").arg (length,8,16,QChar('0')));
    }
    m_length = length;
    emit lengthChanged(m_length);
}
