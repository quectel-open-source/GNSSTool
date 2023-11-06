#include "nmea_parser.h"
#include "QDebug"
#include "common.h"
#include "mainwindow.h"

NMEA_Parser::NMEA_Parser(QObject *parent, NMEA_t *Nt) : QObject(parent)
{
    this->Nt = Nt;
    ResetNMEA_Data();
    NMEA_Type_ME = QMetaEnum::fromType<NMEA_Type>();
    Other_Type_ME = QMetaEnum::fromType<Other_Type>();
}

NMEA_Parser::~NMEA_Parser()
{
}

void NMEA_Parser::ResetNMEA_Data()
{
    if (Nt != nullptr)
    {
        *Nt = NMEA_t();
        EpochHasNMEA_Type.clear();
        Nt->lastTTFF = 0; // Do not calculate TTFF by default
    }
}

int NMEA_Parser::parseNMEA(QQueue<QByteArray> &NQ)
{
    while (!NQ.isEmpty())
    {
        QByteArray NMEA_Ste = NQ.takeFirst();
        if (!NMEAcheckSUM(NMEA_Ste))
        {
            Nt->_error++;
            qWarning() << "NMEA Checksum error count:" << Nt->_error << NMEA_Ste;
            continue;
        }

        Nt->_n++;
        QString NMEA_Head = NMEA_Ste.mid(3, 3);
        int NMEA_Type_E = NMEA_Type_ME.keyToValue(NMEA_Head.toLocal8Bit());
        if (EpochHasNMEA_Type.contains(NMEA_Type_E))
        {
            qDebug() << "Found new epoch.";
            EpochHasNMEA_Type.clear();
            // 整合solution
            CreateEpoch();
            // Reset
            ResetGSV_t();
            ResetGSA_t();
        }
        if (Nt->_n == 1)
        {
            qInfo() << "Receive the first NMEA:" << NMEA_Ste;
            timeCounter.restart();
        }
        switch (NMEA_Type_E)
        {
        case NMEA_Type::RMC:
        {
            EpochHasNMEA_Type.insert(NMEA_Type::RMC);
            Nt->RMC_Data._n++;
            NMEA_ProcessRMC(NMEA_Ste);
        }
        break;
        case NMEA_Type::GGA:
        {
            EpochHasNMEA_Type.insert(NMEA_Type::GGA);
            Nt->GGA_Data._n++;
            NMEA_ProcessGGA(NMEA_Ste);
        }
        break;
        case NMEA_Type::VTG:
        {
            EpochHasNMEA_Type.insert(NMEA_Type::VTG);
            Nt->VTG_Data._n++;
            NMEA_ProcessVTG(NMEA_Ste);
        }
        break;
        case NMEA_Type::GNS:
        {
            EpochHasNMEA_Type.insert(NMEA_Type::GNS);
            Nt->GNS_Data._n++;
            NMEA_ProcessGNS(NMEA_Ste);
        }
        break;
        case NMEA_Type::GLL:
        {
            EpochHasNMEA_Type.insert(NMEA_Type::GLL);
        }
        break;
        case NMEA_Type::GSA:
        {
            Nt->GSA_Data._n++;
            NMEA_ProcessGSA(NMEA_Ste);
        }
        break;
        case NMEA_Type::GSV:
        {
            Nt->GSV_Data._n++;
            NMEA_ProcessGSV(NMEA_Ste);
        }
        break;
        default:
        {
            processOther(NMEA_Ste);
        }
        break;
        }
    }
    return 0;
}

bool NMEA_Parser::NMEAcheckSUM(QByteArray NMEA_Ste)
{
    char sum = 0;
    for (int i = 1; i < NMEA_Ste.length(); i++)
    {
        if (NMEA_Ste.at(i) == '*')
        {
            if (sum == NMEA_Ste.mid(i + 1, 2).toInt(nullptr, 16))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        sum ^= NMEA_Ste.at(i);
    }
    return false;
}

int NMEA_Parser::fillSatData(const QList<QByteArray> &sFields, GSV_Data_t &GSV_data_t)
{
    SignalInfo_t sInfo = SignalInfo_t();
    if (sFields.size() % 4 == 1)
    {
        sInfo.Channel = sFields[sFields.size() - 1].toInt();
    }
    // TotalSentences
    GSV_data_t.nTotalSentences = sFields[1].toInt();
    // Number of sentences
    GSV_data_t.nSentenceNumber = sFields[2].toInt();
    // Number of satellites in view
    GSV_data_t.nSatsInView = sFields[3].toInt();

    for (int i = 1; i <= sFields.size() / 4 - 1; i++)
    {
        //        if(i*4+3 > sFields.size() - 1) return -1;
        if (!sFields[i * 4].isEmpty())
        {
            sInfo.PRN = sFields[i * 4].toInt();
            sInfo.Elevation = sFields[i * 4 + 1].toInt();
            sInfo.Azimuth = sFields[i * 4 + 2].toInt();
            sInfo.SNR = sFields[i * 4 + 3].toInt();
            GSV_data_t.signal_L.append(sInfo);
        }
        else
        {
            break;
        }
    }
    return 0;
}

int NMEA_Parser::processOther(QByteArray &NMEA_Ste)
{
    QString tmp = QString(NMEA_Ste);
    QList<QString> sFields = tmp.split('*')[0].split(',');
    int Type = Other_Type_ME.keyToValue(sFields[0].mid(1).toLocal8Bit());

    switch (Type) {
    case Other_Type::PQEPE:
        NMEA_ProcessPQEPE(NMEA_Ste);
        break;
    case Other_Type::PQTMEPE:
        NMEA_ProcessPQTMEPE(NMEA_Ste);
        break;
    case Other_Type::PQTMSVINSTATUS:
        NMEA_ProcessPQTMSVINSTATUS(NMEA_Ste);
        break;
    case Other_Type::PQTMINS:{
        if(sFields.size() > 11){
            if(sFields.at(2).size()){
                int v = sFields.at(2).toInt();
                if(v >=0 && v <=3)
                    Nt->Solution_Data.DR_Type = v;
            }
            else{
                Nt->Solution_Data.DR_Type = nullopt;
            }
        }
        else{
            qInfo()<<"Error in PQTMINS format.";
        }
    }
        break;
    default:
        break;
    }

    if (NMEA_Ste.split('*')[0] == "$PSTMEPHEMOK")
    {
        Nt->ACK._L26DR++;
    }
    else if (NMEA_Ste.split(',')[0] == "$PMTK001")
    {
        Nt->ACK._MTK++;
    }
    else if (NMEA_Ste.split('*')[0] == "$PQLTODOWNLOADOK")
    {
        Nt->ACK._nST_LTODOWNLOAD++;
    }
    else if (NMEA_Ste.split(',')[0] == "$PSTMDRCAL")
    {
        NMEA_ProcessDRCAL(NMEA_Ste);
    }
    else if (NMEA_Ste.split(',')[0] == "$PQSOL")
    {
        NMEA_ProcessUDR(NMEA_Ste);
    }
    else if (NMEA_Ste.mid(3, 3) == "STD")
    {
        Nt->DR_Data.NavModeInfo = NMEA_Ste.split(',')[2].toUInt();
    }
    else if ((NMEA_Ste.split(',')[0] == "$PSTMRTCTIME") || (NMEA_Ste.split(',')[0] == "$PSTMPPS") || (NMEA_Ste.split(',')[0] == "$PSTMODO") || (NMEA_Ste.split(',')[0] == "$PSTMSETPAR") || (NMEA_Ste.split(',')[0] == "$PSTMGEOFENCESTATUS"))
    {
        qDebug() << " NMEA_Ste:" << NMEA_Ste;
        emit parseData_L26DR(NMEA_Ste);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQSLEEP") && (sFields.count() == 2))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.cSleepMode = sFields[1].toInt();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQGLP") && (sFields.count() == 2))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.cGLPModeStatus = sFields[1].toInt();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQCNST") && (sFields.count() == 2))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.c_B0 = sFields[1].toInt() & 0x1;
        Nt->Lc79DA.c_B1 = (sFields[1].toInt() >> 1) & 0x1;
        Nt->Lc79DA.c_B2 = (sFields[1].toInt() >> 2) & 0x1;
        Nt->Lc79DA.c_B3 = (sFields[1].toInt() >> 3) & 0x1;
        Nt->Lc79DA.c_B4 = (sFields[1].toInt() >> 4) & 0x1;
        Nt->Lc79DA.c_B5 = (sFields[1].toInt() >> 5) & 0x1;
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQL5BIAS") && (sFields.count() == 2))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.dL5L5Bias = sFields[1].toDouble();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQCFGODO") && (sFields.count() == 5))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.nAuto_Start = sFields[2].toUInt();
        Nt->Lc79DA.nMSG_Mode = sFields[3].toUInt();
        Nt->Lc79DA.dInitVal = sFields[4].toDouble();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQODO") && (sFields.count() == 2))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.dOdo_Val = sFields[1].toDouble();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQCFGGEOFENCE") && (sFields.count() > 7))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.nGeoID = sFields[2].toUInt();
        Nt->Lc79DA.nGeoMode = sFields[3].toUInt();
        Nt->Lc79DA.nGeoShape = sFields[4].toUInt();
        Nt->Lc79DA.fLat0 = sFields[5].toFloat();
        Nt->Lc79DA.fLon0 = sFields[6].toFloat();
        Nt->Lc79DA.fLat1 = sFields[7].toFloat();
        Nt->Lc79DA.fLon1 = sFields.count() > 8 ? sFields[8].toFloat() : 361.0000;
        Nt->Lc79DA.fLat2 = sFields.count() > 9 ? sFields[9].toFloat() : 361.0000;
        Nt->Lc79DA.fLon2 = sFields.count() > 10 ? sFields[10].toFloat() : 361.0000;
        Nt->Lc79DA.fLat3 = sFields.count() > 11 ? sFields[11].toFloat() : 361.0000;
        Nt->Lc79DA.fLon3 = sFields.count() > 12 ? sFields[12].toFloat() : 361.0000;
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQGEOFENCE") && (sFields.count() == 2))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.nGeoEn = sFields[1].toUInt();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQCFGNMEAMSG") && (sFields.count() == 8))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.nNMEA_GGA = sFields[2].toUInt();
        Nt->Lc79DA.nNMEA_RMC = sFields[3].toUInt();
        Nt->Lc79DA.nNMEA_GSV = sFields[4].toUInt();
        Nt->Lc79DA.nNMEA_GSA = sFields[5].toUInt();
        Nt->Lc79DA.nNMEA_GLL = sFields[6].toUInt();
        Nt->Lc79DA.nNMEA_VTG = sFields[7].toUInt();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQCFGEAMASK") && (sFields.count() == 3))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.nEA_Mask = sFields[2].toUInt();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQCFGCLAMPING") && (sFields.count() == 3))
    {
        Nt->Lc79DA.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DA.nEnable = sFields[2].toUInt();
        emit parseData_LC79DA(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMPORT") && (sFields.count() == 5))
    {
        // LG69TAS
        Nt->LG69TAS.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAS.nPortType = sFields[2].toUInt();
        Nt->LG69TAS.nProtocolType = sFields[3].toUInt();
        Nt->LG69TAS.nBaudRate = sFields[4].toUInt();
        emit parseData_LG69TAS(Nt);
        // LG69TAP
        Nt->LG69TAP.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAP.nPortType = sFields[2].toUInt();
        Nt->LG69TAP.nProtocolType = sFields[3].toUInt();
        Nt->LG69TAP.nBaudRate = sFields[4].toUInt();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMRECVMODE") && (sFields.count() == 7))
    {
        Nt->LG69TAS.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAS.nMode = sFields[1].toUInt();
        Nt->LG69TAS.nDuration = sFields[2].toUInt();
        Nt->LG69TAS.dAntHeight = sFields[3].toDouble();
        Nt->LG69TAS.dECEF_X = sFields[4].toDouble();
        Nt->LG69TAS.dECEF_Y = sFields[5].toDouble();
        Nt->LG69TAS.dECEF_Z = sFields[6].toDouble();
        emit parseData_LG69TAS(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMMSMMODE") && (sFields.count() == 2))
    {
        Nt->LG69TAS.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAS.nMode = sFields[1].toUInt();
        emit parseData_LG69TAS(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCONSTMASK") && (sFields.count() == 2))
    {
        Nt->LG69TAS.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAS.nMask = sFields[1].toUInt();
        emit parseData_LG69TAS(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMVERNO") && (sFields.count() > 3))
    {
        // LG69TAP
        QByteArray commandName = sFields[0].toLatin1();
        QByteArray mainVersion = (sFields[1] + " " + sFields[2] + " " + sFields[3]).toLatin1();
        Nt->LG69TAS.commandName = commandName.data();
        Nt->LG69TAS.mainVersion = mainVersion.data();
        emit parseData_LG69TAS(Nt);
        // LG69TAP
        Nt->LG69TAP.commandName = commandName.data();
        Nt->LG69TAP.mainVersion = mainVersion.data();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMGLP") && (sFields.count() == 2)) // LC79DC
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.nMode = sFields[1].toUInt();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCNST") && (sFields.count() == 2))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.c_B0 = sFields[1].toInt() & 0x1;
        Nt->Lc79DC.c_B1 = (sFields[1].toInt() >> 1) & 0x1;
        Nt->Lc79DC.c_B2 = (sFields[1].toInt() >> 2) & 0x1;
        Nt->Lc79DC.c_B3 = (sFields[1].toInt() >> 3) & 0x1;
        Nt->Lc79DC.c_B4 = (sFields[1].toInt() >> 4) & 0x1;
        Nt->Lc79DC.c_B5 = (sFields[1].toInt() >> 5) & 0x1;
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTML5BIAS") && (sFields.count() == 2))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.dL5Bias = sFields[1].toDouble();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCFGODO") && (sFields.count() == 5))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.nAuto_Start = sFields[2].toUInt();
        Nt->Lc79DC.nMSG_Mode = sFields[3].toUInt();
        Nt->Lc79DC.dInitVal = sFields[4].toDouble();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMODO") && (sFields.count() == 2))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.dOdo_Val = sFields[1].toDouble();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCFGGEOFENCE") && sFields.count()>7)
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.nGeoID = sFields[2].toUInt();
        Nt->Lc79DC.nGeoMode = sFields[3].toUInt();
        Nt->Lc79DC.nGeoShape = sFields[4].toUInt();
        Nt->Lc79DC.fLat0 = sFields[5].toFloat();
        Nt->Lc79DC.fLon0 = sFields[6].toFloat();
        Nt->Lc79DC.fLat1 = sFields[7].toFloat();
        Nt->Lc79DC.fLon1 = sFields.count() > 8 ? sFields[8].toFloat() : 361.0000;
        Nt->Lc79DC.fLat2 = sFields.count() > 9 ? sFields[9].toFloat() : 361.0000;
        Nt->Lc79DC.fLon2 = sFields.count() > 10 ? sFields[10].toFloat() : 361.0000;
        Nt->Lc79DC.fLat3 = sFields.count() > 11 ? sFields[11].toFloat() : 361.0000;
        Nt->Lc79DC.fLon3 = sFields.count() > 12 ? sFields[12].toFloat() : 361.0000;
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMGEOFENCE") && (sFields.count() == 2))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.nGeoEn = sFields[1].toDouble();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMGEOFENCESTATUS") && (sFields.count() == 3))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.nGeoID = sFields[1].toUInt();
        Nt->Lc79DC.nGeoStatus = sFields[2].toUInt();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCFGNMEAMSG") && (sFields.count() == 8))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.nNMEA_GGA = sFields[2].toUInt();
        Nt->Lc79DC.nNMEA_RMC = sFields[3].toUInt();
        Nt->Lc79DC.nNMEA_GSV = sFields[4].toUInt();
        Nt->Lc79DC.nNMEA_GSA = sFields[5].toUInt();
        Nt->Lc79DC.nNMEA_GLL = sFields[6].toUInt();
        Nt->Lc79DC.nNMEA_VTG = sFields[7].toUInt();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCFGEAMASK") && (sFields.count() == 3))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.nEA_Mask = sFields[2].toUInt();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCFGCLAMPING") && (sFields.count() == 3))
    {
        Nt->Lc79DC.commandName = sFields[0].toLatin1().data();
        Nt->Lc79DC.nEnable = sFields[2].toUInt();
        emit parseData_LC79DC(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMNMEAMSGMASK") && (sFields.count() == 2))
    {
        bool ok;
        Nt->LG69TAP.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAP.MsgMask = sFields[1].toUInt(&ok, 16);
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMDRMODE") && (sFields.count() == 3))
    {
        Nt->LG69TAP.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAP.nSpeedMode = sFields[2].toUInt();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMFWD") && (sFields.count() == 5))
    {
        Nt->LG69TAP.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAP.nMode = sFields[2].toUInt();
        Nt->LG69TAP.nInvert = sFields[3].toUInt();
        Nt->LG69TAP.nPull = sFields[4].toUInt();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMWHEELTICK") && (sFields.count() == 5))
    {
        Nt->LG69TAP.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAP.nEdgeType = sFields[2].toUInt();
        Nt->LG69TAP.nPull = sFields[3].toUInt();
        Nt->LG69TAP.dMPT = sFields[4].toDouble();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCAN") && (sFields.count() == 7))
    {
        Nt->LG69TAP.commandName = sFields[0].toLatin1().data();
        Nt->LG69TAP.nPortID = sFields[2].toUInt();
        Nt->LG69TAP.nEnable = sFields[3].toUInt();
        Nt->LG69TAP.nFrameFormat = sFields[4].toDouble();
        Nt->LG69TAP.nBaudRate = sFields[5].toDouble();
        Nt->LG69TAP.nDataBaudrate = sFields[6].toDouble();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMCANFILTER") && (sFields.count() == 9))
    {
        QByteArray commandName = sFields[0].toLatin1();
        QByteArray ID1 = sFields[7].toLatin1();
        QByteArray ID2 = sFields[8].toLatin1();
        Nt->LG69TAP.commandName = commandName.data();
        Nt->LG69TAP.nPortID = sFields[2].toUInt();
        Nt->LG69TAP.nIndex = sFields[3].toUInt();
        Nt->LG69TAP.nEnable = sFields[4].toUInt();
        Nt->LG69TAP.nFilterType = sFields[5].toUInt();
        Nt->LG69TAP.nID_Type = sFields[6].toUInt();
        Nt->LG69TAP.ID1 = ID1.data();
        Nt->LG69TAP.ID2 = ID2.data();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMVEHDBC") && (sFields.count() == 12))
    {
        QByteArray commandName = sFields[0].toLatin1();
        QByteArray MsgID = sFields[3].toLatin1();
        Nt->LG69TAP.commandName = commandName.data();
        Nt->LG69TAP.nIndex = sFields[2].toUInt();
        Nt->LG69TAP.MsgID = MsgID.data();
        Nt->LG69TAP.nStartBit = sFields[4].toUInt();
        Nt->LG69TAP.nBitSize = sFields[5].toUInt();
        Nt->LG69TAP.nByteOrder = sFields[6].toUInt();
        Nt->LG69TAP.nValueType = sFields[7].toUInt();
        Nt->LG69TAP.dFactor = sFields[8].toDouble();
        Nt->LG69TAP.dOffset = sFields[9].toDouble();
        Nt->LG69TAP.dMin = sFields[10].toDouble();
        Nt->LG69TAP.dMax = sFields[11].toDouble();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMVEHRVAL") && (sFields.count() == 2))
    {
        QByteArray commandName = sFields[0].toLatin1();
        Nt->LG69TAP.commandName = commandName.data();
        Nt->LG69TAP.nRVal = sFields[1].toUInt();
        emit parseData_LG69TAP(Nt);
    }
    else if ((NMEA_Ste.split(',')[0] == "$PQTMTAR"))
    {
        Nt->LC02H.nMsgVer      = sFields[1 ].toUInt()  ;
        Nt->LC02H.dUTC         = sFields[2 ].toDouble();
        Nt->LC02H.nQuality     = sFields[3 ].toUInt()  ;
        Nt->LC02H.dLength      = sFields[5 ].toDouble();
        Nt->LC02H.dPitch       = sFields[6 ].toDouble();
        Nt->LC02H.dRoll        = sFields[7 ].toDouble();
        Nt->LC02H.dHeading     = sFields[8 ].toDouble();
        Nt->LC02H.dAcc_Pitch   = sFields[9 ].toDouble();
        Nt->LC02H.dAcc_Roll    = sFields[10].toDouble();
        Nt->LC02H.dAcc_Heading = sFields[11].toDouble();
        Nt->LC02H.nUsedSV      = sFields[12].toUInt()  ;
        Nt->LC02H.nIndex++;
        emit parseDataLC02H(Nt);

        lco2Nt.LC02H.nMsgVer      = sFields[1 ].toUInt()  ;
        lco2Nt.LC02H.dUTC         = sFields[2 ].toDouble();
        lco2Nt.LC02H.nQuality     = sFields[3 ].toUInt()  ;
        lco2Nt.LC02H.dLength      = sFields[5 ].toDouble();
        lco2Nt.LC02H.dPitch       = sFields[6 ].toDouble();
        lco2Nt.LC02H.dRoll        = sFields[7 ].toDouble();
        lco2Nt.LC02H.dHeading     = sFields[8 ].toDouble();
        lco2Nt.LC02H.dAcc_Pitch   = sFields[9 ].toDouble();
        lco2Nt.LC02H.dAcc_Roll    = sFields[10].toDouble();
        lco2Nt.LC02H.dAcc_Heading = sFields[11].toDouble();
        lco2Nt.LC02H.nUsedSV      = sFields[12].toUInt()  ;
        lco2Nt.LC02H.nIndex++;
        emit parseDataLC02H(lco2Nt);
    }

    return 0;
}

int NMEA_Parser::NMEA_ProcessRMC(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() >= 13)
    {
        // UTC
        if (sFields[1].size() > 6)
        {
            Nt->Solution_Data.Hour = Nt->RMC_Data.m_nHour = sFields[1].mid(0, 2).toInt();
            Nt->Solution_Data.Minute = Nt->RMC_Data.m_nMinute = sFields[1].mid(2, 2).toInt();
            Nt->Solution_Data.dSecond = Nt->RMC_Data.m_dSecond = sFields[1].mid(4).toDouble();
        }
        // Status
        if (sFields[2].size())
        {
            Nt->RMC_Data.m_nStatus = static_cast<RMC_STATUS_E>(sFields[2].at(0)); // TODO:char
        }
        // Latitude
        if (sFields[3].size() > 3)
        {
            Nt->RMC_Data.m_dLatitude = sFields[3].mid(0, 2).toDouble() + sFields[3].mid(2).toDouble() / 60;
        }
        if (sFields[4] == "S")
        {
            Nt->RMC_Data.m_dLatitude = -Nt->RMC_Data.m_dLatitude;
        }
        // Longitude
        if (sFields[5].size() > 4)
            Nt->RMC_Data.m_dLongitude = sFields[5].mid(0, 3).toDouble() + sFields[5].mid(3).toDouble() / 60;
        if (sFields[6] == "W")
        {
            Nt->RMC_Data.m_dLongitude = -Nt->RMC_Data.m_dLongitude;
        }
        // Speed over ground knots
        if (sFields[7].length())
            Nt->Solution_Data.SpeedKnots = Nt->RMC_Data.m_dSpeedKnots = sFields[7].toDouble();
        // Track Angle
        Nt->RMC_Data.m_dTrackAngle = sFields[8].toDouble();
        // Data
        if (sFields[9].size() == 6)
        {
            Nt->Solution_Data.Day = Nt->RMC_Data.m_nDay = sFields[9].mid(0, 2).toInt();
            Nt->Solution_Data.Month = Nt->RMC_Data.m_nMonth = sFields[9].mid(2, 2).toInt();
            Nt->Solution_Data.Year = Nt->RMC_Data.m_nYear = sFields[9].mid(4, 2).toInt() + 2000;
        }
        // Magnetic Variation
        Nt->RMC_Data.m_dMagneticVariation = sFields[10].toDouble();
        if (sFields[11] == "W")
            Nt->RMC_Data.m_dMagneticVariation *= -1.0;
        // Positioning system mode
        if (sFields[12].size())
            Nt->RMC_Data.m_cMode = sFields[12].at(0);
        // Navigational status
        if (sFields.size() >= 14)
            if (sFields[13].size())
                Nt->RMC_Data.m_cNavStatus = sFields[13].at(0);
    }
    else
    {
        Nt->RMC_Data._error++;
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessGGA(QByteArray &NMEA_Ste)
{
    strcpy_s(Nt->GGA_Data.GGA_String, NMEA_Ste.data());
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() >= 13)
    {
        // UTC
        if (sFields[1].size() > 6)
        {
            Nt->Solution_Data.Hour = Nt->GGA_Data.m_nHour = sFields[1].mid(0, 2).toInt();
            Nt->Solution_Data.Minute = Nt->GGA_Data.m_nMinute = sFields[1].mid(2, 2).toInt();
            Nt->Solution_Data.dSecond = Nt->GGA_Data.m_dSecond = sFields[1].mid(4).toDouble();
        }
        // Latitude
        if (sFields[2].size() > 3)
        {
            Nt->GGA_Data.m_dLatitude = sFields[2].mid(0, 2).toDouble() + sFields[2].mid(2).toDouble() / 60;
        }
        if (sFields[3] == "S")
        {
            Nt->GGA_Data.m_dLatitude = -Nt->GGA_Data.m_dLatitude;
        }
        // Longitude
        if (sFields[4].size() > 4)
            Nt->GGA_Data.m_dLongitude = sFields[4].mid(0, 3).toDouble() + sFields[4].mid(3).toDouble() / 60;
        if (sFields[5] == "W")
        {
            Nt->GGA_Data.m_dLongitude = -Nt->GGA_Data.m_dLongitude;
        }
        // GPS quality
        Nt->GGA_Data.m_nGPSQuality = sFields[6].toInt();
        if (Nt->GGA_Data.m_nGPSQuality)
        {
            Nt->GGA_Data._Fixed++;
        }
        if (Nt->GGA_Data.m_nGPSQuality == 4)
        {
            Nt->GGA_Data._RTK++;
        }
        else if (Nt->GGA_Data.m_nGPSQuality == 5)
        {
            Nt->GGA_Data._FloatRTK++;
        }
        // Satellites in use
        Nt->GGA_Data.m_nSatsInView = sFields[7].toInt();
        // HDOP
        if (sFields[8].size())
            Nt->Solution_Data.HDOP = sFields[8].toDouble();
        // Altitude, Meters, above mean sea level
        if (sFields[9].size() < 8)
            Nt->GGA_Data.AltitudeMSL = sFields[9].toDouble();
        // Geoidal separation, meters
        if (sFields[11].size() < 8)
            strcpy_s(Nt->GGA_Data.GeoidalSep, sFields[11]);
        // Age of Differential GPS data
        if (sFields[13].size() < 8)
            Nt->GGA_Data.m_dDifferentialAge = sFields[13].toDouble();
        if (sFields[13].size())
        {
            Nt->Solution_Data.AgeOfDiff = sFields[13].toDouble();
        }
    }
    else
    {
        Nt->GGA_Data._error++;
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessGSA(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    SatSys sys;
    if (sFields.size() >= 18 && sFields.size() <= 19)
    {
        // Auto mode
        if (sFields[1].size())
        {
            Nt->GSA_Data.cAutoMode = sFields[1].at(0);
        }
        // Fix mode
        if (sFields[2].size())
        {
            Nt->Solution_Data.FixMode = Nt->GSA_Data.cFixMode = sFields[2].at(0);
        }
        if (sFields.size() >= 19 && sFields[18].size())
        {
            if (sFields[18].toInt() > GNSS_NUM_OF_SYSTEMS + 1)
            {
                qDebug() << "GSA satellite system does not exist!";
                Nt->GSA_Data._error++;
                return 1;
            }
            sys = static_cast<SatSys>(sFields[18].toInt());
        }
        else
        {
            QString sysMark;
            if (sFields[0].size() > 3)
            {
                sysMark = sFields[0].mid(1, 2);
            }
            if (QString("_GP").indexOf(sysMark) > 0)
            {
                sys = SatSys::GPS;
            }
            else if (QString("_GL").indexOf(sysMark) > 0)
            {
                sys = SatSys::GLONASS;
            }
            else if (QString("_GB_BD").indexOf(sysMark) > 0)
            {
                sys = SatSys::BeiDou;
            }
            else if (QString("_GA").indexOf(sysMark) > 0)
            {
                sys = SatSys::Galileo;
            }
            else if (QString("_GQ_QZ").indexOf(sysMark) > 0)
            {
                sys = SatSys::QZSS;
            }
            else if (QString("_GI").indexOf(sysMark) > 0)
            {
                sys = SatSys::IRNSS;
            }
            else if (QString("_GN").indexOf(sysMark) > 0)
            {
                sys = SatSys::All;
            }
        }
        for (int i = 3; i < 15; i++)
        {
            if (sFields[i].toInt())
                Nt->GSA_Data.UsedInSolution[static_cast<int>(sys)].append(sFields[i].toInt());
        }
        // PDOP
        if (sFields[15].length())
            Nt->Solution_Data.PDOP = Nt->GSA_Data.PDOP = sFields[15].toDouble();
        // HDOP
        if (sFields[16].length())
            Nt->Solution_Data.HDOP = sFields[16].toDouble();
        // VDOP
        if (sFields[17].length())
            Nt->Solution_Data.VDOP = Nt->GSA_Data.VDOP = sFields[17].toDouble();
    }
    else
    {
        Nt->GSA_Data._error++;
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessGSV(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    QString sysMark;
    if (sFields[0].size() > 3)
    {
        sysMark = sFields[0].mid(1, 2);
    }
    if (QString("_GP").indexOf(sysMark) > 0)
    {
        fillSatData(sFields, Nt->GSV_Data.gpGSV_Data);
    }
    else if (QString("_GL").indexOf(sysMark) > 0)
    {
        fillSatData(sFields, Nt->GSV_Data.glGSV_Data);
    }
    else if (QString("_GB_BD").indexOf(sysMark) > 0)
    {
        fillSatData(sFields, Nt->GSV_Data.gbGSV_Data);
    }
    else if (QString("_GA").indexOf(sysMark) > 0)
    {
        fillSatData(sFields, Nt->GSV_Data.gaGSV_Data);
    }
    else if (QString("_GQ_QZ").indexOf(sysMark) > 0)
    {
        fillSatData(sFields, Nt->GSV_Data.gqGSV_Data);
    }
    else if (QString("_GI").indexOf(sysMark) > 0)
    {
        fillSatData(sFields, Nt->GSV_Data.giGSV_Data);
    }
    else if (QString("_GN").indexOf(sysMark) > 0)
    {
        fillSatData(sFields, Nt->GSV_Data.gnGSV_Data);
        //        fillTeseoSatData(sFields);
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessVTG(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() > 9)
    {
        // Track in reference to "true" earth poles
        Nt->VTG_Data.dTrackTrue = sFields[1].toDouble();
        Nt->VTG_Data.dTrackMag = sFields[3].toDouble();
        Nt->VTG_Data.dSpeedKn = sFields[5].toDouble();
        Nt->VTG_Data.dSpeedKm = sFields[7].toDouble();
        // Mode indicator
        if (sFields[9].size())
            Nt->VTG_Data.cModeIndicator = sFields[9].at(0);
    }
    else
    {
        Nt->VTG_Data._error++;
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessGNS(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() >= 13)
    {
        // UTC
        if (sFields[1].size() > 6)
        {
            Nt->Solution_Data.Hour = Nt->GNS_Data.nHour = sFields[1].mid(0, 2).toInt();
            Nt->Solution_Data.Minute = Nt->GNS_Data.nMinute = sFields[1].mid(2, 2).toInt();
            Nt->Solution_Data.dSecond = Nt->GNS_Data.dSecond = sFields[1].mid(4).toDouble();
        }
        // Latitude
        if (sFields[2].size() > 3)
        {
            Nt->GNS_Data.dLatitude = sFields[2].mid(0, 2).toDouble() + sFields[2].mid(2).toDouble() / 60;
        }
        if (sFields[3] == "S")
        {
            Nt->GNS_Data.dLatitude = -Nt->GNS_Data.dLatitude;
        }
        // Longitude
        if (sFields[4].size() > 4)
            Nt->GNS_Data.dLongitude = sFields[4].mid(0, 3).toDouble() + sFields[4].mid(3).toDouble() / 60;
        if (sFields[5] == "W")
        {
            Nt->GNS_Data.dLongitude = -Nt->GNS_Data.dLongitude;
        }
        // Pos mode
        if (sFields[6].size() < 8)
            memcpy(Nt->GNS_Data.cPosMode, sFields[6].data(), static_cast<uint8_t>(sFields[6].size()));
        // Satellites in use
        Nt->GNS_Data.nSatsInView = sFields[7].toInt();
        // HDOP
        if (sFields[8].size() < 8)
            strcpy_s(Nt->GNS_Data.HDOP, sFields[8]);
        if (sFields[8].size())
            Nt->Solution_Data.HDOP = sFields[8].toDouble();
        // Altitude, Meters, above mean sea level
        if (sFields[9].size() < 8)
            strcpy_s(Nt->GNS_Data.AltitudeMSL, sFields[9]);
        // Geoidal separation, meters
        if (sFields[10].size() < 8)
            strcpy_s(Nt->GNS_Data.GeoidalSep, sFields[10]);
    }
    else
    {
        Nt->GNS_Data._error++;
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessDRCAL(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() > 4)
    {
        Nt->DR_Data.dr_is_calib = sFields[1].toUInt();
        Nt->DR_Data.odo_is_calib = sFields[2].toUInt();
        Nt->DR_Data.gyro_sensitivity_is_calib = sFields[3].toUInt();
        Nt->DR_Data.gyro_bias_is_calib = sFields[4].toUInt();
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessUDR(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() > 11)
    {
        Nt->DR_Data.NavModeInfo = sFields[11].toUInt();
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessPQEPE(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() > 2)
    {
        Nt->Solution_Data.ACC_2D = sFields[1].toDouble();
        Nt->Solution_Data.ACC_3D = sFields[2].toDouble();
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessPQTMEPE(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() > 5)
    {
        Nt->Solution_Data.ACC_2D = sFields[5].toDouble();
        Nt->Solution_Data.ACC_3D = sFields[6].toDouble();
    }
    return 0;
}

int NMEA_Parser::NMEA_ProcessPQTMSVINSTATUS(QByteArray &NMEA_Ste)
{
    QList<QByteArray> sFields = NMEA_Ste.split('*')[0].split(',');
    if (sFields.size() >= 11)
    {
        Nt->SurveyInStatus.valid = sFields[3].toInt();
        Nt->SurveyInStatus.obs = sFields[6].toInt();
        Nt->SurveyInStatus.CfgDur = sFields[7].toInt();
        Nt->SurveyInStatus.MeanX = sFields[8].toDouble();
        Nt->SurveyInStatus.MeanY = sFields[9].toDouble();
        Nt->SurveyInStatus.MeanZ = sFields[10].toDouble();
        if(surveyUpdate != nullptr)
            surveyUpdate();
    }
    return 0;
}

void NMEA_Parser::clearGSV_Data()
{
    ResetGSV_t();
}

void NMEA_Parser::ResetRMC_t()
{
    Nt->RMC_Data = RMC_t();
}

void NMEA_Parser::ResetGGA_t()
{
    Nt->GGA_Data = GGA_t();
}

void NMEA_Parser::ResetGNS_t()
{
    Nt->GNS_Data = GNS_t();
}

void NMEA_Parser::ResetGSA_t()
{
    Nt->GSA_Data = GSA_t();
}

void NMEA_Parser::ResetGSV_t()
{
    Nt->GSV_Data._n = Nt->GSV_Data._error = 0;
    //    Nt->GSV_Data.received = 0;

    Nt->GSV_Data.gpGSV_Data.signal_L.clear();
    Nt->GSV_Data.gpGSV_Data.SystemID = 1;
    Nt->GSV_Data.gpGSV_Data.nSatsInView = 0;
    Nt->GSV_Data.gpGSV_Data.nSentenceNumber = 0;
    Nt->GSV_Data.gpGSV_Data.nTotalSentences = 0;

    Nt->GSV_Data.gsGSV_Data.signal_L.clear();
    Nt->GSV_Data.gsGSV_Data.SystemID = 1;
    Nt->GSV_Data.gsGSV_Data.nSatsInView = 0;
    Nt->GSV_Data.gsGSV_Data.nSentenceNumber = 0;
    Nt->GSV_Data.gsGSV_Data.nTotalSentences = 0;

    Nt->GSV_Data.glGSV_Data.signal_L.clear();
    Nt->GSV_Data.glGSV_Data.SystemID = 2;
    Nt->GSV_Data.glGSV_Data.nSatsInView = 0;
    Nt->GSV_Data.glGSV_Data.nSentenceNumber = 0;
    Nt->GSV_Data.glGSV_Data.nTotalSentences = 0;

    Nt->GSV_Data.gaGSV_Data.signal_L.clear();
    Nt->GSV_Data.gaGSV_Data.SystemID = 3;
    Nt->GSV_Data.gaGSV_Data.nSatsInView = 0;
    Nt->GSV_Data.gaGSV_Data.nSentenceNumber = 0;
    Nt->GSV_Data.gaGSV_Data.nTotalSentences = 0;

    Nt->GSV_Data.gbGSV_Data.signal_L.clear();
    Nt->GSV_Data.gbGSV_Data.SystemID = 4;
    Nt->GSV_Data.gbGSV_Data.nSatsInView = 0;
    Nt->GSV_Data.gbGSV_Data.nSentenceNumber = 0;
    Nt->GSV_Data.gbGSV_Data.nTotalSentences = 0;

    Nt->GSV_Data.gqGSV_Data.signal_L.clear();
    Nt->GSV_Data.gqGSV_Data.SystemID = 5;
    Nt->GSV_Data.gqGSV_Data.nSatsInView = 0;
    Nt->GSV_Data.gqGSV_Data.nSentenceNumber = 0;
    Nt->GSV_Data.gqGSV_Data.nTotalSentences = 0;

    Nt->GSV_Data.giGSV_Data.signal_L.clear();
    Nt->GSV_Data.giGSV_Data.SystemID = 6;
    Nt->GSV_Data.giGSV_Data.nSatsInView = 0;
    Nt->GSV_Data.giGSV_Data.nSentenceNumber = 0;
    Nt->GSV_Data.giGSV_Data.nTotalSentences = 0;

    Nt->GSV_Data.gnGSV_Data.signal_L.clear();
    Nt->GSV_Data.gnGSV_Data.SystemID = 6;
    Nt->GSV_Data.gnGSV_Data.nSatsInView = 0;
    Nt->GSV_Data.gnGSV_Data.nSentenceNumber = 0;
    Nt->GSV_Data.gnGSV_Data.nTotalSentences = 0;
}

void NMEA_Parser::ResetVTG_t()
{
    Nt->VTG_Data = VTG_t();
}

void NMEA_Parser::CreateEpoch()
{
    SavedData_t sData = SavedData_t();
    bool validSolution = false;
    if (Nt->GGA_Data.m_nGPSQuality)
    {
        validSolution = true;
        sData.GPSQuality = Nt->GGA_Data.m_nGPSQuality;
        sData.Latitude = Nt->GGA_Data.m_dLatitude;
        sData.Longitude = Nt->GGA_Data.m_dLongitude;
        sData.AltitudeMSL = Nt->GGA_Data.AltitudeMSL;
    }
    if (Nt->RMC_Data.m_nStatus == RMC_STATUS_E::RMC_STATUS_ACTIVE ||
            Nt->RMC_Data.m_nStatus == RMC_STATUS_E::RMC_STATUS_D)
    {
        validSolution = true;
        sData.Latitude = Nt->RMC_Data.m_dLatitude;
        sData.Longitude = Nt->RMC_Data.m_dLongitude;
    }
    if (validSolution)
        Nt->Solution_Data.savedData_L.append(sData);
    UpdateSatelliteInfo();
    /* After using the SSCOM test, the module
     * will continue to send the positioning
     * RMC after sending the restart command,
     * so it is changed to test the second
     * positioning statement.*/
    if (Nt->Solution_Data.savedData_L.size() == 2 && !Nt->lastTTFF.has_value())
    {
        qInfo() << "saved solution size tow.";
        Nt->lastTTFF = timeCounter.elapsed();
        if(Nt->lastTTFF.value() > 1000){
            Nt->lastTTFF = Nt->lastTTFF.value() - 1000;
        }
    }
}

void NMEA_Parser::UpdateSatelliteInfo()
{
    if(Nt->GSV_Data._n)
        Nt->Solution_Data.SatInfo.clear();
    foreach (auto var, Nt->GSV_Data.gpGSV_Data.signal_L)
    {
        var.Sys = SatSys::GPS;
        if (Nt->GSA_Data.UsedInSolution[static_cast<int>(var.Sys)].indexOf(var.PRN) >= 0)
            var.usedInSolution = 1;
        Nt->Solution_Data.SatInfo.append(var);
    }
    foreach (auto var, Nt->GSV_Data.glGSV_Data.signal_L)
    {
        var.Sys = SatSys::GLONASS;
        if (Nt->GSA_Data.UsedInSolution[static_cast<int>(var.Sys)].indexOf(var.PRN) >= 0)
            var.usedInSolution = 1;
        Nt->Solution_Data.SatInfo.append(var);
    }
    foreach (auto var, Nt->GSV_Data.gaGSV_Data.signal_L)
    {
        var.Sys = SatSys::Galileo;
        if (Nt->GSA_Data.UsedInSolution[static_cast<int>(var.Sys)].indexOf(var.PRN) >= 0)
            var.usedInSolution = 1;
        Nt->Solution_Data.SatInfo.append(var);
    }
    foreach (auto var, Nt->GSV_Data.gqGSV_Data.signal_L)
    {
        var.Sys = SatSys::QZSS;
        if (Nt->GSA_Data.UsedInSolution[static_cast<int>(var.Sys)].indexOf(var.PRN) >= 0)
            var.usedInSolution = 1;
        Nt->Solution_Data.SatInfo.append(var);
    }
    foreach (auto var, Nt->GSV_Data.gbGSV_Data.signal_L)
    {
        var.Sys = SatSys::BeiDou;
        if (Nt->GSA_Data.UsedInSolution[static_cast<int>(var.Sys)].indexOf(var.PRN) >= 0)
            var.usedInSolution = 1;
        Nt->Solution_Data.SatInfo.append(var);
    }
    foreach (auto var, Nt->GSV_Data.gsGSV_Data.signal_L)
    {
        var.Sys = SatSys::SBAS;
        if (Nt->GSA_Data.UsedInSolution[static_cast<int>(var.Sys)].indexOf(var.PRN) >= 0)
            var.usedInSolution = 1;
        Nt->Solution_Data.SatInfo.append(var);
    }
    foreach (auto var, Nt->GSV_Data.giGSV_Data.signal_L)
    {
        var.Sys = SatSys::IRNSS;
        if (Nt->GSA_Data.UsedInSolution[static_cast<int>(var.Sys)].indexOf(var.PRN) >= 0)
            var.usedInSolution = 1;
        Nt->Solution_Data.SatInfo.append(var);
    }
}
