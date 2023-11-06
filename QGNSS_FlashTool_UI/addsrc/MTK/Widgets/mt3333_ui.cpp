#include "mt3333_ui.h"

MT3333_UI::MT3333_UI(QWidget *parent)
    :MTK_base_Window(parent)
{
    cfg_parameter();
    cfg_construct();
    cfg_widget();
    MTKMODEL3333_=new MTK_MODEL_3333;
    cfg_model (MTKMODEL3333_);
}

MT3333_UI::~MT3333_UI()
{

}

void MT3333_UI::start()
{
    wgt_enable(true);
    statuswgt_->setBt_open(false);
    int  baudrate= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[0]).toInt ();
    int  mode=     downmode_.indexOf (CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]).toString ());
    mtk_model->qGNSS_SetDownLoadBaudRate (baudrate);
    statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+":Downloading"});
    if(MTKMODEL3333_->start_download(edit_DA_paths_->text (),loaclitem->text (),qgnss_help_->currentmodel ()->value (),mode))
    {
        statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+":Download Success"});
    }
    statuswgt_->setBt_open(true);
    wgt_enable(false);
}

void MT3333_UI::stop()
{
    mtk_model->download_stop ();
}

void MT3333_UI::ini_IO(IODvice *io)
{
    mtk_model->MTK_init_IO (io);
}
void MT3333_UI::cfg_widget()
{
    menu_setting_=     menuBar ()->addMenu (tr("Settings"));
    menu_baudrate_= menu_setting_->addMenu (tr("Download Baudrate"));
    menu_downmode_= menu_setting_->addMenu (tr("Download Mode"));

    //[0]menubar
    connect (this,&MT3333_UI::currentBaudrateChanged,this,[&](int index){
        //settings_.setValue ("mtk3333/baudrate",baudrates_[index]);
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[index]);
    });
    connect (this,&MT3333_UI::currentDownmodeChanged,this,[&](int index){
        // settings_.setValue ("mtk3333/downmode",downmode_[index]);
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[index]);
    });
    connect (menu_baudrate_,&QMenu::triggered,this,[&](QAction *action){
        setCurrentBaudrate (baudrates_.indexOf (action->text ()));
    });
    connect (menu_downmode_,&QMenu::triggered,this,[&](QAction *action){
        setCurrentDownmode (downmode_.indexOf (action->text ()));
    });

    for (auto &act :baudrates_)
    {
        QAction *action=  menu_baudrate_->addAction (act);
        action->setCheckable (true);
        action->setActionGroup (baudrate_group_);
        //if(action->text ()==settings_.value ("mtk3333/baudrate",baudrates_[0]).toString ())
        if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[0]))
        {
            action->setChecked (true);
            setCurrentBaudrate (baudrates_.indexOf (action->text ()));
        }
    }
    for (auto &act :downmode_)
    {
        QAction *action=menu_downmode_->addAction (act);
        action->setCheckable (true);
        action->setActionGroup (downmode_group_);
        //if(action->text ()== settings_.value ("mtk3333/downmode",downmode_[0]).toString ())
        if(action->text ()==CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::downmode,downmode_[0]))
        {
            action->setChecked (true);
            setCurrentDownmode (downmode_.indexOf (action->text ()));
        }
    }
    //[1]toolbar
    toolbar_->setFloatable (false);
    toolbar_->setMovable (false);
    toolbar_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    toolbar_->addAction (QIcon(":/assets/images/uEA3A-download.svg"),tr("Download"));
    //[2]mainVlayout_
    mainVlayout_->addLayout (dafileHlayout_);
    mainVlayout_->addWidget (groupbox_);
    label_DA_name_->setText (tr("DA File:"));
    dafileHlayout_->addWidget (label_DA_name_);
    dafileHlayout_->addWidget (edit_DA_paths_);
    dafileHlayout_->addWidget (openfileBT_);
    connect (openfileBT_,&QPushButton::clicked,this,[&](){
        open_file(true);
    });
    groupVlayout_->addWidget (rom_table_);
    groupbox_->setTitle (tr("Download Information"));
    groupbox_->setLayout (groupVlayout_);
    groupbox_->setFixedHeight (120);
    //[3]table
    rom_table_->setColumnCount (4);
    rom_table_->setHorizontalHeader (headview_);
    headview_->setFirstisCheckable (false);

    rom_table_->setRowCount (1);
    rom_table_->setHorizontalHeaderLabels (tabelheaderlabels_);
    rom_table_->horizontalHeader ()->setSectionResizeMode (QHeaderView::Interactive);
    rom_table_->setColumnWidth (3,480);
    rom_table_->horizontalHeaderItem (3)->setTextAlignment (Qt::AlignLeft|Qt::AlignVCenter);


    loaclitem->setFlags (Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    rom_table_->setItem (0,1,beginitem);
    rom_table_->setItem (0,2,enditem);
    rom_table_->setItem (0,3,loaclitem);

    connect (romlb_,&QPushButton::clicked,this,[&](){
        open_file(false);
    });

    rom_table_->setCellWidget (0,0,romlb_);
    //romlb_->setFlat (true);
    //romlb_->setIcon (QIcon(":/assets/images/uEA12-open-file.svg"));
    load_file(true);
    load_file(false);
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

void MT3333_UI::open_file(bool DAbt)
{
    QString fileName;
    if(DAbt)
    {
        fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select File"), nullptr, QObject::tr("DA File (*.bin)"));
        if(fileName.isEmpty ()){return;}
        //settings_.setValue ("mtk3333/DApath",fileName);
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::DApath,fileName);
    }
    else
    {
        fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select File"), nullptr, QObject::tr("ROM File (*.bin)"));
        if(fileName.isEmpty ()){return;}
        // settings_.setValue ("mtk3333/ROMpath",fileName);
        CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::ROMpath,fileName);
    }
    load_file(DAbt);
}

void MT3333_UI::load_file(bool DAbt)
{
    QString fileName;
    if(DAbt)
    {
        fileName=CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::DApath,fileName).toString (); /*settings_.value ("mtk3333/DApath",fileName).toString ();*/
        edit_DA_paths_->setText (fileName);
    }
    else
    {
        fileName=CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::ROMpath,fileName).toString ();/* settings_.value ("mtk3333/ROMpath",fileName).toString ();*/
        if(fileName.isEmpty ()){return;}
        QFile file(fileName);
        if(!file.exists ()){return;}
        beginitem->setText ("0x00000000");
        file.open (QFile::ReadOnly);
        QByteArray data=file.readAll ();
        QString size=QString::number (data.size ()-1,16).rightJustified (8,'0').toUpper ();
        enditem->setText ("0x"+size);
        loaclitem->setText (fileName);
        loaclitem->setToolTip (fileName);
        file.close ();
    }
}

bool MT3333_UI::event(QEvent *event)
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
        return MTK_base_Window::event (event);
    }
    return MTK_base_Window::event (event);
}

void MT3333_UI::wgt_enable(bool Run)
{
    if(Run)
    {
        menubar_    ->setEnabled (false);
        rom_table_->setEnabled (false);
        openfileBT_ ->setEnabled (false);
        edit_DA_paths_->setEnabled (false);
    }
    else
    {
        menubar_    ->setEnabled (true);
        rom_table_->setEnabled (true);
        openfileBT_ ->setEnabled (true);
        edit_DA_paths_->setEnabled (true);
    }
}

void MT3333_UI::cfg_construct()
{
    headview_=new MTK_base_HeadView(Qt::Orientation::Horizontal);
}
