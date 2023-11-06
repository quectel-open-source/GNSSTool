/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         status_wgt.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-30      victor.gong
* ***************************************************************************/
#include "status_wgt.h"
#define RETURNCHAIR "&#10;&#13;"

Status_WGT::Status_WGT(QWidget *parent) : QWidget(parent)
{
    create_LogFile();
    msg_=new QTextEdit();
    msg_->setReadOnly (true);
    m_current_status=new msg_str;
    glayout_=new QGridLayout();
    glayout_->addWidget (msg_,0,0,1,2);
    glayout_->setSpacing (0);
    bt_=new QPushButton(this);
    bt_->setShortcut (QKeySequence("Ctrl+R"));
    bt_->setToolTip (tr("Run ")+"Ctrl+R");
    bt_->setStyleSheet ("QPushButton:hover "
    "{"
   "background-color: #5e5f60; "
   "border-radius:15px;"
     "}");
    bt_->setFlat (true);
    connect (bt_,&QPushButton::clicked,this,&Status_WGT::bt_click);
    setBt_open(true);

    progressbar_=new QProgressBar;
    progressbar_->setStyleSheet
    (
    "QProgressBar "
    "{"
    "border: 2px solid grey;"
    "border-radius: 5px;"
     "text-align: center;"
    "}"
    "QProgressBar::chunk "
    "{"
    "background-color: #5e5f60;"
    "margin:0px;"
    "width: 20px;"
    "}");
    /*
    msg_->setStyleSheet ("border-width:1px;"
                        "border-radius:2px;"
                        "border-style:solid;"
                        "border-color:#404142;"
                        );
    msg_->setStyleSheet ("border-width: 1px;"
                        "border-style:solid;"
                        );
    */
    msg_->setStyleSheet ("QTextEdit {border-width:2px;"
                        "border-radius:4px;"
                        "border-style:solid;"
                        "border-color:#404142;"
                        "}"
                        );

    progressbar_->setAlignment ( Qt::AlignVCenter);
    progressbar_->setFixedHeight (40);
    glayout_->addWidget (progressbar_,1,0,1,1);
    glayout_->addWidget (bt_,1,1);
    glayout_->setHorizontalSpacing (5);
    glayout_->setVerticalSpacing(10);
    bt_->setFixedSize (wgt_width_,wgt_width_);
    this->setLayout (glayout_);
    set_widget ({Ready,tr("Ready")});



    msg_->setContextMenuPolicy (Qt::ContextMenuPolicy::CustomContextMenu);
    connect (msg_,&QTextEdit::customContextMenuRequested,this,[&](const QPoint &pos){
        Q_UNUSED (pos)
        QMenu *mnu= msg_->createStandardContextMenu ();
        QAction *clearAc=new QAction("Clear",this);
        clearAc->setShortcut (QKeySequence("Ctrl+X"));
        connect (clearAc,&QAction::triggered,msg_,&QTextEdit::clear);
        mnu->addSeparator ();
        mnu->setAttribute (Qt::WA_DeleteOnClose);
        mnu->addAction (clearAc);
        mnu->move (cursor ().pos ());
        mnu->show ();
    });
}

Status_WGT::~Status_WGT()
{
  delete  m_current_status;
}

void Status_WGT::create_LogFile()
{
    fileName_ = QDateTime::currentDateTime().toString("[MMdd#hhmmss]");
    QString key=QGNSS_FLASHTOOL_HELP::get_instance ()->currentmodel ()->key ();
    //qDebug()<<key;
    QString Path = QString("./logFile/[Firmware Download][Module#%1]"+fileName_+".log").arg (key);
    auto some_logger = spdlog::basic_logger_mt<spdlog::async_factory>(fileName_.toStdString (), Path.toStdString());
         some_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l]  %v");
}

bool Status_WGT::event(QEvent *event)
{
    return QWidget::event (event);
}

void Status_WGT::paintEvent(QPaintEvent *event)
{
    Q_UNUSED (event);
}

void Status_WGT::setCurrent_status(MSG_str * current_status)
{
    if (m_current_status->info_msg == current_status->info_msg&&m_current_status->currentstatus==current_status->currentstatus)
        return;
    m_current_status->info_msg = current_status->info_msg;
    m_current_status->currentstatus=current_status->currentstatus;
    emit current_statusChanged(m_current_status);
}

Status_WGT::MSG_str * Status_WGT::current_status() const
{
    return m_current_status;
}

bool Status_WGT::bt_open() const
{
    return m_bt_open;
}

void Status_WGT::setBt_open(bool bt_open)
{
    m_bt_open = bt_open;
    if(m_bt_open)
    {
        bt_->setIcon (QIcon(":/assets/images/uEA1F-play - blue.svg"));
        bt_->setIconSize ({50,50});
    }
    else
    {
        bt_->setIcon (QIcon(":/assets/images/uEA23-stop - red.svg"));
        bt_->setIconSize ({50,50});
    }
    emit bt_openChanged(m_bt_open);
}

void Status_WGT::bt_click()
{
     emit bt_clickChanged();
}

void Status_WGT::setProgress(int value, QString txt)
{
    progressbar_->setValue (value);
    progressbar_->setFormat (QString("%1:%2%").arg (txt).arg (value));

}

void Status_WGT::set_widget(const Status_WGT::MSG_str &msgstr)
{
  Status_WGT::MSG_str msg_s;
  msg_s.info_msg=msgstr.info_msg;
  msg_s.currentstatus=msgstr.currentstatus;
  setCurrent_status(&msg_s);
    //static long index=0;
    QString nowtime_=QDateTime::currentDateTime ().toString ("[hh:mm:ss.zzz]");
    QString msg=QString("<div style='color:%3'>"

                          "<img src='%1'alt='txt' align='top' width='15' height='15'></img>"
                         // "<a>"  ""+QString::number (index)+"" "</a>"
                          "<a style='color:%3'  > "+nowtime_+"</a> %2</div>");
    auto spdlog_instance=  spdlog::get(fileName_.toStdString ());
#ifdef DEBUG_MSG
    qDebug()<<msgstr.info_msg;
#endif
    switch (msgstr.currentstatus)
    {
    case Status_WGT::Ready:
    {

        QString fialmsg=msg
                .arg (":/assets/images/uEA6B-get-started.svg")
                .arg (msgstr.info_msg)
                .arg ("#404142")
                ;

        msg_->append (fialmsg);
        spdlog_instance->info(msgstr.info_msg.toStdString ());
    }
        break;
    case Status_WGT::Downloadding:
    {

        QString fialmsg=msg
                .arg (":/assets/images/uEA3A-download -blue.svg")
                .arg (msgstr.info_msg)
                .arg ("#12a6ff")
                ;

        msg_->append (fialmsg);
         spdlog_instance->info("#########################################################################");
        spdlog_instance->info(msgstr.info_msg.toStdString ());
    }
        break;
    case Status_WGT::DownloadStep:
    {

        QString fialmsg=msg
                              .arg (":/assets/images/uEA32-breakpoint -darkblue.svg")
                              .arg (msgstr.info_msg)
                              .arg ("#0000c7")
            ;

        msg_->append (fialmsg);
         spdlog_instance->info(msgstr.info_msg.toStdString ());
    }
        break;
    case Status_WGT::Warning:
    {

        QString fialmsg=msg
                .arg (":/assets/images/uEA1D-warning - yellow.svg")
                .arg (msgstr.info_msg)
                .arg ("#ff8000")
                ;

        msg_->append (fialmsg);
        spdlog_instance->warn(msgstr.info_msg.toStdString ());
    }
        break;
    case Status_WGT::Error:
    {

        QString fialmsg=msg
                .arg (":/assets/images/uEA1C-error - orange.svg")
                .arg (msgstr.info_msg)
                .arg ("#ff6600")
                ;

        msg_->append (fialmsg);
         spdlog_instance->error(msgstr.info_msg.toStdString ());
    }
        break;
    case Status_WGT::Success:
    {
        QString fialmsg=msg
                .arg (":/assets/images/uEA0F-check - red.svg")
                .arg (msgstr.info_msg)
                .arg ("#92bd6c")
                ;

        msg_->append (fialmsg);
        spdlog_instance->info(msgstr.info_msg.toStdString ());

    }
        break;
    case Status_WGT::Fail:
    {

        QString fialmsg=msg
                .arg (":/assets/images/uEA1C-error - red.svg")
                .arg (msgstr.info_msg)
                .arg ("#e40d0a")
                ;

        msg_->append (fialmsg);
        spdlog_instance->error(msgstr.info_msg.toStdString ());

    }
        break;
    }
    spdlog_instance->flush ();
}
