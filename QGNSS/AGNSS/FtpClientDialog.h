#ifndef FTPCLIENTDIALOG_H
#define FTPCLIENTDIALOG_H

#include <QDialog>
#include <QStatusBar>
#include <QSettings>
#include <QUrl>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTreeWidgetItem>
#include "qftp.h"
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
//#include "gps_nmea.h"
#include "LC79D_AGNSS_LTO.h"
#include "common.h"
#include "DataParser/nmea_parser.h"

#define MTKBIN_3EPO_PKT_LNG 227

namespace Ui {
class FtpClientDialog;
}

typedef void (*p_sendAGPS)();

class FtpClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FtpClientDialog(QWidget *parent = nullptr);
    ~FtpClientDialog();

    void switch_agnss_online_offline_mode( bool mode);
    void sendAidingDataToModule(void);
private :
    void UpdateSizeAndCRC();
    int WriteLTOFileInfo();
    int ProgramFlashMemory();
    void downloadAgnssDataToLC79dModule();
    void downloadAgnssDataToLC29dModule();
    void downloadAgnssDataToL26DRModule();
    void downloadAgnssDataToL76ModuleHost();
    void downloadAgnssDataToL76Module();
    void sendTimeAndPositionAidingToLC79D();
    void sendTimeAndPositionAidingToLC29D();
    void sendTimeAndPositionAidingToL26DR();
    void sendTimeAndPositionAidingToL76();

 private slots:
    void connectOrDisconnect();
    void downloadFile();
    void cancelDownload();
    void connectToFtp();

    void ftpCommandFinished(int commandId, bool error);
    void addToList(const QUrlInfo &urlInfo);
    void processItem(QTreeWidgetItem *item, int column);
    void cdToParent();
    void updateDataTransferProgress(qint64 readBytes,
    qint64 totalBytes);
    void enableDownloadButton();
    void enableConnectButton();

    void on_cdToParentButton_clicked();

    void on_useCurrentLLApushButton_clicked();

    void on_radioButton_2_toggled(bool checked);

    void on_radioButton_toggled(bool checked);


    void on_saveAgnssFile_pushButton_clicked();

    void on_resetAndAiding_pushButtonfon_clicked();

    void on_downloadToModule_Button_clicked();
    void downloadToModule();
    void sendTimeAndPosition(uint32_t Index);

signals:
    void  sendData(const char * data, unsigned int size);

public:
    // bool        m_update_in_process = false; /* update process */
    int        m_update_percent = 0; /* update process */
    void setSolution_data(Solution_t *value);

private:
    Ui::FtpClientDialog         *ui;

private:

    LTO_Options                 ltoOptions={0,0};
    uint                        crc32 = 0;
    uint32_t                    fileSize = 0;



    QFtp	                    *m_pFtp=nullptr;
    QNetworkSession             *m_pNetworkSession=nullptr;
    QNetworkConfigurationManager m_Manager;

    QHash<QString, bool>         m_bIsDirectory;
    QString                      m_strCurrentPath;
    QFile                       *m_pFile=nullptr;

    /* file download process info */
    QProgressDialog             *m_pProgressDialog=nullptr; /* process bar */
    qint64                      m_file_total_size=0;

    QFile      *m_storeAgnssFile=nullptr;
    QString     m_storeAgnssFileDir=nullptr;         //file save
    bool        m_storeStatus = false;

    QStatusBar *statusBar;
    Solution_t *solution_data;
};

#endif // FTPCLIENTDIALOG_H
