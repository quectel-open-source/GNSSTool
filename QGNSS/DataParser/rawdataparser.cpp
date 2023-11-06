#include "rawdataparser.h"
#include "QDebug"
#include <mainwindow.h>
#include <RTKLib/rtklib.h>

RawDataParser::RawDataParser(QWidget *parent) : QObject(parent)
{
    updeteSettings();
}

int RawDataParser::inputData(QByteArray raw_data)
{
#ifdef DisplayToMainWindow
//    MainWindow * mainForm = static_cast<MainWindow*>(parentWidget());
#endif
    foreach (char c, raw_data)
    {
        if (curSte.length() != 0)
        {
            // NMEA Msg
            if (curSte.at(0) == '$')
            {
                if (c == '\n')
                {
                    curSte.append(c);
#ifdef DisplayToMainWindow
                    mainForm->m_textConsoleWidget->displaySentence(curSte);
#endif
                    emit displayTextData(curSte);
                    // Adds to the queue and waits for parsing
                    NMEA_Stes_Q.append(curSte);
                    // View the queue status
                    static int maxSize = NMEA_Stes_Q.size();
                    if (NMEA_Stes_Q.size() > maxSize)
                    {
                        maxSize = NMEA_Stes_Q.size();
                        qInfo() << "NMEA_Stes_Q max size:" << maxSize;
                    }
                    // make the next sentence
                    curSte.clear();
                }
                else
                {
                    curSte.append(c);
                    if (curSte.length() == 2)
                    {
                        if (!NMEA_prefix.contains(curSte.at(1)))
                        {
                            qDebug() << curSte << "---is not a nmea statement prefix.";
                            curSte.clear();
                        }
                    }
                    // over length
                    if (curSte.length() > NMEA_MaxLength)
                    {
                        curSte.clear();
                    }
                }
            }
            // Bream Msg
            else if (static_cast<uint8_t>(curSte.at(0)) == 0xB5)
            {
                if (curSte.length() < 6)
                {
                    curSte.append(c);
                    if (static_cast<uint8_t>(curSte.at(1)) != 0x62)
                    {
                        curSte.clear();
                        continue;
                    }
                    if (curSte.length() == 6)
                    {
                        /****/
                        msgLength = (static_cast<uint8_t>(curSte.at(5)) << 8) + static_cast<uint8_t>(curSte.at(4)) + 8;
                        if (msgLength > Bream_MaxLength)
                            curSte.clear();
                    }
                }
                else if (static_cast<uint32_t>(curSte.length()) < msgLength)
                {
                    curSte.append(c);
                    if (static_cast<uint32_t>(curSte.length()) == msgLength)
                    {
#ifdef DisplayToMainWindow
                        mainForm->m_binConsoleWidget->displayHexText(curSte);
#endif
                        emit displayHexData(curSte);
                        breamStes_Q.append(curSte);
                        // View the queue status
                        static int maxSize = breamStes_Q.size();
                        if (breamStes_Q.size() > maxSize)
                        {
                            maxSize = breamStes_Q.size();
                            qInfo() << "breamStes_Q max size:" << maxSize;
                        }
                        curSte.clear();
                    }
                }
            }
            // RTCM Msg
            else if (static_cast<uint8_t>(curSte.at(0)) == 0xD3)
            {
                if (curSte.length() < 3)
                {
                    curSte.append(c);
                    if (getbitu(reinterpret_cast<const uint8_t *>(curSte.data()), 8, 6) != 0x00)
                    {
                        curSte.clear();
                        continue;
                    }
                    if (curSte.length() == 3)
                    {
                        msgLength = static_cast<int>(getbitu(reinterpret_cast<const uint8_t *>(curSte.data()), 14, 10) + 6);
                        if (msgLength > RTCM_MaxLength)
                            curSte.clear();
                    }
                }
                else if (static_cast<uint32_t>(curSte.length()) < msgLength)
                {
                    curSte.append(c);
                    if (static_cast<uint32_t>(curSte.length()) == msgLength)
                    {
#ifdef DisplayToMainWindow
                        mainForm->m_binConsoleWidget->displayHexText(curSte);
#endif
                        RTCM_Stes_Q.append(curSte);
                        // View the queue status
                        static int maxSize = RTCM_Stes_Q.size();
                        if (RTCM_Stes_Q.size() > maxSize)
                        {
                            maxSize = RTCM_Stes_Q.size();
                            qInfo() << "RTCM_Stes_Q max size:" << maxSize;
                        }
                        curSte.clear();
                    }
                }
            }
            // binaryMTK Msg
            else if (static_cast<uint8_t>(curSte.at(0)) == 0x04)
            {
                if (curSte.length() < 4)
                {
                    curSte.append(c);
                    if (static_cast<uint8_t>(curSte.at(1)) != 0x24)
                    {
                        curSte.clear();
                        continue;
                    }
                    if (curSte.length() == 4)
                    {
                        msgLength = (static_cast<uint8_t>(curSte.at(3)) << 8) + static_cast<uint8_t>(curSte.at(2));
                        // When the msgLength is equal to 4, it will always be stuck in this loop, so choose the intInRange()
                        if (!intInRange(binaryMTK_MinLength, binaryMTK_MaxLength, msgLength))
                            curSte.clear();
                    }
                }
                else if (static_cast<uint32_t>(curSte.length()) < msgLength)
                {
                    curSte.append(c);
                    if (static_cast<uint32_t>(curSte.length()) == msgLength)
                    {
#ifdef DisplayToMainWindow
                        mainForm->m_binConsoleWidget->displayHexText(curSte);
#endif
                        emit displayHexData(curSte);
                        binaryMTK_Stes_Q.append(curSte);
                        // View the queue status
                        static int maxSize = binaryMTK_Stes_Q.size();
                        if (binaryMTK_Stes_Q.size() > maxSize)
                        {
                            maxSize = binaryMTK_Stes_Q.size();
                            qDebug() << "binaryMTK_Stes_Q max size:" << maxSize;
                        }
                        curSte.clear();
                    }
                }
            }
            // CASIC Msg
            else if (static_cast<uint8_t>(curSte.at(0)) == 0xBA)
            {
                if (curSte.length() < 4)
                {
                    curSte.append(c);
                    if (static_cast<uint8_t>(curSte.at(1)) != 0xCE)
                    {
                        curSte.clear();
                        continue;
                    }
                    if (curSte.length() == 4)
                    {
                        msgLength = (static_cast<uint8_t>(curSte.at(3)) << 8) + static_cast<uint8_t>(curSte.at(2)) + 10;
                        if (msgLength > CASIC_MaxLength)
                            curSte.clear();
                    }
                }
                else if (static_cast<uint32_t>(curSte.length()) < msgLength)
                {
                    curSte.append(c);
                    if (static_cast<uint32_t>(curSte.length()) == msgLength)
                    {
#ifdef DisplayToMainWindow
                        mainForm->m_binConsoleWidget->displayHexText(curSte);
#endif
                        emit displayHexData(curSte);
                        CASIC_Stes_Q.append(curSte);
                        // View the queue status
                        static int maxSize = CASIC_Stes_Q.size();
                        if (CASIC_Stes_Q.size() > maxSize)
                        {
                            maxSize = CASIC_Stes_Q.size();
                            qDebug() << "CASIC_Stes_Q max size:" << maxSize;
                        }
                        curSte.clear();
                    }
                }
            }
            // P1 Msg
            else if (static_cast<uint8_t>(curSte.at(0)) == 0x2E)
            {
                if (curSte.length() < 24)
                {
                    curSte.append(c);
                    if (static_cast<uint8_t>(curSte.at(1)) != 0x31)
                    {
                        curSte.clear();
                        continue;
                    }
                    if (curSte.length() == 24)
                    {
                        msgLength = *(reinterpret_cast<int *>(&curSte.data()[16])) + 24;
                        // When the msgLength is equal to 24, it will always be stuck in this loop, so choose the intInRange()
                        if (!intInRange(P1_MinLength, P1_MaxLength, msgLength))
                            curSte.clear();
                    }
                }
                else if (static_cast<uint32_t>(curSte.length()) < msgLength)
                {
                    curSte.append(c);
                    if (static_cast<uint32_t>(curSte.length()) == msgLength)
                    {
                        emit displayHexData(curSte);
                        P1_Stes_Q.append(curSte);
                        // View the queue status
                        static int maxSize = P1_Stes_Q.size();
                        if (breamStes_Q.size() > maxSize)
                        {
                            maxSize = breamStes_Q.size();
                            qDebug() << "P1_Stes_Q max size:" << maxSize;
                        }
                        curSte.clear();
                    }
                }
            }
        }
        else if (Protocol_prefix.contains(c))
        {
            curSte.append(c);
        }
    }
    return 0;
}

void RawDataParser::clearData()
{
    curSte.clear();
    breamStes_Q.clear();
    NMEA_Stes_Q.clear();
    RTCM_Stes_Q.clear();
    binaryMTK_Stes_Q.clear();
    CASIC_Stes_Q.clear();
}

ProtocolType RawDataParser::getProtocolTypeType(QByteArray data)
{
    QByteArray type = "RMC";
    if (data.contains(type))
        return NMEA_RMC;
    type = "GGA";
    if (data.contains(type))
        return NMEA_GGA;
    type = "GNS";
    if (data.contains(type))
        return NMEA_GNS;
    type.clear();
    type.append(0xd3);
    type.append(static_cast<char>(0));
    if (data.contains(type))
        return RTCM;
    return Unknown;
}

void RawDataParser::updeteSettings()
{
    QSettings settings(SETTINGS_FILE_NAME, QSettings::IniFormat);
    Protocol_prefix.clear();
    if (settings.value(parseRTCM_Key, 1).toBool())
    {
        Protocol_prefix.insert(0xD3);
    }
    if (settings.value(parseNMEA_Key, 1).toBool())
    {
        Protocol_prefix.insert('$');
    }
    if (settings.value(parseMTK_Key, 1).toBool())
        Protocol_prefix.insert(0x04);
    if (settings.value(parseBroad_Key, 1).toBool())
        Protocol_prefix.insert(0xB5);
    if (settings.value(parseCASIC_Key, 1).toBool())
        Protocol_prefix.insert(0xBA);
    if (settings.value(parseP1_Key, 1).toBool())
        Protocol_prefix.insert(0x2E);
}
