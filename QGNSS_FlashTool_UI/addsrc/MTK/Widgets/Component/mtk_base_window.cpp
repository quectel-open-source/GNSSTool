/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_base_window.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-06      victor.gong
* ***************************************************************************/
#include "mtk_base_window.h"

MTK_base_Window::MTK_base_Window(QWidget *parent, Qt::WindowFlags flags)
    :Base_MainWindow (parent,flags)
{
    construct_ui();
}

MTK_base_Window::~MTK_base_Window()
{
}

void MTK_base_Window::construct_ui()
{
    toolbar_=new QToolBar;
    menubar_=new QMenuBar;
    statusbar_=new QStatusBar;
    baudrate_group_=new QActionGroup(this);
    downmode_group_=new QActionGroup(this);
    content_=new QWidget(this);
    /*content_->setStyleSheet ("QWidget{"
                            "background: #f0f0f0;"
                            "}");
    */
    mainVlayout_=new QVBoxLayout(content_);
    dafileHlayout_=new QHBoxLayout();
    label_DA_name_=new QLabel(content_);
    edit_DA_paths_=new QLineEdit(content_);
    openfileBT_=new QPushButton(content_);
    openfileBT_->setIcon (QIcon(":/assets/images/uEA12-open-file.svg"));
    openfileBT_->setToolTip (tr("Open DA File"));
    groupbox_=new QGroupBox(content_);
    groupVlayout_=new QVBoxLayout(groupbox_);
    rom_table_    =new MTK_TableWidget(groupbox_);

    statuswgt_=new Status_WGT;
    romlb_=new QPushButton(tr("ROM"));
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

void MTK_base_Window::cfg_model(MTK_Base *Base)
{
    //[4] status widget
    mtk_model=Base;
    qgnss_help_=QGNSS_FLASHTOOL_HELP::get_instance ();

    connect (mtk_model,&MTK_Base::download_percentageChanged,this,[&](int value){

        statuswgt_->setProgress (value,mtk_model->statekeyvalue ()->key ());
    });
    connect (mtk_model,&MTK_Base::erroStringChanged,this,[&](){
        Status_WGT::MSG_str str{Status_WGT::Error,mtk_model->erroString ()};
        statuswgt_->set_widget (str);
    });
    connect (mtk_model,&MTK_Base::bbmsgStrChanged,this,[&](const BROM_BASE::BROM_MSG &msgstr){
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
    connect (mtk_model,&MTK_Base::statekeyvalueChanged ,this,[&](){
        Status_WGT::MSG_str str{Status_WGT::DownloadStep,mtk_model->statekeyvalue ()->key ()};
        statuswgt_->set_widget (str);
    });
    connect (mtk_model,&MTK_Base::flashInfoChanged ,this,[&](){


        downrate_->setText (mtk_model->flashInfo ()["DownloadRate"].toString ());

        portName_->setText (mtk_model->flashInfo ()["PORTNAME"].toString ());
        downbuadrate_->setText (tr("Download Baudrate:")+mtk_model->flashInfo ()["DOWNLOADBAUDRATE"].toString ());
        downmodetxt_->setText (mtk_model->flashInfo ()["DownLoadMode"].toString ());

        downloadtime_->setText (tr("Download Time:")+mtk_model->flashInfo ()["DownLoadTotaltime"].toString ());

        QString flashinfo=  mtk_model->flashInfo ()["TooltipTXT"].toString ();
        statusbar_->setToolTip (flashinfo);
    });
}

void MTK_base_Window::cfg_parameter()
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
                "4800",
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
int MTK_base_Window::currentDownmode() const
{
    return m_currentDownmode;
}

int MTK_base_Window::currentBaudrate() const
{
    return m_currentBaudrate;
}

void MTK_base_Window::setCurrentDownmode(int currentDownmode)
{

    m_currentDownmode = currentDownmode;
    emit currentDownmodeChanged(m_currentDownmode);
}

void MTK_base_Window::setCurrentBaudrate(int currentBaudrate)
{

    m_currentBaudrate = currentBaudrate;
    emit currentBaudrateChanged(m_currentBaudrate);
}

