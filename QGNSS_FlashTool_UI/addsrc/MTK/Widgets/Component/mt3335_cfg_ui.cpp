/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mt3335_cfg_ui.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-06      victor.gong
* ***************************************************************************/
#include "mt3335_cfg_ui.h"

MT3335_cfg_UI::MT3335_cfg_UI(QWidget *parent , Qt::WindowFlags f)
              : Base_Dialog(parent,f)
{
    cfg_parameter();
    construct_ui();
    cfg_widget();
    connect_ui();
    //load_pth();
}

MT3335_cfg_UI::~MT3335_cfg_UI()
{

}

void MT3335_cfg_UI::cfg_parameter()
{

}

void MT3335_cfg_UI::cfg_widget()
{
    main_vlayout->addLayout (girdlayout_);
    main_vlayout->addStretch ();
    main_vlayout->addLayout (bt_Hlayout);

    girdlayout_->addWidget (label1_,0,0);
    girdlayout_->addWidget (line921600_,0,1);
    girdlayout_->addWidget (bt921600DA_,0,2);


    girdlayout_->addWidget (label2_,1,0);
    girdlayout_->addWidget (line115200_,1,1);
    girdlayout_->addWidget (bt115200DA_,1,2);


    bt_Hlayout->addStretch ();
    bt_Hlayout->addWidget (okbt_);
    bt_Hlayout->addWidget (cancelbt_);
    bt_Hlayout->addWidget (applybt_);
    this->setWindowTitle (tr("Download Agent"));
    this->setAttribute (Qt::WA_DeleteOnClose);
}

void MT3335_cfg_UI::construct_ui()
{
    main_vlayout=new QVBoxLayout(this);
    girdlayout_=new QGridLayout;
    bt_Hlayout=new QHBoxLayout;

    label1_=new QLabel(tr("DA File (921600)"));
    label2_=new QLabel(tr("DA File (115200)"));
    line921600_=new QLineEdit;
    line115200_=new QLineEdit;
    bt921600DA_=new QPushButton(QIcon(":/assets/images/uEA12-open-file.svg"),"");
    bt115200DA_=new QPushButton(QIcon(":/assets/images/uEA12-open-file.svg"),"");

    okbt_=new QPushButton(QIcon(":/assets/images/uEA0F-check.svg"),tr("&OK"));
    cancelbt_=new QPushButton(QIcon(":/assets/images/uEA97-cancel.svg"),tr("&Cancel"));
    applybt_=new QPushButton(QIcon(":/assets/images/uEAB2-save.svg"),tr("&Save"));
}

void MT3335_cfg_UI::connect_ui()
{
    //this
    connect (bt921600DA_,&QPushButton::clicked,this,[&](){
        QString path=QFileDialog::getOpenFileName (this,tr("Select 921600Baudrate DA file!"),"",tr("DA files (*.bin)"));
        if(path.isEmpty ()){return;}
        setDa921600path (path);
    });
    connect (bt115200DA_,&QPushButton::clicked,this,[&](){
        QString path=QFileDialog::getOpenFileName (this,tr("Select 115200Baudrate DA file!"),"",tr("DA files (*.bin)"));
        if(path.isEmpty ()){return;}
        setDa115200path (path);
    });
    connect (okbt_,&QPushButton::clicked,this,[&](){
        save_pth();
        close ();
    });
    connect (cancelbt_,&QPushButton::clicked,this,[&](){
        close ();
    });
    connect (applybt_,&QPushButton::clicked,this,[&](){
        save_pth();
    });
}

void MT3335_cfg_UI::load_pth(BROM_BASE::QGNSS_Model md)
{
        model_=md;
   // QString path921600= setting_.value ("mt3335/DA921600pth","").toString ();
   // QString path115200 =setting_.value ("mt3335/DA115200pth","").toString ();
        QString path921600= CFG::Setting_cfg<QString>::readValue(md,QString("DA921600pth"),"").toString ();
     QString path115200 =  CFG::Setting_cfg<QString>::readValue(md,QString("DA115200pth"),"").toString ();
    setDa115200path (path115200);
    setDa921600path (path921600);
}

void MT3335_cfg_UI::save_pth()
{
    //setting_.setValue ("mt3335/DA921600pth",da921600path());
    //setting_.setValue ("mt3335/DA115200pth",da115200path());
    CFG::Setting_cfg<QString>::writeValue (model_,QString("DA921600pth"),da921600path());
    CFG::Setting_cfg<QString>::writeValue(model_,QString("DA115200pth"),da115200path());
}

/**************************************property******************************************/
QString MT3335_cfg_UI::da921600path() const
{
    return m_da921600path;
}

QString MT3335_cfg_UI::da115200path() const
{
    return m_da115200path;
}

void MT3335_cfg_UI::setDa921600path(QString da921600path)
{
    if (m_da921600path == da921600path)
        return;

    m_da921600path = da921600path;
    if(line921600_)
    {
        line921600_->setText ( m_da921600path);
        line921600_->setToolTip (m_da921600path);
    }
    emit da921600pathChanged(m_da921600path);
}

void MT3335_cfg_UI::setDa115200path(QString da115200path)
{
    if (m_da115200path == da115200path)
        return;
    m_da115200path = da115200path;
    if(line115200_)
    {
        line115200_->setText ( m_da115200path);
        line115200_->setToolTip (m_da115200path);
    }
    emit da115200pathChanged(m_da115200path);
}
