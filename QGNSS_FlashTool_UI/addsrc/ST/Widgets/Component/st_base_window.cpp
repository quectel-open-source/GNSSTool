/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_base_window.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-08      victor.gong
* ***************************************************************************/
#include "st_base_window.h"

ST_Base_Window::ST_Base_Window(QWidget *parent, Qt::WindowFlags flags)
    :Base_MainWindow (parent,flags)
{
    construct_ui();
}

ST_Base_Window::~ST_Base_Window()
{
#ifdef DEBUG_MSG
    qDebug()<<Q_FUNC_INFO;
#endif
}

void ST_Base_Window::cfg_model(ST_Base* Base)
{
    stBase=Base;
    //[4] status widget
    qgnss_help_=QGNSS_FLASHTOOL_HELP::get_instance ();

    connect (stBase,&ST_Base::download_percentageChanged,this,[&](int value){

        statuswgt_->setProgress (value,stBase->statekeyvalue ()->key ());
    });

    connect (stBase,&ST_Base::bbmsgStrChanged,this,[&](const BROM_BASE::BROM_MSG &msgstr){
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

    connect (stBase,&ST_Base::statekeyvalueChanged ,this,[&](){
        Status_WGT::MSG_str str{Status_WGT::DownloadStep,stBase->statekeyvalue ()->key ()};
        statuswgt_->set_widget (str);
    });
    connect (stBase,&ST_Base::flashInfoChanged ,this,[&](){
        downrate_->setText (stBase->flashInfo ()["DownloadRate"].toString ());

        portName_->setText (stBase->flashInfo ()["PORTNAME"].toString ());
        downbuadrate_->setText (tr("Download Baudrate:")+stBase->flashInfo ()["DOWNLOADBAUDRATE"].toString ());
        downmodetxt_->setText (stBase->flashInfo ()["DownLoadMode"].toString ());

        downloadtime_->setText (tr("Download Time:")+stBase->flashInfo ()["DownLoadTotaltime"].toString ());

        QString flashinfo=  stBase->flashInfo ()["TooltipTXT"].toString ();
        statusbar_->setToolTip (flashinfo);
    });

}

void ST_Base_Window::construct_ui()
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
    crclayout_=new QGridLayout;

    label_firmware_name_=new QLabel(content_);
    edit_firmware_path_=new QLineEdit(content_);
    openfileBT_=new QPushButton(content_);
    openfileBT_->setIcon (QIcon(":/assets/images/uEA12-open-file.svg"));
    openfileBT_->setToolTip (tr("Open"));

    firmware_opt_=new QLabel;
    cb_eraseNVM_=new QCheckBox;
    cb_Recovery_=new QCheckBox();

    //formatsizegrp=new QButtonGroup;
    //c_2m_box=new QCheckBox(tr("2Mflash"));
    //c_4m_box=new QCheckBox(tr("4Mflash"));
    //cb_eraseLTO_=new QCheckBox;


    statuswgt_=new Status_WGT;
    downrate_=new QLabel;
    portName_=new QLabel;
    downbuadrate_=new QLabel;
    downmodetxt_=new QLabel;
    downloadtime_=new QLabel;

    size_label=new QLabel(tr("Size:"));
    crc_label =new QLabel(tr("CRC:"));
    filesize_txt=new QLineEdit;
    filecrc_txt=new QLineEdit;
    filesize_txt->setReadOnly (true);
    filesize_txt->setAlignment (Qt::AlignVCenter);
    filecrc_txt->setReadOnly (true);
    filecrc_txt->setAlignment (Qt::AlignVCenter);
    setMenuWidget (menubar_);
    setStatusBar (statusbar_);
}

bool ST_Base_Window::GetCRCINFO(QString filepath, uint &filesize, QString &crc)
{
    QFile file(filepath);
    if(!file.exists ())
    {
        return false;
    }
    file.open (QIODevice::ReadOnly);
    QByteArray firmwaredata_=file.readAll ();
    file.close ();
    filesize=firmwaredata_.size ();
    uint crc32num=ST_Base::calculate_ether_crc32(0U,filesize);
    crc32num=ST_Base::calculate_ether2_crc32(crc32num,firmwaredata_.data (),filesize);
    crc=QString::number (crc32num,16).toUpper ();
    return true;
}

/*******************************************property*************************/
int ST_Base_Window::currentDownmode() const
{
    return m_currentDownmode;
}

int ST_Base_Window::currentBaudrate() const
{
    return m_currentBaudrate;
}

void ST_Base_Window::setCurrentDownmode(int currentDownmode)
{
    m_currentDownmode = currentDownmode;
    emit currentDownmodeChanged(m_currentDownmode);
}

void ST_Base_Window::setCurrentBaudrate(int currentBaudrate)
{
    m_currentBaudrate = currentBaudrate;
    emit currentBaudrateChanged(m_currentBaudrate);
}
