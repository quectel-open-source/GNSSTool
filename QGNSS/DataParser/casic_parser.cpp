#include "casic_parser.h"

CASIC_Parser* CASIC_Parser::m_instance = nullptr;

CASIC_Parser::CASIC_Parser()
{
    initData();
}

void CASIC_Parser::initData()
{
    memset(&casic_data,0,sizeof (casic_data));
}

CASIC_Parser* CASIC_Parser::getInstance()
{
    if (m_instance == nullptr){
        m_instance = new CASIC_Parser();
    }
    return m_instance;
}

void CASIC_Parser::parseCASIC_Msg(QQueue<QByteArray> &Sets_Q)
{
    while(!Sets_Q.isEmpty()){
        QByteArray reply = Sets_Q.takeFirst();
        if(reply.size()>7){
            if(reply.at(4)==0x05 && reply.at(5)==0x00){
                casic_data.nACK++;
                casic_data.b_nACK = true;
            }
            else if(reply.at(4)==0x05 && reply.at(5)==0x01){
                casic_data.ACK++;
                casic_data.b_ACK = true;
            }
            else if(reply.at(4)==0x06 && reply.at(5)==0x00 && reply.size() > 13){
                casic_data.prt.portID = reply.at(6);
                casic_data.prt.ProtoMask = reply.at(7);
                casic_data.prt.MODE = (static_cast<uint8_t>(reply.at(9)) << 8) + static_cast<uint8_t>(reply.at(8));
                casic_data.prt.BaudRate =
                        (static_cast<uint8_t>(reply.at(13)) << 24) +
                        (static_cast<uint8_t>(reply.at(12)) << 16) +
                        (static_cast<uint8_t>(reply.at(11)) << 8) +
                        static_cast<uint8_t>(reply.at(10));
            }
            else if(reply.at(4)==0x06 && reply.at(5)==0x01 && reply.size() > 9){
                casic_data.msg.ClsID = reply.at(6);
                casic_data.msg.MsgID = reply.at(7);
                casic_data.msg.Rate = (static_cast<uint8_t>(reply.at(9)) << 8) + static_cast<uint8_t>(reply.at(8));
            }
            else if(reply.at(4)==0x06 && reply.at(5)==0x04 && reply.size() > 7){
                casic_data.rate.Interval = (static_cast<uint8_t>(reply.at(7)) << 8) + static_cast<uint8_t>(reply.at(6));
            }
        }
    }
}
