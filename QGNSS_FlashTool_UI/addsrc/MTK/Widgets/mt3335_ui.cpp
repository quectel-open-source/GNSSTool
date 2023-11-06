/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mt3335_ui.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-06      victor.gong
* ***************************************************************************/
#include "mt3335_ui.h"

MT3335_UI::MT3335_UI(QWidget *parent)
    :MTK_base_Window(parent)
{
    delete romlb_;
    delete beginitem;
    delete enditem;
    delete loaclitem;

    delete label_DA_name_;
    delete edit_DA_paths_;

    cfg_parameter();
    cfg_construct();
    cfg_widget();
    MTKMODEL3335_=new MTK_MODEL_3335;
    cfg_model (MTKMODEL3335_);
    cfg_connect();
    load_cfg();
}

MT3335_UI::~MT3335_UI()
{
    MTKMODEL3335_->deleteLater ();
}

void MT3335_UI::start()
{

    auto cfg_ROM=  MTKMODEL3335_->cfg_struct ();
    //2022-06-14 fix tools-016
    if(label_cfg_pth_->text ().isEmpty ())
    {
        Misc::Utilities::showMessageBox(
            tr(" Config file text is empty!")
                ,tr("Please select a .CFG file!")
                ,tr("File Empty!")
                ,QMessageBox::Ok
            );
        return ;
    }
//    bool havecheck=false;
//    if(rom_table_->rowCount ()>=4)//2022-06-14 fix tools-016
//    {
//        cfg_ROM->rom_partition_table.selected=rom_table_->item (0,0)->checkState ()==Qt::Checked?true:false;
//        if(cfg_ROM->rom_partition_table.selected){havecheck=true;}

//        cfg_ROM->rom_bootloader.selected=rom_table_     ->item (1,0)->checkState ()==Qt::Checked?true:false;
//         if(cfg_ROM->rom_bootloader.selected){havecheck=true;}

//        cfg_ROM->rom_CM4.selected=rom_table_            ->item (2,0)->checkState ()==Qt::Checked?true:false;
//         if(cfg_ROM->rom_CM4.selected){havecheck=true;}

//        cfg_ROM->rom_gnssconfig.selected=rom_table_     ->item (3,0)->checkState ()==Qt::Checked?true:false;
//         if(cfg_ROM->rom_gnssconfig.selected){havecheck=true;}
//    }
    int row=0;
    for(auto i=cfg_ROM->cfg_roms.begin ();i!=cfg_ROM->cfg_roms.end ();++i)
    {
      (*i)->selected=rom_table_->item (row,0)->checkState ()==Qt::Checked?true:false;
      row++;
    }
    wgt_enable(true);
    statuswgt_->setBt_open(false);
    int  baudrate= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[0]).toInt ();
    QString DApth=baudrate==baudrates_[0].toInt ()?da921600pth_:da115200pth_;
    mtk_model->qGNSS_SetDownLoadBaudRate (baudrate);

    switch (currentmode ())
    {
    case MT3335_UI::Download:
    {
        statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+":Downloading"});
        if(MTKMODEL3335_->start_download(DApth,label_cfg_pth_->text (),qgnss_help_->currentmodel ()->value (),0))
        {
            statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+":Download Success"});
        }
    }
    break;
    case MT3335_UI::Format:
    {
        statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+":Formating"});
        if(MTKMODEL3335_->start_format (DApth,label_cfg_pth_->text (),qgnss_help_->currentmodel ()->value ()
                                                                           ,formatUI_->currentmode (),formatUI_->formatInfo ()->beginaddress,formatUI_->formatInfo ()->length))
        {
            statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+":Format Success"});
        }
    }
    break;
    case MT3335_UI::ReadBack:
    {
      QVector<MTK_BROM_3335::Struct_ReadBack> str_readback=  readbackUI_->Get_Struct_ReadBack ();
        statuswgt_->set_widget ({Status_WGT::Downloadding,qgnss_help_->currentmodel ()->key ()+":ReadBack"});
        if(MTKMODEL3335_->start_readback (DApth,label_cfg_pth_->text (),qgnss_help_->currentmodel ()->value (),formatUI_->currentmode (),
                                          str_readback.data (),str_readback.size ()))
        {
            statuswgt_->set_widget ({Status_WGT::Success,qgnss_help_->currentmodel ()->key ()+":ReadBack Success"});
        }
    }
    break;
    }

    statuswgt_->setBt_open(true);
    wgt_enable(false);
}

void MT3335_UI::stop()
{
    mtk_model->download_stop ();
}

void MT3335_UI::ini_IO(IODvice *io)
{
    mtk_model->MTK_init_IO (io);
}

void MT3335_UI::cfg_construct()
{
    cfg_name_=new QLabel(tr("Config File:"));
    label_cfg_pth_=new QLineEdit;
    cfgfile_Glayout_=new QHBoxLayout;
    headview_=new MTK_base_HeadView(Qt::Orientation::Horizontal);

    formatUI_=new MTK3335_Format_UI(this);
    readbackUI_=new MTK3335_ReadBack_Ui(tr("ReadBack Information"),this);
    stackWidget_=new QStackedWidget;
    downloadbt_=new QToolButton;;
    formatbt_=new QToolButton;
    readbackbt_=new QToolButton;
}

void MT3335_UI::load_cfg()
{
    QString cfgph=  CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::cfgpth,"").toString ();
    setCfgPTH (cfgph);
    setCurrentmode (UI_mode::Download);
}

void MT3335_UI::cfg_widget()
{
    this->setMinimumSize ({840,650});

    //[0]menubar
    menu_setting_ = menuBar ()->addMenu (tr("Settings"));
    menu_baudrate_= menu_setting_->addMenu (tr("Download Baudrate"));
    menu_setting_->addSeparator ();
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
    downloadbt_->setIcon (QIcon(":/assets/images/firmwareupgrade.svg"));
    downloadbt_->setText (tr("Download"));
    downloadbt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    downloadbt_->setStyleSheet (boxstyle);
    downloadbt_->setCheckable (true);

    formatbt_->setVisible (false);
    formatbt_->setIcon (QIcon(":/assets/images/format.svg"));
    formatbt_->setText (tr("Format"));
    formatbt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    formatbt_->setStyleSheet (boxstyle);
    formatbt_->setCheckable (true);

    readbackbt_->setVisible (false);//not complete
    readbackbt_->setIcon (QIcon(":/assets/images/format.svg"));
    readbackbt_->setText (tr("ReadBack"));
    readbackbt_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    readbackbt_->setStyleSheet (boxstyle);
    readbackbt_->setCheckable (true);

    btgroup->addButton (downloadbt_);
    btgroup->addButton (formatbt_);
    btgroup->addButton (readbackbt_);
    toolbar_->addWidget (downloadbt_);
    toolbar_->addWidget (formatbt_);
    toolbar_->addWidget (readbackbt_);
    addToolBar (toolbar_);
    //[2]mainVlayout_
    mainVlayout_->addLayout (cfgfile_Glayout_);
    //cfg layout
    cfgfile_Glayout_->addWidget (cfg_name_);
    cfgfile_Glayout_->addWidget (label_cfg_pth_);
    openfileBT_->setToolTip (tr("Open CFG File"));
    cfgfile_Glayout_->addWidget (openfileBT_);

    groupVlayout_->addWidget (rom_table_);
    groupbox_->setTitle (tr("Download Information"));
    groupbox_->setLayout (groupVlayout_);
    groupbox_->setFixedHeight (280);
    //[3]table
    rom_table_->setColumnCount (6);
    rom_table_->setHorizontalHeader (headview_);
    headview_->setFirstisCheckable (true);
    for(int i=1;i<tabelheaderlabels_.size ();i++)
    {
        rom_table_->setHorizontalHeaderItem (i,new QTableWidgetItem(tabelheaderlabels_[i]));
    }
    rom_table_->horizontalHeaderItem (5)->setTextAlignment (Qt::AlignLeft|Qt::AlignVCenter);
    rom_table_->horizontalHeader ()->setSectionResizeMode (QHeaderView::ResizeToContents);

    stackWidget_->addWidget (groupbox_);
    stackWidget_->addWidget (formatUI_);
    stackWidget_->addWidget (readbackUI_);
    stackWidget_->setFixedHeight (groupbox_->height ());

    mainVlayout_->addWidget (stackWidget_);
    mainVlayout_->addWidget (statuswgt_);

    //[5] status toolbar
    statusbar_->addWidget (downrate_,1);
    statusbar_->addWidget (portName_);
    statusbar_->addWidget (downbuadrate_);
    statusbar_->addWidget (downmodetxt_);
    statusbar_->addWidget (downloadtime_);

    Qt::Alignment align=Qt::AlignHCenter| Qt::AlignVCenter;
    downrate_->setAlignment(Qt::AlignLeft| Qt::AlignVCenter);
    portName_->setAlignment(align);
    downbuadrate_->setAlignment(align);
    downmodetxt_->setAlignment(align);
    downloadtime_->setAlignment(align);

    int frameStyle=QFrame::Plain;
    downrate_->setFrameStyle (frameStyle);
    portName_->setFrameStyle (frameStyle);
    downbuadrate_->setFrameStyle (frameStyle);
    downmodetxt_->setFrameStyle (frameStyle);
    downloadtime_->setFrameStyle (frameStyle);

    enabel_format_=new QAction;
    enabel_readback_=new QAction;
    addAction (enabel_format_);
    addAction (enabel_readback_);
    //###### format shortcut
    QString shortcut= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::FormatEnableShortcut,"").toString ();
    if(shortcut.isEmpty ())
    {
        shortcut= CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::FormatEnableShortcut,"Ctrl+Alt+G");
    }
    shortcut=CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::FormatEnableShortcut,"Ctrl+Alt+G").toString ();
    enabel_format_->setShortcut (shortcut);
    //####### readback shortcut
    QString readback_shortcut= CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::ReadBackShortcut,"").toString ();
    if(readback_shortcut.isEmpty ())
    {
        readback_shortcut= CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::ReadBackShortcut,"Ctrl+Alt+R");
    }
    readback_shortcut=CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::ReadBackShortcut,"Ctrl+Alt+R").toString ();
    enabel_readback_->setShortcut (readback_shortcut);

    setCentralWidget (content_);
    content_->setLayout (mainVlayout_);
    formatUI_->setLength (QString(MTKMODEL3335_->getFormatTotalLength (QGNSS_FLASHTOOL_HELP::get_instance ()->model ())).toUInt (nullptr,16));
}

void MT3335_UI::cfg_connect()
{

    connect (menu_baudrate_,&QMenu::triggered,this,[&](QAction *action){

        setCurrentBaudrate (baudrates_.indexOf (action->text ()));
        CFG::Setting_cfg<cfg_key>::writeValue (QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,action->text ());
    });

    for (auto &act :baudrates_)
    {
        QAction *action=  menu_baudrate_->addAction (act);
        action->setCheckable (true);
        action->setActionGroup (baudrate_group_);
        if(action->text ()==  CFG::Setting_cfg<cfg_key>::readValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::baudrate,baudrates_[0]).toString ())
        {
            action->setChecked (true);
            setCurrentBaudrate (baudrates_.indexOf (action->text ()));
        }
    }

    connect (openfileBT_,&QPushButton::clicked,this,[&](){
        QString cfgpth=QFileDialog::getOpenFileName (this,tr("Select Config File"),"",tr("cfg files(*.cfg)"));
        if(cfgpth.isEmpty ()){return;}
        setCfgPTH(cfgpth);
    });
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

    connect (headview_,&Base_HeadView::sectionClicked,this,[&](int index){

        if(index!=0){return;}
        //2022-06-14 fix tools-017
        if(rom_table_->rowCount ()<4)
        {
            return;
        }
        rom_table_->item (0,0)->setCheckState (headview_->firstChecked ()?Qt::Checked:Qt::Unchecked);
        rom_table_->item (1,0)->setCheckState (headview_->firstChecked ()?Qt::Checked:Qt::Unchecked);
        rom_table_->item (2,0)->setCheckState (headview_->firstChecked ()?Qt::Checked:Qt::Unchecked);
        rom_table_->item (3,0)->setCheckState (headview_->firstChecked ()?Qt::Checked:Qt::Unchecked);
    });

    connect (rom_table_,&MTK_TableWidget::itemClicked,this,[&](QTableWidgetItem *item){
        if(item->column ()==0)
        {
            bool selectedall=true;
            for(int i=0;i<rom_table_->rowCount ();i++)
            {
                if(rom_table_->item (i,0)->checkState ()==Qt::Unchecked)
                {
                    selectedall=false;
                }
            }
            headview_->setFirstChecked (selectedall);
        }
    });

    connect (label_cfg_pth_,&QLineEdit::returnPressed,this,[&](){
        setCfgPTH(label_cfg_pth_->text ());
    });

    connect (downloadbt_,&QToolButton::pressed,this,[&](){
        setCurrentmode(UI_mode::Download);
    });

    connect (formatbt_,&QToolButton::pressed,this,[&](){
        setCurrentmode(UI_mode::Format);
    });

    connect (readbackbt_,&QToolButton::pressed,this,[&](){
        setCurrentmode(UI_mode::ReadBack);
    });

    connect (enabel_format_,&QAction::triggered,this,[&](bool checked){
        Q_UNUSED (checked)

        if(toolbar_->actions ()[1]->isVisible())
        {
            //formatbt_->setVisible (false);
            toolbar_->actions ()[1]->setVisible (false);
        }
        else
        {
            //formatbt_->setVisible (true);
            toolbar_->actions ()[1]->setVisible (true);
        }
    });

    connect (enabel_readback_,&QAction::triggered,this,[&](bool checked){
        Q_UNUSED (checked)

        if(toolbar_->actions ()[2]->isVisible())
        {
            //formatbt_->setVisible (false);
            toolbar_->actions ()[2]->setVisible (false);
        }
        else
        {
            //formatbt_->setVisible (true);
            toolbar_->actions ()[2]->setVisible (true);
        }
    });
}

    void MT3335_UI::cfg_parameter()
{
    baudrates_=
        {
            "921600",
            "115200",
            };
    downmode_=
        {
            "Formatdownload",
            };
    tabelheaderlabels_={
        "",
        "Name",
        "Begin Address",
        "End Address",
        "Address Type",
        "File Path"
    };

}

bool MT3335_UI::event(QEvent *event)
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

QString MT3335_UI::cfgPTH() const
{
    return m_cfgPTH;
}

void MT3335_UI::setCfgPTH(QString cfgPTH)
{
    m_cfgPTH = cfgPTH;
    CFG::Setting_cfg<cfg_key>::writeValue(QGNSS_FLASHTOOL_HELP::get_instance ()->model (),cfg_key::cfgpth,m_cfgPTH);
    if(label_cfg_pth_)
    {

        QFile file(m_cfgPTH);
        if(!file.exists ()){return;}
        label_cfg_pth_->setText (m_cfgPTH);
        file.open (QFile::ReadOnly);
        QByteArray cgfdata=file.readAll ();
        file.close ();
        QFileInfo fileinfo(m_cfgPTH);
        MTKMODEL3335_->cfg_parse (cgfdata,fileinfo.absoluteDir ().path ());
        load_table_content();

    }
    emit cfgPTHChanged(m_cfgPTH);
}

void MT3335_UI::load_table_content()
{
  /*
    rom_table_->setRowCount (4);
    for(int i=0;i<rom_table_->rowCount ();i++)
    {
        QTableWidgetItem *check=new QTableWidgetItem;
        check->setCheckState (Qt::CheckState::Checked);
        rom_table_->setItem (i,0,check);
        for (int j=1;j<6;j++)
        {
            QTableWidgetItem *txt=new QTableWidgetItem;
            rom_table_->setItem (i,j,txt);
        }
    }
    auto cfg_ROM=  MTKMODEL3335_->cfg_struct ();

    //name
    rom_table_->item (0,1)->setText (cfg_ROM->rom_partition_table.name);
    rom_table_->item (1,1)->setText (cfg_ROM->rom_bootloader.name);
    rom_table_->item (2,1)->setText (cfg_ROM->rom_CM4.name);
    rom_table_->item (3,1)->setText (cfg_ROM->rom_gnssconfig.name);
    //Begin Address
    rom_table_->item (0,2)->setText ("0x"+QString(cfg_ROM->rom_partition_table.begin_address));
    rom_table_->item (1,2)->setText ("0x"+QString(cfg_ROM->rom_bootloader.begin_address));
    rom_table_->item (2,2)->setText ("0x"+QString(cfg_ROM->rom_CM4.begin_address));
    rom_table_->item (3,2)->setText ("0x"+QString(cfg_ROM->rom_gnssconfig.begin_address));
    //End Address
    rom_table_->item (0,3)->setText ("0x"+QString::number (QString(cfg_ROM->rom_partition_table.begin_address).toInt (nullptr,16)+cfg_ROM->rom_partition_table.size,16).toUpper ());
    rom_table_->item (1,3)->setText ("0x"+QString::number (QString(cfg_ROM->rom_bootloader.begin_address).toInt (nullptr,16)+cfg_ROM->rom_bootloader.size,16).toUpper ());
    rom_table_->item (2,3)->setText ("0x"+QString::number (QString(cfg_ROM->rom_CM4.begin_address).toInt (nullptr,16)+cfg_ROM->rom_CM4.size,16).toUpper ());
    rom_table_->item (3,3)->setText ("0x"+QString::number (QString(cfg_ROM->rom_gnssconfig.begin_address).toInt (nullptr,16)+cfg_ROM->rom_gnssconfig.size,16).toUpper ());
    //Address Type
    rom_table_->item (0,4)->setText (IODvice::Enum_Value2Key<MTK_MODEL_3335::Address_Type>(cfg_ROM->addresstype));
    rom_table_->item (1,4)->setText (IODvice::Enum_Value2Key<MTK_MODEL_3335::Address_Type>(cfg_ROM->addresstype));
    rom_table_->item (2,4)->setText (IODvice::Enum_Value2Key<MTK_MODEL_3335::Address_Type>(cfg_ROM->addresstype));
    rom_table_->item (3,4)->setText (IODvice::Enum_Value2Key<MTK_MODEL_3335::Address_Type>(cfg_ROM->addresstype));
    //File path
    QFileInfo fileinfo(cfgPTH ());
    QString dir= fileinfo.absoluteDir ().path ();

    rom_table_->item (0,5)->setText (dir+"/"+QString(cfg_ROM->rom_partition_table.file));
    rom_table_->item (1,5)->setText (dir+"/"+QString(cfg_ROM->rom_bootloader.file));
    rom_table_->item (2,5)->setText (dir+"/"+QString(cfg_ROM->rom_CM4.file));
    rom_table_->item (3,5)->setText (dir+"/"+QString(cfg_ROM->rom_gnssconfig.file));
    rom_table_->resizeColumnToContents (5);
*/

  auto cfg_ROM=  &MTKMODEL3335_->cfg_struct ()->cfg_roms;

  int row=0;
  QFileInfo fileinfo(cfgPTH ());
  QString dir= fileinfo.absoluteDir ().path ();
  rom_table_->clear ();
  for(int i=1;i<tabelheaderlabels_.size ();i++)
  {
    rom_table_->setHorizontalHeaderItem (i,new QTableWidgetItem(tabelheaderlabels_[i]));
  }
  rom_table_->horizontalHeaderItem (5)->setTextAlignment (Qt::AlignLeft|Qt::AlignVCenter);
  rom_table_->horizontalHeader ()->setSectionResizeMode (QHeaderView::ResizeToContents);

  //while(cfg_ROM)
  for(auto i=cfg_ROM->begin ();i!=cfg_ROM->end ();++i)
  {
    auto rom=*i;
    if(rom_table_->rowCount ()<=row)
    {
      rom_table_->insertRow (row);
    }
    QTableWidgetItem *check=new QTableWidgetItem;
    check->setCheckState (Qt::CheckState::Checked);
    rom_table_->setItem (row,0,check);
    for (int j=1;j<6;j++)
    {
      QTableWidgetItem *txt=new QTableWidgetItem;
      rom_table_->setItem (row,j,txt);
    }
    rom_table_->item (row,1)->setText (rom->name);
    rom_table_->item (row,2)->setText ("0x"+QString::number( rom->begin_address,16).toUpper ());
    rom_table_->item (row,3)->setText ("0x"+QString::number (rom->begin_address+rom->size,16).toUpper ());
    rom_table_->item (row,4)->setText (MTKMODEL3335_->cfg_struct ()->addresstype);
    rom_table_->item (row,5)->setText (dir+"/"+QString(rom->file));
    row++;
  }
  rom_table_->resizeColumnToContents (5);
}

void MT3335_UI::wgt_enable(bool Run)
{
    if(Run)
    {
        menubar_    ->setEnabled (false);
        downloadbt_ ->setEnabled (false);
        rom_table_->setEnabled (false);
        openfileBT_ ->setEnabled (false);
        formatbt_->setEnabled (false);
        label_cfg_pth_->setEnabled (false);
        enabel_format_->setEnabled (false);
    }
    else
    {
        menubar_    ->setEnabled (true);
        downloadbt_ ->setEnabled (true);
        rom_table_->setEnabled (true);
        openfileBT_ ->setEnabled (true);
        formatbt_->setEnabled (true);
        label_cfg_pth_->setEnabled (true);
        enabel_format_->setEnabled (true);
    }
}

MT3335_UI::UI_mode MT3335_UI::currentmode() const
{
    return m_currentmode;
}

void MT3335_UI::setCurrentmode(UI_mode currentmode)
{
    if(stackWidget_)
    {
        stackWidget_->setCurrentIndex (currentmode);
        if(stackWidget_->currentIndex ()==0)
        {
            downloadbt_->setChecked (true);
        }
        else
        {
            formatbt_->setChecked (true);
        }
    }
    m_currentmode = currentmode;
    emit currentmodeChanged(m_currentmode);
}
