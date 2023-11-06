#ifndef RAWDATAPARSER_H
#define RAWDATAPARSER_H

#include <QObject>
#include <QQueue>
#include <QSet>

#include "common.h"

//#define DisplayToMainWindow

#define NMEA_MaxLength 1024
#define Bream_MaxLength 2048
#define RTCM_MaxLength 2048
#define binaryMTK_MaxLength 512
#define binaryMTK_MinLength 5
#define CASIC_MaxLength 2048
#define P1_MaxLength 2048
#define P1_MinLength 25

using namespace quectel_gnss;

class RawDataParser : public QObject
{
    Q_OBJECT
public:
    explicit RawDataParser(QWidget *parent = nullptr);
    int inputData(QByteArray raw_data);
    void clearData(void);
    static ProtocolType getProtocolTypeType(QByteArray data);

    ProtocolType mainMsg = ProtocolType::Unknown;
    QByteArray curSte;
    uint32_t msgLength = 0;
    // bream Sentence queue
    QQueue<QByteArray> breamStes_Q;
    // NMEA Sentence queue
    QQueue<QByteArray> NMEA_Stes_Q;
    // RTCM Sentence queue
    QQueue<QByteArray> RTCM_Stes_Q;
    // MTK Bin Sentence queue
    QQueue<QByteArray> binaryMTK_Stes_Q;
    // CASIC Sentence queue
    QQueue<QByteArray> CASIC_Stes_Q;
    // P1 Sentence queue
    QQueue<QByteArray> P1_Stes_Q;

private:
    QSet<char> NMEA_prefix{'P', 'G', 'B', 'Q', 'L'};
    QSet<uint8_t> Protocol_prefix{'$', 0xB5, 0xD3, 0xBA, 0x2E, 0x04};

signals:
    void displayHexData(QByteArray);
    void displayTextData(QByteArray);

public slots:
    void updeteSettings();
};

#endif // RAWDATAPARSER_H
