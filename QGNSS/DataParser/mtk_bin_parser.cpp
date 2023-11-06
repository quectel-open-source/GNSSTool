#include "mtk_bin_parser.h"

mtk_bin_parser::mtk_bin_parser(MTKbin_t *mtkBin)
{
    initData(mtkBin);
}

void mtk_bin_parser::initData(MTKbin_t *mtkBin)
{
    MTKbin = mtkBin;
    memset(mtkBin,0,sizeof (MTKbin_t));
}

int mtk_bin_parser::parseMTK_Bin(QQueue<QByteArray> &MQ)
{
    QByteArray mtkBin_Ste;
    while(!MQ.isEmpty())
    {
        mtkBin_Ste = MQ.takeFirst();
        if(mtkBin_Ste.at(4) == 0x02){
            MTKbin->nEPO_ACK++;
            if(mtkBin_Ste.at(6) == 0x01){
                MTKbin->_n++;
            }
        }
    }
    return 0;
}
