/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         qgnss_flashtool.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-25      victor.gong
* ***************************************************************************/
#include "qgnss_flashtool.h"

QGNSS_FLASHTOOL::QGNSS_FLASHTOOL(QObject *parent)
    : QObject{parent}
{
    setopen(0);
}

QGNSS_FLASHTOOL::~QGNSS_FLASHTOOL()
{
    setopen(0);
}

QSharedPointer<QGNSS_FLASHTOOL> QGNSS_FLASHTOOL::get_instance( )
{
    static QSharedPointer<QGNSS_FLASHTOOL> appmange_=QSharedPointer<QGNSS_FLASHTOOL>(new QGNSS_FLASHTOOL());
    return appmange_;
}

bool QGNSS_FLASHTOOL::flashtool_get_io_instance(QSerialPort *serial)
{
    if(serial&&serial->isOpen ())
    {
        qio_serial_=serial;
        io_serial_=new IOserial(qio_serial_);
        return true;
    }
    else
    {
        Misc::Utilities::showMessageBox(
            tr("flash tool can not show "),
            tr("Configure and connect the serial port!"), APP_NAME,
            QMessageBox::Yes );
        return false;
    }
}

void QGNSS_FLASHTOOL::flashtool_show(QGNSSMODEL qgnssmodel,QWidget *parent)
{
    choose_model(qgnssmodel,parent);
}

bool QGNSS_FLASHTOOL::isopen()
{
    return status_==1;
}

void QGNSS_FLASHTOOL::setopen(int status)
{
    if(status_==status)
    {
        return;
    }
    status_=status;
    emit openChanged (status_==1);
}

void QGNSS_FLASHTOOL::choose_model(QGNSSMODEL md,QWidget *parent)
{
    QMetaEnum gnss_model=QMetaEnum::fromType<QGNSSMODEL>();
    QString key=gnss_model.valueToKey (md);
    KeyValue *currentmodel=new KeyValue();
    currentmodel->setKey (key);
    currentmodel->setValue ((int)md);
    qgnss_help_=QGNSS_FLASHTOOL_HELP::get_instance ();
    qgnss_help_->setCurrentmodel (currentmodel);
    qgnss_help_->setModel (md);
    // QIcon uiIcon=QIcon(":/assets/images/Download.svg");
    QIcon uiIcon=QIcon(":/assets/images/firmwareupgrade.svg");
    QString title="Module: %1";

    switch (md) {
    case QGNSSMODEL::L26_LB:
    case QGNSSMODEL::L76_LB:
    case QGNSSMODEL::LG77L:
    case QGNSSMODEL::LC86L:
    case QGNSSMODEL::L26:
    case QGNSSMODEL::L26B:
    case QGNSSMODEL::L76:
    case QGNSSMODEL::L76B:
    case QGNSSMODEL::L76_L:
    case QGNSSMODEL::L86:
    case QGNSSMODEL::L96:
    case QGNSSMODEL::L86LIC:
    case QGNSSMODEL::LG77LIC:
    case QGNSSMODEL::L70_R:
    case QGNSSMODEL::L80_R:
    case QGNSSMODEL::L70:
    case QGNSSMODEL::L80:
        qgnss_help_->setQgnss_platform (QGNSSPALTFORM::MTK3333);
        if(mt3333ui)
        {
            mt3333ui->close ();
            mt3333ui->deleteLater ();
        }
        mt3333ui=new MT3333_UI(parent);
        mt3333ui->ini_IO (io_serial_);
        connect (mt3333ui,&MT3333_UI::destroyed,this,[&](QObject *parent){
            Q_UNUSED (parent);
            setopen(0);
            mt3333ui=nullptr;
        });
        mt3333ui->setWindowIcon (uiIcon);
        mt3333ui->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
        mt3333ui->setWindowFlag (Qt::Dialog);
        mt3333ui->setWindowModality (Qt::ApplicationModal);
        mt3333ui->show ();
        break;
    case QGNSSMODEL::L89HA:
    case QGNSSMODEL::LC29HAA:
    case QGNSSMODEL::LC29HAL:
    case QGNSSMODEL::LC29HBA:
    case QGNSSMODEL::LC29HCA:
    case QGNSSMODEL::LC29HCA_TB:
    case QGNSSMODEL::LC29HDA:
    case QGNSSMODEL::LC29HEA:
    case QGNSSMODEL::LC79HAA:
    case QGNSSMODEL::LC79HAL:
    case QGNSSMODEL::LC79HBA:
    case QGNSSMODEL::LC02HAA:
    case QGNSSMODEL::LC02HBA:
        qgnss_help_->setQgnss_platform (QGNSSPALTFORM::MTK3335);
        if(mt3335ui)
        {
            mt3335ui->close ();
            mt3335ui->deleteLater ();
        }
        mt3335ui=new MT3335_UI(parent);
        mt3335ui->ini_IO (io_serial_);
        connect (mt3335ui,&MT3333_UI::destroyed,this,[&](QObject *parent){
            Q_UNUSED (parent);
            setopen(0);
            mt3335ui=nullptr;
        });
        mt3335ui->setWindowIcon (uiIcon);
        mt3335ui->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
        mt3335ui->setWindowFlag (Qt::Dialog);
        mt3335ui->setWindowModality (Qt::ApplicationModal);
        mt3335ui->show ();
        break;
    case QGNSSMODEL::LC26G:
    case QGNSSMODEL::LC26GTAA:
    case QGNSSMODEL::LC76G:
    case QGNSSMODEL::LC86G:
    {
        qgnss_help_->setQgnss_platform (QGNSSPALTFORM::MTK3352);
        if(mt3352ui)
        {
            mt3352ui->close ();
            mt3352ui->deleteLater ();
        }
        mt3352ui=new MT3352_UI(parent);
        mt3352ui->ini_IO (io_serial_);
        connect (mt3352ui,&ST_MQSP_UI::destroyed,this,[&](QObject *parent){
            Q_UNUSED (parent);
            setopen(0);
            mt3352ui=nullptr;
        });
        mt3352ui->setWindowIcon (uiIcon);
        mt3352ui->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
        mt3352ui->setWindowFlag (Qt::Dialog);
        mt3352ui->setWindowModality (Qt::ApplicationModal);
        mt3352ui->show ();
        break;
    }

    case QGNSSMODEL::LC79DA:
    case QGNSSMODEL::LC79DC:
    case QGNSSMODEL::LC79DD:

    case QGNSSMODEL::LC29DA:
    case QGNSSMODEL::LC29DB:
    case QGNSSMODEL::LC29DC:
    case QGNSSMODEL::LC29DE:
    case QGNSSMODEL::LC29DF:
    case QGNSSMODEL::LC29DG:
    case QGNSSMODEL::LC79DF:
        qgnss_help_->setQgnss_platform (QGNSSPALTFORM::Broadcom);
        if(broadcomui)
        {
            broadcomui->close ();
            broadcomui->deleteLater ();
        }
        broadcomui=new Broadcom_UI(parent);
        broadcomui->ini_IO (io_serial_);
        connect (broadcomui,&Broadcom_UI::destroyed,this,[&](QObject *parent){
            Q_UNUSED (parent);
            setopen(0);
            broadcomui=nullptr;
        });
        broadcomui->setWindowIcon (uiIcon);
        broadcomui->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
        broadcomui->setWindowFlag (Qt::Dialog);
        broadcomui->setWindowModality (Qt::ApplicationModal);
        broadcomui->show ();
        break;
    /******************ST STA8090+STA5635******************************/
    case   QGNSSMODEL::L89:
    /******************ST STA8089**************************************/
    case   QGNSSMODEL::L26ADR:
    case   QGNSSMODEL::L26ADRA:
    case   QGNSSMODEL::L26ADRC:
    case   QGNSSMODEL::L26T:
    case   QGNSSMODEL::L26T_RD:
    case   QGNSSMODEL::L26TB:
    case   QGNSSMODEL::L26P:
    case   QGNSSMODEL::L26UDR:
    case   QGNSSMODEL::LC98S:
    /******************ST STA8100*************************************************/
    case   QGNSSMODEL::LG69TAA:
    case   QGNSSMODEL::LG69TAD:
    case   QGNSSMODEL::LG69TAI:
    case   QGNSSMODEL::LG69TAJ:
    case   QGNSSMODEL::LG69TAK:

    case   QGNSSMODEL::LG69TAH:
    case   QGNSSMODEL::LC29TAA:
    case   QGNSSMODEL::LC99TIA:
    /******************ST STA9100*********************************************/
    case   QGNSSMODEL::    LG69TAB:
        qgnss_help_->setQgnss_platform (QGNSSPALTFORM::ST);
        if(stui)
        {
            stui->close ();
            stui->deleteLater ();
        }
        stui=new ST_UI(parent);
        stui->ini_IO (io_serial_);
        connect (stui,&ST_UI::destroyed,this,[&](QObject *parent){
            Q_UNUSED (parent);
            setopen(0);
            stui=nullptr;
        });
        stui->setWindowIcon (uiIcon);
        stui->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
        stui->setWindowFlag (Qt::Dialog);
        stui->setWindowModality (Qt::ApplicationModal);
        stui->show ();
        break;
    case   QGNSSMODEL::    LG69TAP:
    {
      io_serial_->io_Close ();//handle not work suitable because of qt mask rxchar and read data into cache
      qgnss_help_->setQgnss_platform (QGNSSPALTFORM::ST);
      if(st_apam_)
      {
        st_apam_->close ();
        st_apam_->deleteLater ();
      }
      st_apam_=new ST_PM_discardqt_ui (parent);

      st_apam_->ini_IO (io_serial_);
      connect (st_apam_,&ST_PM_discardqt_ui::destroyed ,this,[&](QObject *parent){
        Q_UNUSED (parent);
        setopen(0);
        st_apam_=nullptr;
        io_serial_->io_Open ();
      });
      st_apam_->setWindowIcon (uiIcon);
      st_apam_->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
      st_apam_->setWindowFlag (Qt::Dialog);
      st_apam_->setWindowModality (Qt::ApplicationModal);
      st_apam_->show ();
      break;
    }
    case   QGNSSMODEL::    LG69TAQ:
    case   QGNSSMODEL::    LG69TAM:
    case   QGNSSMODEL::    LG69TAS:
    {
        qgnss_help_->setQgnss_platform (QGNSSPALTFORM::ST);
        if(stmqsp)
        {
            stmqsp->close ();
            stmqsp->deleteLater ();
        }
        stmqsp=new ST_MQSP_UI(parent);
        stmqsp->ini_IO (io_serial_);
        connect (stmqsp,&ST_MQSP_UI::destroyed,this,[&](QObject *parent){
            Q_UNUSED (parent);
            setopen(0);
            stmqsp=nullptr;
        });
        stmqsp->setWindowIcon (uiIcon);
        stmqsp->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
        stmqsp->setWindowFlag (Qt::Dialog);
        stmqsp->setWindowModality (Qt::ApplicationModal);
        stmqsp->show ();
        break;
    }
    case QGNSSMODEL::LC76F:
    {
      qgnss_help_->setQgnss_platform (QGNSSPALTFORM::Goke);
      if(gokeui)
      {
        gokeui->close ();
        gokeui->deleteLater ();
      }
      gokeui=new Goke_UI(parent);
      gokeui->ini_IO (io_serial_);
      connect (gokeui,&Goke_UI::destroyed,this,[&](QObject *parent){
        Q_UNUSED (parent);
        setopen(0);
        gokeui=nullptr;
      });
      gokeui->setWindowIcon (uiIcon);
      gokeui->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
      gokeui->setWindowFlag (Qt::Dialog);
      gokeui->setWindowModality (Qt::ApplicationModal);
      gokeui->show ();
      break;
    }
    case QGNSSMODEL::LC29YIA:
    case QGNSSMODEL::LC760Z:
    case QGNSSMODEL::LC260Z:
    {
      qgnss_help_->setQgnss_platform (QGNSSPALTFORM::Allystar);
      if(allystarui)
      {
        allystarui->close ();
        allystarui->deleteLater ();
      }
      allystarui=new AllyStar_UI(parent);
      allystarui->ini_IO (io_serial_);
      connect (allystarui,&AllyStar_UI::destroyed,this,[&](QObject *parent){
        Q_UNUSED (parent);
        setopen(0);
        allystarui=nullptr;
      });
      allystarui->setWindowIcon (uiIcon);
      allystarui->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
      allystarui->setWindowFlag (Qt::Dialog);
      allystarui->setWindowModality (Qt::ApplicationModal);
      allystarui->show ();
      break;
    }
    case QGNSSMODEL::L76C:
    case QGNSSMODEL::L26C:
    {
      qgnss_help_->setQgnss_platform (QGNSSPALTFORM::Unicorecomm);
      if(uc_ui)
      {
        uc_ui->close ();
        uc_ui->deleteLater ();
      }
      uc_ui=new UC_UI(parent);
      uc_ui->ini_IO (io_serial_);
      connect (uc_ui,&UC_UI::destroyed,this,[&](QObject *parent){
        Q_UNUSED (parent);
        setopen(0);
        uc_ui=nullptr;
      });
      uc_ui->setWindowIcon (uiIcon);
      uc_ui->setWindowTitle (title.arg (qgnss_help_->currentmodel ()->key ()));
      uc_ui->setWindowFlag (Qt::Dialog);
      uc_ui->setWindowModality (Qt::ApplicationModal);
      uc_ui->show ();
      break;
    }
    break;
    default:
        Misc::Utilities::showMessageBox(
            tr("Model:%1 not suport Download or not exist!").arg (qgnss_help_->currentmodel ()->key ())
                ,tr("can not download!")
                ,tr("Warning!")
                ,QMessageBox::Ok
            );
        setopen(0);//fix tools-20
        return;
        //break;
    }
    setopen(1);    //fix tools-20
}
