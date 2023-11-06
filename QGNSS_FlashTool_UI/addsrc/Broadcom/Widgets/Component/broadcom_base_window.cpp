/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_base_window.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-25      victor.gong
* ***************************************************************************/

#include "broadcom_base_window.h"
Broadcom_Base_Window::Broadcom_Base_Window(QWidget *parent, Qt::WindowFlags flags)
    :Base_MainWindow (parent,flags)
{
    construct_ui();
}

Broadcom_Base_Window::~Broadcom_Base_Window()
{
#ifdef DEBUG_MSG
    qDebug()<<Q_FUNC_INFO;
#endif
}

void Broadcom_Base_Window::cfg_model(Broadcom_Base* Base)
{
    broadcombase_=Base;
    //[4] status widget
    qgnss_help_=QGNSS_FLASHTOOL_HELP::get_instance ();

    connect (broadcombase_,&Broadcom_Base::download_percentageChanged,this,[&](int value){

        statuswgt_->setProgress (value,broadcombase_->statekeyvalue ()->key ());
    });
    connect (broadcombase_,&Broadcom_Base::erroStringChanged,this,[&](){
        // qDebug()<<broadcombase_->erroString ();
        // Status_WGT::MSG_str str{Status_WGT::Error,broadcombase_->erroString ()};
        // statuswgt_->set_widget (str);
    });
    connect (broadcombase_,&Broadcom_Base::bbmsgStrChanged,this,[&](const BROM_BASE::BROM_MSG &msgstr){
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

    connect (broadcombase_,&Broadcom_Base::statekeyvalueChanged ,this,[&](){
        Status_WGT::MSG_str str{Status_WGT::DownloadStep,broadcombase_->statekeyvalue ()->key ()};
        statuswgt_->set_widget (str);
    });
    connect (broadcombase_,&Broadcom_Base::flashInfoChanged ,this,[&](){
        downrate_->setText (broadcombase_->flashInfo ()["DownloadRate"].toString ());

        portName_->setText (broadcombase_->flashInfo ()["PORTNAME"].toString ());
        downbuadrate_->setText (tr("Download Baudrate:")+broadcombase_->flashInfo ()["DOWNLOADBAUDRATE"].toString ());
        downmodetxt_->setText (broadcombase_->flashInfo ()["DownLoadMode"].toString ());

        downloadtime_->setText (tr("Download Time:")+broadcombase_->flashInfo ()["DownLoadTotaltime"].toString ());

        QString flashinfo=  broadcombase_->flashInfo ()["TooltipTXT"].toString ();
        statusbar_->setToolTip (flashinfo);
    });

    connect(statuswgt_,&Status_WGT::bt_openChanged,this,[&](bool opened){

    });
}

void Broadcom_Base_Window::construct_ui()
{
    toolbar_=new QToolBar;
    menubar_=new QMenuBar;
    statusbar_=new QStatusBar;
    baudrate_group_=new QActionGroup(this);
    downmode_group_=new QActionGroup(this);
    content_=new QWidget(this);

    mainVlayout_=new QVBoxLayout(content_);
    dafileHlayout_=new QHBoxLayout();
    optHlayout_=new QHBoxLayout();
    label_firmware_name_=new QLabel(content_);
    edit_firmware_path_=new QLineEdit(content_);
    openfileBT_=new QPushButton(content_);
    openfileBT_->setIcon (QIcon(":/assets/images/uEA12-open-file.svg"));
    openfileBT_->setToolTip (tr("Open"));

    firmware_opt_=new QLabel;
    cb_eraseNVM_=new QCheckBox;
    cb_eraseLTO_=new QCheckBox;


    statuswgt_=new Status_WGT;
    downrate_=new QLabel;
    portName_=new QLabel;
    downbuadrate_=new QLabel;
    downmodetxt_=new QLabel;
    downloadtime_=new QLabel;

    setMenuWidget (menubar_);
    setStatusBar (statusbar_);
}

/*******************************************property********************/
int Broadcom_Base_Window::currentDownmode() const
{
    return m_currentDownmode;
}

int Broadcom_Base_Window::currentBaudrate() const
{
    return m_currentBaudrate;
}

void Broadcom_Base_Window::setCurrentDownmode(int currentDownmode)
{

    m_currentDownmode = currentDownmode;
    emit currentDownmodeChanged(m_currentDownmode);
}

void Broadcom_Base_Window::setCurrentBaudrate(int currentBaudrate)
{

    m_currentBaudrate = currentBaudrate;
    emit currentBaudrateChanged(m_currentBaudrate);
}

