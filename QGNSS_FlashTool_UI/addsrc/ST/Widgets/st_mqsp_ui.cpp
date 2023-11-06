/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_mqsp_ui.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-17      victor.gong
* ***************************************************************************/
#include "st_mqsp_ui.h"
#include "expert_mqsp_upg_ui.h"
ST_MQSP_UI::ST_MQSP_UI(QWidget *parent, Qt::WindowFlags flags)
    :ST_Base_Window(parent,flags)
{
    GNSSupgradebt_=new QToolButton;
    mcuupgradebt_=new QToolButton;
    mcuDownloadbt_=new QToolButton;

    optHlayout_->setEnabled (false);

    cfg_parameter();
    cfg_widget();
    STModel=new ST_MQSP_Model();
    cfg_model (STModel);

    connect (STModel,&ST_MQSP_Model::showGNSSFWINFO_UI ,this,[=](GNSS_FW_INFO *opt)
            {
              qDebug()<<"showGNSSFWINFO_UI ";
              Expert_MQSP_UPG_UI *upgui=new Expert_MQSP_UPG_UI(opt,this);
              upgui->exec ();
            });

    connect (STModel,&ST_MQSP_Model::showAPPFWINFO_UI ,this,[=](APP_FW_INFO *opt)
            {
              qDebug()<<"showAPPFWINFO_UI ";
              Expert_MQSP_UPG_UI *upgui=new Expert_MQSP_UPG_UI(opt,this);
              upgui->exec ();
            });
}

ST_MQSP_UI::~ST_MQSP_UI()
{
#ifdef DEBUG_MSG
    qDebug()<<Q_FUNC_INFO;
#endif
    STModel->deleteLater ();
}

void ST_MQSP_UI::start()
{
    wgt_enable(true);
    statuswgt_->setBt_open(false);
    int  baudrate= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Baudrate,baudrates_[0]).toInt ();
    int  mode=     downmode_.indexOf (CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downmode,downmode_[0]).toString ());
    stBase->qGNSS_SetDownLoadBaudRate (baudrate);
    QString dwmd=IODvice::Enum_Value2Key<ST_MQSP_Upgrade::Down_Mode>((ST_MQSP_Upgrade::Down_Mode)mode);
    statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)});

    if(STModel->start_download(edit_firmware_path_->text (),qgnss_help_->currentmodel ()->value (),mode,downloadOpt_ac_->isChecked ()))
    {
        statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)+": Success"});
    }
    statuswgt_->setBt_open(true);
    wgt_enable(false);
}

void ST_MQSP_UI::stop()
{
    stBase->download_stop ();
}

void ST_MQSP_UI::ini_IO(IODvice *io)
{
    stBase->init_IO (io);
}

    void ST_MQSP_UI::cfg_parameter()
{
    baudrates_=
        {
            "57600",
            "115200",
            "230400",
            "460800",
            "921600",
            };

    downmode_=
        {
            "MCU_Download",
            "MCU_upgrade",
            "GNSS_upgrade"
        };
}

void ST_MQSP_UI::cfg_widget()
{
    menu_setting_=     menuBar ()->addMenu (tr("Settings"));
    menu_baudrate_= menu_setting_->addMenu (tr("Download Baudrate"));
    //menu_downmode_= menu_setting_->addMenu (tr("Mode"));
    showExpertMenu_=new QAction;
    addAction (showExpertMenu_);
    showExpertMenu_->setShortcut ( QKeySequence("Ctrl+Shift+E") );
    showExpert_= menuBar ()->addMenu (tr("Expert"));
    connect (showExpertMenu_,&QAction::triggered,this,[=](bool chk)
            {
              qDebug()<<chk;
              if(showExpert_->menuAction()->isVisible ())
              {
                showExpert_->menuAction ()->setVisible (false);
              }
              else
              {
                showExpert_->menuAction ()->setVisible (true);
              }
            });
    showExpert_->menuAction ()->setVisible (false);
    downloadOpt_ac_=new QAction(QIcon(),"EnableDownloadOpt");
    downloadOpt_ac_->setCheckable (true);
    downloadOpt_ac_->setChecked (false);
    showExpert_->addAction (downloadOpt_ac_);
    //[1]toolbar
    toolbar_->setFloatable (false);
    toolbar_->setMovable (false);
    toolbar_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    QButtonGroup *btgroup=new QButtonGroup(this);
    QString boxstyle="QToolButton:hover{"
                       "background:#ffffff;"
                       "}"
                       "QToolButton:checked{"
                       "background:#ffffff;"
                       "}";
    mcuDownloadbt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
    mcuDownloadbt_->setText (tr("MCU_Download"));
    mcuDownloadbt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    mcuDownloadbt_->setStyleSheet (boxstyle);
    mcuDownloadbt_->setCheckable (true);

    mcuupgradebt_->setVisible (true);
    mcuupgradebt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
    mcuupgradebt_->setText (tr("MCU_Upgrade"));
    mcuupgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    mcuupgradebt_->setStyleSheet (boxstyle);
    mcuupgradebt_->setCheckable (true);


    GNSSupgradebt_->setVisible (true);
    GNSSupgradebt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
    GNSSupgradebt_->setText (tr("GNSS_Upgrade"));
    GNSSupgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    GNSSupgradebt_->setStyleSheet (boxstyle);
    GNSSupgradebt_->setCheckable (true);

    btgroup->addButton (mcuDownloadbt_);
    btgroup->addButton (mcuupgradebt_);
    btgroup->addButton (GNSSupgradebt_);
    toolbar_->addWidget (mcuDownloadbt_);
    toolbar_->addWidget (mcuupgradebt_);
    toolbar_->addWidget (GNSSupgradebt_);
    addToolBar (toolbar_);
    //[0]menubar
    connect (this,&ST_MQSP_UI::currentBaudrateChanged,this,[&](int index){
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Baudrate,baudrates_[index]);
    });
    connect (this,&ST_MQSP_UI::currentDownmodeChanged,this,[&](int index){
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downmode,downmode_[index]);
    });
    connect (menu_baudrate_,&QMenu::triggered,this,[&](QAction *action){
        setCurrentBaudrate (baudrates_.indexOf (action->text ()));
    });
    connect (mcuDownloadbt_,&QToolButton::clicked,this,[&](bool checked){
        Q_UNUSED (checked)
        mnu_enable(true);
        mcuDownloadbt_->setChecked (true);
        setCurrentDownmode (0);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                          cfg_key::MCUDownLoadPath,edit_firmware_path_->text ()).toString ());
        uint filesize=0;
        QString crc_txt="";
        GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
        filesize_txt->setText (QString::number (filesize));
        filecrc_txt->setText (crc_txt);
    });
    connect (mcuupgradebt_,&QToolButton::clicked,this,[&](bool checked){
        Q_UNUSED (checked)
        mnu_enable(false);
        mcuupgradebt_->setChecked (true);
        setCurrentDownmode (1);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                          cfg_key::MCUUpgradePath,edit_firmware_path_->text ()).toString ());
        uint filesize=0;
        QString crc_txt="";
        GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
        filesize_txt->setText (QString::number (filesize));
        filecrc_txt->setText (crc_txt);
    });
    connect (GNSSupgradebt_,&QToolButton::clicked,this,[&](bool checked){
        Q_UNUSED (checked)
        mnu_enable(false);
        GNSSupgradebt_->setChecked (true);
        setCurrentDownmode (2);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),
                                                                          cfg_key::GNSSUpgradePath,edit_firmware_path_->text ()).toString ());
        uint filesize=0;
        QString crc_txt="";
        GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
        filesize_txt->setText (QString::number (filesize));
        filecrc_txt->setText (crc_txt);
    });

    for (auto &act :baudrates_)
    {
        QAction *action=  menu_baudrate_->addAction (act);
        action->setCheckable (true);
        action->setActionGroup (baudrate_group_);
        if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Baudrate,baudrates_[4]))
        {
            action->setChecked (true);
            setCurrentBaudrate (baudrates_.indexOf (action->text ()));
        }
    }

    QString  mode= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downmode,downmode_[0]).toString ();
    int index=downmode_.indexOf (mode);//(ST_MQSP_Upgrade::Down_Mode)
    if(index<0)
    {
        index=0;
    }
    setCurrentDownmode (index);
    switch ((ST_MQSP_Upgrade::Down_Mode)index)
    {
    case ST_MQSP_Upgrade::Down_Mode::MCU_Download:
        mcuDownloadbt_->click ();
        break;
    case ST_MQSP_Upgrade::Down_Mode::MCU_Upgrade:
        mcuupgradebt_->click ();
        break;
    case ST_MQSP_Upgrade::Down_Mode::GNSS_Upgrade:
        GNSSupgradebt_->click ();
        break;
    }

    //[2]mainVlayout_
    mainVlayout_->addLayout (dafileHlayout_);
    mainVlayout_->addLayout (crclayout_);
    label_firmware_name_->setText (tr("Firmware File:"));
    dafileHlayout_->addWidget (label_firmware_name_);
    dafileHlayout_->addWidget (edit_firmware_path_);
    dafileHlayout_->addWidget (openfileBT_);

    crclayout_->setColumnStretch (0,1);
    crclayout_->addWidget (size_label,0,1);
    crclayout_->addWidget (filesize_txt,0,2);
    crclayout_->addWidget (crc_label,1,1);
    crclayout_->addWidget (filecrc_txt,1,2);

    connect (openfileBT_,&QPushButton::clicked,this,[&](){
        open_file();
    });

    load_file();
    mainVlayout_->addWidget (statuswgt_);
    connect (statuswgt_,&Status_WGT::bt_clickChanged ,this,[&](){
        if(!statuswgt_->bt_open ())
        {
            stop();
        }
        else
        {
            start ();
        }
    });
    //[5] status toolbar
    statusbar_->addWidget (downrate_);
    statusbar_->addWidget (portName_);
    statusbar_->addWidget (downbuadrate_);
    statusbar_->addWidget (downmodetxt_);
    statusbar_->addWidget (downloadtime_);



    downrate_->setMinimumWidth (180);
    downrate_->setAlignment(Qt::AlignHCenter);
    portName_->setMinimumSize(portName_->sizeHint());
    portName_->setAlignment(Qt::AlignHCenter);
    downbuadrate_->setMinimumSize(downbuadrate_->sizeHint());
    downbuadrate_->setAlignment(Qt::AlignHCenter);
    downloadtime_->setMinimumSize(downloadtime_->sizeHint());
    downloadtime_->setAlignment(Qt::AlignHCenter);
    //end
    setCentralWidget (content_);
    content_->setLayout (mainVlayout_);
    this->setMinimumSize ({840,450});
}

bool ST_MQSP_UI::event(QEvent *event)
{
    switch (event->type ()) {
    case QEvent::Close:
        if(!statuswgt_->bt_open ())
        {
            Misc::Utilities::showMessageBox(
                tr("Downloading !")
                    ,tr("Close while download finished or stoped!")
                    ,tr("Warning!")
                    ,QMessageBox::Ok
                );
            event->ignore();
            return true;
        }
        this->close ();
        this->deleteLater ();

        break;
    default:
        return ST_Base_Window::event (event);
    }
    return ST_Base_Window::event (event);
}



void ST_MQSP_UI::open_file()
{
    QString fileName;
    QString filter;
    filter=tr(" (*.bin )");
    fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select File"), nullptr,filter );
    if(fileName.isEmpty ()){return;}

    switch ((ST_MQSP_Upgrade::Down_Mode)currentDownmode ())
    {
    case ST_MQSP_Upgrade::Down_Mode::MCU_Download:
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUDownLoadPath,fileName);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUDownLoadPath,fileName).toString ());
        break;
    case ST_MQSP_Upgrade::Down_Mode::MCU_Upgrade:
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUUpgradePath,fileName);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUUpgradePath,fileName).toString ());
        break;
    case ST_MQSP_Upgrade::Down_Mode::GNSS_Upgrade:
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::GNSSUpgradePath,fileName);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::GNSSUpgradePath,fileName).toString ());
        break;
    }
    uint filesize=0;
    QString crc_txt="";
    GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
    filesize_txt->setText (QString::number (filesize));
    filecrc_txt->setText (crc_txt);
}

void ST_MQSP_UI::load_file()
{
    switch ((ST_MQSP_Upgrade::Down_Mode)currentDownmode ())
    {
    case ST_MQSP_Upgrade::Down_Mode::MCU_Download:
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUDownLoadPath,"").toString ());
        break;
    case ST_MQSP_Upgrade::Down_Mode::MCU_Upgrade:
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::MCUUpgradePath,"").toString ());
        break;
    case ST_MQSP_Upgrade::Down_Mode::GNSS_Upgrade:
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::GNSSUpgradePath,"").toString ());
        break;
    }
    uint filesize=0;
    QString crc_txt="";
    GetCRCINFO (edit_firmware_path_->text (),filesize,crc_txt);
    filesize_txt->setText (QString::number (filesize));
    filecrc_txt->setText (crc_txt);
}

void ST_MQSP_UI::wgt_enable(bool Run)
{
    if(Run)
    {
        menubar_       ->setEnabled (false);
        GNSSupgradebt_ ->setEnabled (false);
        mcuupgradebt_  ->setEnabled (false);
        mcuDownloadbt_ ->setEnabled (false);
        cb_eraseNVM_   ->setEnabled (false);
        openfileBT_    ->setEnabled (false);
        //c_2m_box->setEnabled (false);
        //c_4m_box->setEnabled (false);
        edit_firmware_path_->setEnabled (false);
    }
    else
    {
        menubar_       ->setEnabled (true);
        GNSSupgradebt_ ->setEnabled (true);
        mcuupgradebt_  ->setEnabled (true);
        mcuDownloadbt_ ->setEnabled (true);
        cb_eraseNVM_   ->setEnabled (true);
        //c_2m_box->setEnabled (true);
        //c_4m_box->setEnabled (true);
        openfileBT_    ->setEnabled (true);
        edit_firmware_path_->setEnabled (true);
    }
}

void ST_MQSP_UI::mnu_enable(bool enable,QString baudrate)
{
    for (int i=0;i<menu_baudrate_->actions ().size ();i++)
    {

        if(!enable&&menu_baudrate_->actions ()[i]->text ()==baudrate)
        {
            menu_baudrate_->actions ()[i]->setChecked (true);
            setCurrentBaudrate (baudrates_.indexOf (baudrate));
        }
        else
        {
            menu_baudrate_->actions ()[i]->setEnabled (enable);
        }
    }
}

