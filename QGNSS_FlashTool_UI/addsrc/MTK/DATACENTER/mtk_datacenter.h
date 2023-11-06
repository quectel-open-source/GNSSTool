#ifndef MTK_DATACENTER_H
#define MTK_DATACENTER_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QQueue>
#include <QThread>
#include <QEventLoop>
#include <QFileDialog>
#include "myUtilities.h"
#ifdef _WIN32
#define NEWLINE "\r\n"
#elif __linux__
#define NEWLINE "\n"
#elif __APPLE__
#define NEWLINE "\r"
#endif
class  MTK_DataCenter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MTK_MSG current_msg READ current_msg WRITE setCurrent_msg NOTIFY current_msgChanged)

public:
    explicit MTK_DataCenter(QObject *parent = nullptr);
            ~MTK_DataCenter();
    /**
     * @brief get_instance
     * single instance mode   correct order destruct with sharedpointer
     * @return
     **/
    static QSharedPointer<MTK_DataCenter> get_instance();

public:

    enum MsgStruct{
        Msg_index,
        Msg_time,
        Msg_type,
        Msg_hexStr,
        Msg_binStr,
        Msg_asciiStr
    };
    Q_ENUM (MsgStruct);

    typedef struct mtk_MGS
    {
        int index;
        QString time;
        QString type;
        QString hexStr;
        QString binStr;
        QString asciiStr;
        mtk_MGS()
        {
            index=0;
            time=QDateTime::currentDateTime ().toString ("hh:mm:ss.zzz");
            type="";
            hexStr="";
            binStr="";
            asciiStr="";
        }
    } MTK_MSG;

    const MTK_MSG &current_msg() const;

    void setRecored_data(const MTK_MSG &msg);
    void clearRecored_data();



     Q_INVOKABLE void showterminal();

     Q_INVOKABLE void save(QString msg=QString());
    void asyncsleep(int timeout)
    {
        QEventLoop loop;
        QTimer::singleShot (timeout,&loop,&QEventLoop::quit);
        loop.exec ();
    }

Q_SIGNALS:
    void current_msgChanged();

    void rwInfoChanged(const QString &str);


public Q_SLOTS:
    void setCurrent_msg(const MTK_MSG &newCurrent_msg);

    void setRw_Info(const QPair<QString, QString> &pair);

    void samp_timeout();

private:

    MTK_MSG current_msg_;

    //static qint64 msg_indx;

    /**
     * @brief
     *    recored_data_
     *    size of serials outside
     *    inside len of msg
     *    hash size struct
     */
    QQueue<MTK_MSG >  recored_data_;
    QString prase2QString(const MTK_MSG &mtk_msg);
    QString data_;

    QTimer lowsamptimer_; /*1hz*/
    QTimer highsamptimer_;/*10hz*/
};

Q_DECLARE_METATYPE(MTK_DataCenter::MTK_MSG);
#endif // MTK_DATACENTER_H
