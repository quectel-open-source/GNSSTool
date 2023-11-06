#ifndef CASIC_PARSER_H
#define CASIC_PARSER_H

#include <QObject>
#include "QQueue"

typedef struct {
    uint32_t n;
    uint8_t portID;
    uint8_t ProtoMask;
    uint16_t MODE;
    uint32_t BaudRate;
}tPRT;

typedef struct {
    uint32_t n;
    uint16_t Interval;
}tRATE;

typedef struct {
    uint32_t n;
    uint8_t ClsID;
    uint8_t MsgID;
    uint16_t Rate;
}tMSG;

typedef struct {
    uint32_t n,ACK,nACK;
    bool b_ACK;
    bool b_nACK;
    tPRT prt;
    tRATE rate;
    tMSG msg;
}tCASIC;

class CASIC_Parser
{
public:
    static CASIC_Parser* getInstance();
    void parseCASIC_Msg(QQueue<QByteArray> &Sets_Q);

    tCASIC casic_data;
private:
    static CASIC_Parser* m_instance;
    CASIC_Parser();
    void initData();
};

#endif // CASIC_PARSER_H
