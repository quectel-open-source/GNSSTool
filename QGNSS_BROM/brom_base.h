#ifndef BROM_BASE_H
#define BROM_BASE_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPointer>
#include <QByteArray>
#include <QMetaEnum>
#include <QPair>
#include "ioserial.h"
#include "keyvalue.h"
#include "QGNSS_BROM_global.h"
#include "nmea_0813v14.h"
#ifndef TEST_ENVIR
#include "common.h"
#endif
#define DOWNSTATE(Down_State,state) "["+IODvice::Enum_Value2Key<Down_State>(state)+"] "
class QGNSS_BROM_EXPORT BROM_BASE : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int download_percentage READ download_percentage WRITE setDownload_percentage NOTIFY download_percentageChanged)
    Q_PROPERTY(int totaltime READ totaltime WRITE setTotaltime NOTIFY totaltimeChanged)
    Q_PROPERTY(bool startdownload READ startdownload WRITE setStartdownload NOTIFY startdownloadChanged)
    Q_PROPERTY(QGNSS_Model current_model READ current_model WRITE setCurrent_model NOTIFY current_modelChanged)
    Q_PROPERTY(QString erroString READ erroString WRITE setErroString NOTIFY erroStringChanged)
    Q_PROPERTY(KeyValue* statekeyvalue READ statekeyvalue WRITE setStatekeyvalue NOTIFY statekeyvalueChanged)
    Q_PROPERTY(BROM_MSG bbmsgStr READ bbmsgStr WRITE setBbmsgStr NOTIFY bbmsgStrChanged)
public:

    explicit BROM_BASE(QObject *parent = nullptr);
    virtual ~BROM_BASE();

    int download_percentage() const;
    int totaltime() const;
    bool startdownload() const;
    KeyValue *statekeyvalue() const;
    enum QGNSS_Platform
    {
        MTK3333, /*[0]mtk3333 AG3331 MTK3337 MTK3339*/
        MTK3335,
        MTK3352,
        Broadcom,
        ST,
        Goke,
        Allystar,
        Unicorecomm,

    };
    Q_ENUM(QGNSS_Platform);

    enum class brmsgtype{
        bb_Success,
        bb_Fail,
        bb_Error,
        bb_Warning,
        bb_Info
    };
    Q_ENUM (brmsgtype)

    typedef  QPair<brmsgtype,QString> BROM_MSG ;

#ifdef TEST_ENVIR
    enum QGNSS_Model
        {
            NONE,
            /*****************MTK AG3331************************************/
            L76_LB,
            L26_LB,
            LC86L,
            LG77L,
            /*****************MTK MT3333************************************/
            L76,
            L76_L,
            L86,
            L26,
            L26B,
            L76B,
            L96,
            L86LIC,
            LG77LIC,
            /*****************MTK MT3337************************************/
            L70_R,
            L80_R,
            /*****************MTK MT3339************************************/
            L70,
            L80,
            /******************MTK AG3352***********************************/
            LC76G,
            /******************MTK AG3335MN*********************************/
            L89HA,
            LC79HBA,
            /******************MTK AG3335M***********************************/
            LC79HAA,
            LC79HAL,
            LC29HAL,
            LC29HAA,
            /******************MTK AG3335AT**********************************/

            LC29HBA,
            LC29HCA,
            LC29HCA_TB,
            LC29HDA,
            /******************MTK AG3335A************************************/
            LC29HEA,
            // Broadcom
            /******************Broadcom BCM47755*******************************/
            LC79DA,
            LC79DC,
            LC79DD,

            LC29DA,
            LC29DB,
            LC29DC,
            LC29DE,
            LC29DF,
            LC29DG,
            // ST
            /******************ST STA8090+STA5635******************************/
            L89,
            /******************ST STA8089**************************************/
            L26ADR,
            L26UDR_SCY,
            L26ADRA,
            L26ADRC_2WDR_SCY,
            L26T,
            L26T_RD,
            L26TB_SCY,
            L26P,
            LC98SIA,
            LC98SIB,
            LC98SIC,
            /******************ST STA8100*********************************************/
            LG69TAA,
            LG69TAD,
            LG69TAI,
            LG69TAJ,
            LG69TAK,
            LG69TAQ,
            LG69TAM,
            LG69TAP,
            LG69TAS,
            LG69TAB,
            /******************ST STA9100**********************************************/

            /******************Unicorecomm UC6226NIS*************************************/
            L76C,
            L26C,
            /******************Unicorecomm UC6226NIK*************************************/
            L26CB,
            /******************Unicorecomm UC6226CI**************************************/
            L26CA,
            /******************Unicorecomm UC6228CI*************************************/
            L76CC,
            L26CC,
            //Allystar
            /*********************** HD8040D ******************************************/
            LC29YIA,
            //中科微
            /******************Unicorecomm AT6558R**************************************/
            L76KA,
            L26KA,
            LC03K,
            /******************Unicorecomm AT6558R(新版)*********************************/
            L76KB,
            L26KB,
            // Goke
            /******************Goke GK9501**********************************************/
            LC76FA,
            LC76FB,

            LC98S,
            L26UDR,
            L26ADRC

        };
    Q_ENUM(QGNSS_Model);
#else
    typedef quectel_gnss::QGNSS_Model QGNSS_Model;
#endif

    /**
     * each 16 bit swap
     */
    QByteArray swip_each_16(const QByteArray &data)
    {
        QByteArray redata = data;
        if (data.size() % 2)
        {
            return redata;
        }
        for (int i = 0; i < data.size() / 2; i++)
        {
            redata[i * 2] = data.mid(i * 2, 2)[1];
            redata[i * 2 + 1] = data.mid(i * 2, 2)[0];
        }
        return redata;
    }
    QByteArray reverse_4_bys(const QByteArray &data)
    {
        QByteArray redata = data;
        redata[0]=data[3];
        redata[1]=data[2];
        redata[2]=data[1];
        redata[3]=data[0];
        return redata;
    }
    ///array reverse
    static char *RS(const char* data,size_t size)
    {
        char * rdata=new char[size];
        for(uint i=0;i<size;i++)
        {
            rdata[i]=data[size-i-1];
        }
        return  rdata;
    };

    const QGNSS_Model &current_model() const;

    const QString &erroString() const;

    BROM_MSG bbmsgStr() const;


Q_SIGNALS:
    void download_percentageChanged(int);

    void totaltimeChanged(int);

    void startdownloadChanged(bool);

    void current_modelChanged();

    void erroStringChanged();

    void rw_InfoChanged(const QPair<QString, QString> &pair);
    void statekeyvalueChanged();

    void bbmsgStrChanged(const BROM_MSG &bbmsgStr);

public Q_SLOTS:

    void setDownload_percentage(int newDownload_percentage);
    void setTotaltime(int newTotaltime);
    void setStartdownload(bool newStartdownload);
    void setCurrent_model(const QGNSS_Model &newCurrent_model);
    void setErroString(const QString &newErroString);
    void setStatekeyvalue(KeyValue *newStatekeyvalue);

    void setBbmsgStr(BROM_MSG bbmsgStr);

private:
    int download_percentage_ = 0;
    int totaltime_ = 0;
    bool startdownload_ = false;
    QGNSS_Model current_model_;
    KeyValue *statekeyvalue_;
    QString erroString_;

    BROM_MSG m_bbmsgStr;

protected:
    QPointer<IODvice> io_=nullptr;
    bool downloadstoping=false;
protected:
    int init_baudRate_;
};

#endif // BROM_BASE_H
