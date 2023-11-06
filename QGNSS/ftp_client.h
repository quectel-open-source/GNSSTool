#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include <QWidget>
#include <QProgressDialog>
#include <QTreeWidgetItem>
#include "qftp.h"
#include "QNetworkSession"
#include <QNetworkConfigurationManager>
#include "QSettings"
#include "QFile"

namespace Ui {
class FTP_Client;
}

class FTP_Client : public QWidget
{
    Q_OBJECT

public:
    explicit FTP_Client(QWidget *parent = nullptr);
    ~FTP_Client();
    void setFTP_info(QString user,QString pwd);
    void setFTP_addr(QString addr);

private:
    Ui::FTP_Client *ui;
    QProgressDialog *m_pProgressDialog=nullptr;
    qint64 m_file_total_size = 0;
    QFtp *m_pFtp=nullptr;
    QHash<QString, bool> m_bIsDirectory;
    QString m_strCurrentPath;
    QString m_storeAgnssFileDir=nullptr;
    QNetworkSession *m_pNetworkSession=nullptr;
    QNetworkConfigurationManager m_Manager;
    QFile *m_pFile=nullptr;

private slots:
   void connectOrDisconnect();
   void downloadFile();
   void cancelDownload();
   void connectToFtp();
   void cdToParent();
   void processItem(QTreeWidgetItem *item, int column);
   void enableDownloadButton();
   void ftpCommandFinished(int commandId, bool error);
   void addToList(const QUrlInfo &urlInfo);
   void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);
};

#endif // FTP_CLIENT_H
