#include "mtk_datacenter.h"

MTK_DataCenter::MTK_DataCenter(QObject *parent)
    : QObject{parent}
{

}

MTK_DataCenter::~MTK_DataCenter()
{
}

QSharedPointer<MTK_DataCenter> MTK_DataCenter::get_instance()
{
    static QSharedPointer<MTK_DataCenter> MTKDataCenter=QSharedPointer<MTK_DataCenter>(new MTK_DataCenter);
    return MTKDataCenter;
}

const MTK_DataCenter::MTK_MSG &MTK_DataCenter::current_msg() const
{
    return current_msg_;
}

void MTK_DataCenter::setCurrent_msg(const MTK_MSG &newCurrent_msg)
{
    current_msg_ = newCurrent_msg;
    emit current_msgChanged();
}

void MTK_DataCenter::setRw_Info(const QPair<QString, QString> &pair)
{
    MTK_MSG Current_msg;
    QString fristString=pair.first;
    QString secondString=pair.second.toUpper ();
    Current_msg.type=fristString;
    int remaindersize =secondString.size ()%84;
    for(int i=0;i<secondString.size ()/84;i++)
    {
        Current_msg.hexStr.append (secondString.mid (i*84,84));
        Current_msg.hexStr.append(NEWLINE);
    }
    if(remaindersize)
    {
        Current_msg.hexStr.append (secondString.right (remaindersize));
        Current_msg.hexStr.append(NEWLINE);
    }
    //Current_msg.hexStr=secondString.toUpper ();
    QString binstr;
    for(int i=0;i<secondString.length ()/2;i++)
    {
        int bin_char_int =secondString.mid (i,2).toInt (nullptr,16);
        QString bin_char=QString::number (bin_char_int,2).rightJustified (8,'0');
        binstr.append (bin_char);
    }

    QString asciistr;
    for(int i=0;i<secondString.length ()/2;i++)
    {
        int ascii_char_int =secondString.mid (i,2).toInt (nullptr,16);
        QString ascii_char=QString(ascii_char_int);
        asciistr.append (ascii_char);
    }

    Current_msg.asciiStr=asciistr;
    Current_msg.binStr=binstr;
    Current_msg.index=recored_data_.size ()+1;
    setRecored_data(Current_msg);
    //Q_EMIT rw_InfoChanged(prase2QString(Current_msg));
}

void MTK_DataCenter::showterminal()
{
    qDebug()<<recored_data_.size ();
    if(recored_data_.isEmpty ())
    {
        return;
    }

    while(!recored_data_.isEmpty ())
    {
        data_.append (prase2QString(recored_data_.dequeue ()));
    }

    emit rwInfoChanged(data_);
    data_.clear ();
}

void MTK_DataCenter::save(QString msg)
{
    auto path
            = QFileDialog::getSaveFileName(Q_NULLPTR, tr("Export console data"),
                                           QDir::homePath(), tr("Text files") + " (*.txt)");

    if (!path.isEmpty())
    {
        QFile file(path);
        if (file.open(QFile::WriteOnly))
        {
            file.write(msg.toUtf8 ());
            file.close();
            Misc::Utilities::revealFile(path);
        }

        else
            Misc::Utilities::showMessageBox(tr("File save error"), file.errorString());
    }
}

void MTK_DataCenter::setRecored_data(const MTK_MSG &msg)
{
    recored_data_.append (msg);
    //emit rwInfoChanged(prase2QString(msg));

}

void MTK_DataCenter::clearRecored_data()
{
    recored_data_.clear ();
}

QString MTK_DataCenter::prase2QString(const MTK_MSG &mtk_msg)
{
    QString msg=QString("%1  %2  %3%4  %5%6")
            .arg (mtk_msg.index)
            .arg (mtk_msg.time)
            .arg (mtk_msg.type)
            .arg (NEWLINE)
            .arg (mtk_msg.hexStr)
            .arg (NEWLINE);
    return msg;
}

void MTK_DataCenter::samp_timeout()
{
    if(recored_data_.isEmpty ())
    {
        return;
    }

    for(int i=0;i<recored_data_.size ();i++)
    {

        if(recored_data_.isEmpty ())
        {
            continue;
        }
        QString lineMsg_;
        while(!recored_data_.isEmpty ())
        {
            lineMsg_.append (prase2QString(recored_data_.dequeue ()));
        }


        Q_EMIT rwInfoChanged(lineMsg_);
    }
}
