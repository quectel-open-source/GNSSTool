#ifndef MTK_BIN_PARSER_H
#define MTK_BIN_PARSER_H

#include "QQueue"

typedef struct{
    uint32_t _n,_error;
    uint32_t nEPO_ACK;
}MTKbin_t;

class mtk_bin_parser
{
public:
    mtk_bin_parser(MTKbin_t *mtkBin = nullptr);
    void initData(MTKbin_t *mtkBin = nullptr);
    int parseMTK_Bin(QQueue<QByteArray> &MQ);
private:
    MTKbin_t *MTKbin;
};

#endif // MTK_BIN_PARSER_H
