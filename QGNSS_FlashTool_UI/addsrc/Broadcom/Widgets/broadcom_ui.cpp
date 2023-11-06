/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_ui.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-25      victor.gong
* ***************************************************************************/
#include "broadcom_ui.h"

Broadcom_UI::Broadcom_UI(QWidget *parent, Qt::WindowFlags flags)
    :Broadcom_Base_Window(parent,flags)
{
    downloadbt_=new QToolButton;
    upgradebt_=new QToolButton;

    cfg_parameter();
    cfg_widget();
    BroadcomModel=new Broadcom_Model();
    cfg_model (BroadcomModel);
}

Broadcom_UI::~Broadcom_UI()
{
#ifdef DEBUG_MSG
    qDebug()<<Q_FUNC_INFO;
#endif
    BroadcomModel->deleteLater ();
}

void Broadcom_UI::start()
{
    Broadcom_Base::imageoptions opt;
    opt.eraseNVM=cb_eraseNVM_->isChecked ()?1:0;
    opt.eraseLTO=cb_eraseLTO_->isChecked ()?1:0;
    opt.baudRate= currentBaudrate ();
    statuswgt_->setBt_open(false);
    int  baudrate= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[0]).toInt ();
    int  mode=     downmode_.indexOf (CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]).toString ());
    broadcombase_->qGNSS_SetDownLoadBaudRate (baudrate);
    QString dwmd=IODvice::Enum_Value2Key<Broadcom_upgrade::BC_DownMode>((Broadcom_upgrade::BC_DownMode)mode);
    statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)});
    if(BroadcomModel->start_download(edit_firmware_path_->text (),qgnss_help_->currentmodel ()->value (),mode,opt))
    {
        statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+QString(":%1").arg (dwmd)+": Success"});
    }
    statuswgt_->setBt_open(true);
}

void Broadcom_UI::stop()
{
    broadcombase_->download_stop ();
}

void Broadcom_UI::ini_IO(IODvice *io)
{
    broadcombase_->broadcom_init_IO (io);
}

    void Broadcom_UI::cfg_parameter()
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
                "Download",
                "Upgrade",
                };

}

void Broadcom_UI::cfg_widget()
{

    menu_setting_=     menuBar ()->addMenu (tr("Settings"));
    menu_baudrate_= menu_setting_->addMenu (tr("Download Baudrate"));
    //menu_downmode_= menu_setting_->addMenu (tr("Mode"));
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
    downloadbt_->setIcon (QIcon(":/assets/images/uEA3A-download.svg"));
    downloadbt_->setText (tr("Download"));
    downloadbt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    downloadbt_->setStyleSheet (boxstyle);
    downloadbt_->setCheckable (true);

    upgradebt_->setVisible (true);
    upgradebt_->setIcon (QIcon(":/assets/images/uEA3B-upload.svg"));
    upgradebt_->setText (tr("Upgrade"));
    upgradebt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    upgradebt_->setStyleSheet (boxstyle);
    upgradebt_->setCheckable (true);

    btgroup->addButton (downloadbt_);
    btgroup->addButton (upgradebt_);
    toolbar_->addWidget (downloadbt_);
    toolbar_->addWidget (upgradebt_);
    addToolBar (toolbar_);
    //[0]menubar
    connect (this,&Broadcom_UI::currentBaudrateChanged,this,[&](int index){
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[index]);
    });
    connect (this,&Broadcom_UI::currentDownmodeChanged,this,[&](int index){
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[index]);
    });
    connect (menu_baudrate_,&QMenu::triggered,this,[&](QAction *action){
        setCurrentBaudrate (baudrates_.indexOf (action->text ()));
    });
    connect (downloadbt_,&QToolButton::clicked,this,[&](bool checked){
        downloadbt_->setChecked (true);
        setCurrentDownmode (0);
        showopt_cb(0);

        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downloadpath,edit_firmware_path_->text ()).toString ());
    });
    connect (upgradebt_,&QToolButton::clicked,this,[&](bool checked){
        upgradebt_->setChecked (true);
        setCurrentDownmode (1);
        showopt_cb(1);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Upgradepath,edit_firmware_path_->text ()).toString ());
    });
    for (auto &act :baudrates_)
    {
        QAction *action=  menu_baudrate_->addAction (act);
        action->setCheckable (true);
        action->setActionGroup (baudrate_group_);
        if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[4]))
        {
            action->setChecked (true);
            setCurrentBaudrate (baudrates_.indexOf (action->text ()));
        }
    }


    QString  mode= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]).toString ();
    setCurrentDownmode (downmode_.indexOf (mode));
    showopt_cb(downmode_.indexOf (mode));
    if(downmode_.indexOf (mode))
    {
        upgradebt_->click ();
    }else
    {
        downloadbt_->click ();
    }


    //[2]mainVlayout_
    mainVlayout_->addLayout (dafileHlayout_);
    mainVlayout_->addLayout (optHlayout_);
    label_firmware_name_->setText (tr("Firmware File:"));
    dafileHlayout_->addWidget (label_firmware_name_);
    dafileHlayout_->addWidget (edit_firmware_path_);
    dafileHlayout_->addWidget (openfileBT_);

    firmware_opt_->setText (tr("Firmware Options:"));
    cb_eraseNVM_->setText (tr("EraseNVM"));
    cb_eraseLTO_->setText (tr("EraseLTO"));
    optHlayout_->addWidget (firmware_opt_);
    optHlayout_->addWidget (cb_eraseNVM_);
    optHlayout_->addWidget (cb_eraseLTO_);
    optHlayout_->addStretch ();
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

bool Broadcom_UI::event(QEvent *event)
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
        return Broadcom_Base_Window::event (event);
    }
    return Broadcom_Base_Window::event (event);
}

void Broadcom_UI::open_file()
{
    QString fileName;
    QString filter;
    if(currentDownmode ()==(int)Broadcom_Base::BC_DownMode::Download)
    {
        if(QGNSS_FLASHTOOL_HELP::get_instance ()->model ()==BROM_BASE::QGNSS_Model::LC79DA)
        {
            filter=tr("(*.bin)");
        }
        else
        {
            filter=tr("(*.bin *.patch)");
        }
    }
    else
    {
        filter=tr(" (*.bin )");
    }
    fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select File"), nullptr,filter );
    if(fileName.isEmpty ()){return;}

    switch ((Broadcom_Base::BC_DownMode)currentDownmode ())
    {
    case Broadcom_Base::BC_DownMode::Download:
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downloadpath,fileName);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downloadpath,fileName).toString ());
        break;
    case Broadcom_Base::BC_DownMode::upgrade:
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Upgradepath,fileName);
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Upgradepath,fileName).toString ());
        break;
    }

}

void Broadcom_UI::load_file()
{
    switch ((Broadcom_Base::BC_DownMode)currentDownmode ())
    {
    case Broadcom_Base::BC_DownMode::Download:
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Downloadpath,"").toString ());
        break;
    case Broadcom_Base::BC_DownMode::upgrade:
        edit_firmware_path_->setText (CFG::Setting_cfg<cfg_key>::readValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::Upgradepath,"").toString ());
        break;
    }

}

void Broadcom_UI::showopt_cb(int model/*0 download 1 upgrade*/)
{
    if(model==0)
    {
        firmware_opt_->setVisible (false);
        cb_eraseLTO_->setVisible (false);
        cb_eraseNVM_->setVisible (false);
    }
    else
    {
        firmware_opt_->setVisible (true);
        cb_eraseNVM_->setVisible (true);
        cb_eraseNVM_->setChecked (true);
        switch (QGNSS_FLASHTOOL_HELP::get_instance ()->model ())
        {

        case BROM_BASE::QGNSS_Model::LC79DA:
            cb_eraseLTO_->setChecked (true);
            cb_eraseLTO_->setVisible (true);
            break;
        default:
            cb_eraseLTO_->setChecked (false);
            cb_eraseLTO_->setVisible (false);
            break;
        }
    }
}
