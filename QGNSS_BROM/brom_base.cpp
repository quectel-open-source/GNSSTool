#include "brom_base.h"

BROM_BASE::BROM_BASE(QObject *parent)
    :QObject {parent}
{
    statekeyvalue_=new KeyValue();
}

BROM_BASE::~BROM_BASE()
{

}

int BROM_BASE::download_percentage() const
{
    return download_percentage_;
}

void BROM_BASE::setDownload_percentage(int newDownload_percentage)
{
    if (download_percentage_ == newDownload_percentage)
        return;
    download_percentage_ = newDownload_percentage;
    emit download_percentageChanged(download_percentage ());
}

int BROM_BASE::totaltime() const
{
    return totaltime_;
}

void BROM_BASE::setTotaltime(int newTotaltime)
{
    if (totaltime_ == newTotaltime)
        return;
    totaltime_ = newTotaltime;
    emit totaltimeChanged(totaltime ());
}

bool BROM_BASE::startdownload() const
{
    return startdownload_;
}

void BROM_BASE::setStartdownload(bool newStartdownload)
{
    if (startdownload_ == newStartdownload)
        return;
    startdownload_ = newStartdownload;
    emit startdownloadChanged(startdownload());
}

const BROM_BASE::QGNSS_Model &BROM_BASE::current_model() const
{
    return current_model_;
}

void BROM_BASE::setCurrent_model(const QGNSS_Model &newCurrent_model)
{
    if (current_model_ == newCurrent_model)
        return;
    current_model_ = newCurrent_model;
    emit current_modelChanged();
}

const QString &BROM_BASE::erroString() const
{
    return erroString_;
}

void BROM_BASE::setErroString(const QString &newErroString)
{
    erroString_ = newErroString;
    emit erroStringChanged();
}

KeyValue *BROM_BASE::statekeyvalue() const
{
    return statekeyvalue_;
}

void BROM_BASE::setStatekeyvalue(KeyValue *newStatekeyvalue)
{
    statekeyvalue_ = newStatekeyvalue;
    emit statekeyvalueChanged();
}

BROM_BASE::BROM_MSG BROM_BASE::bbmsgStr() const
{
    return m_bbmsgStr;
}

void BROM_BASE::setBbmsgStr(BROM_MSG bbmsgStr)
{
    m_bbmsgStr = bbmsgStr;
    emit bbmsgStrChanged(m_bbmsgStr);
}
