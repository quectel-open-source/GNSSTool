#include "ftp_client.h"
#include "ui_ftp_client.h"
#include "QMessageBox"
#include "QDir"

FTP_Client::FTP_Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FTP_Client)
{
    ui->setupUi(this);
    m_pFtp = nullptr;
    m_pProgressDialog = nullptr;
    ui->fileList->setEnabled(false);
    ui->fileList->setRootIsDecorated(false);
    ui->fileList->setHeaderLabels(QStringList() << tr("Name") << tr("Size") << tr("Owner") << tr("Group") << tr("Time"));
    ui->fileList->header()->setStretchLastSection(true);
    ui->fileList->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->connectButton->setDefault(true);
    ui->cdToParentButton->setVisible(false);
    ui->cdToParentButton->setEnabled(false);
    ui->downloadButton->setEnabled(false);

    connect(ui->fileList, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(processItem(QTreeWidgetItem*, int)));
    connect(ui->fileList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(enableDownloadButton()));
    connect(m_pProgressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectOrDisconnect()));
    connect(ui->cdToParentButton, &QPushButton::clicked, this, &FTP_Client:: cdToParent);
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(downloadFile()));
}

FTP_Client::~FTP_Client()
{
    delete ui;
    delete m_pProgressDialog;
    delete m_pFtp;
    delete m_pNetworkSession;
    delete m_pFile;
}

void FTP_Client::setFTP_info(QString user, QString pwd)
{
    ui->edtUser->setText(user);
    ui->edtPassword->setText(pwd);
}

void FTP_Client::setFTP_addr(QString addr)
{
    ui->ftpServerLineEdit->setText(addr);
}

void FTP_Client::connectOrDisconnect()
{
    if (m_pFtp) {
        m_pFtp->abort();
        m_pFtp->deleteLater();
        m_pFtp = nullptr;

        ui->fileList->setEnabled(false);
        //ui->cdToParentButton->setEnabled(false);
        ui->downloadButton->setEnabled(false);
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText(tr("Connect"));
        setCursor(Qt::ArrowCursor);
        ui->status_plainTextEdit->clear();
//        ui->status_plainTextEdit->insertPlainText(tr("Please enter the name of an FTP server."));
        return;
    }

    if (!m_pNetworkSession || !m_pNetworkSession->isOpen()) {
        if (m_Manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
            if (!m_pNetworkSession) {
                // Get saved network configuration
                QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
                settings.beginGroup(QLatin1String("QtNetwork"));
                const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
                settings.endGroup();

                // If the saved network configuration is not currently discovered use the system default
                QNetworkConfiguration config = m_Manager.configurationFromIdentifier(id);
                if ((config.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
                    config = m_Manager.defaultConfiguration();
                }

                m_pNetworkSession = new QNetworkSession(config, this);
                connect(m_pNetworkSession, SIGNAL(opened()), this, SLOT(connectToFtp()));
                connect(m_pNetworkSession, SIGNAL(error(Qm_pNetworkSession::SessionError)), this, SLOT(enableConnectButton()));
            }

            m_pNetworkSession->open();

            return;
        }
    }
    connectToFtp();
}

void FTP_Client::downloadFile()
{
    QString fileName;
    fileName = ui->fileList->currentItem()->text(0);


    m_pFile = new QFile("./AGNSS_File/"+fileName);


    if (!m_pFile->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("FTP"),
        tr("Unable to save the file %1: %2.")
        .arg(fileName).arg(m_pFile->errorString()));
        delete m_pFile;
        return;
    }

    m_pFtp->get(ui->fileList->currentItem()->text(0), m_pFile);

    if(m_pProgressDialog == nullptr) {
        m_pProgressDialog = new QProgressDialog;
    }
    m_pProgressDialog->setLabelText(tr("Downloading %1...").arg(fileName));
    ui->downloadButton->setEnabled(false);
    m_pProgressDialog->exec();
}

void FTP_Client::cancelDownload()
{
    if(m_pFtp != nullptr) {
        m_pFtp->abort();

        if (m_pFile->exists()) {
            m_pFile->close();
            m_pFile->remove();
        }
        delete m_pFtp;
    }
}

void FTP_Client::connectToFtp()
{
    m_pFtp = new QFtp(this);
//    m_pFtp->setTransferMode(QFtp::Passive);
    QString address = ui->ftpServerLineEdit->text();
    QString userName = ui->edtUser->text();
    QString password = ui->edtPassword->text();
    if(address.isEmpty() || userName.isEmpty() || password.isEmpty()){
        ui->status_plainTextEdit->clear();
        ui->status_plainTextEdit->insertPlainText("Please enter complete FTP server information!");
        return;
    }

    connect(m_pFtp, SIGNAL(commandFinished(int,bool)), this, SLOT(ftpCommandFinished(int,bool)));
    connect(m_pFtp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
    connect(m_pFtp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(updateDataTransferProgress(qint64,qint64)));

    ui->fileList   ->clear();
    m_strCurrentPath.clear();
    m_bIsDirectory.clear();

    QUrl url(ui->ftpServerLineEdit->text());
    if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp")) {
        qDebug()<<"FTP address:"<<address;
        m_pFtp->connectToHost(address , 21);
        qDebug()<<"FTP userName:"<<userName;
        m_pFtp->login(userName,
                      password);
    } else {
        m_pFtp->connectToHost(url.host(), (quint16)url.port(21));
        if (!url.userName().isEmpty())
            m_pFtp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
        else
            m_pFtp->login();
        if (!url.path().isEmpty())
            m_pFtp->cd(url.path());
    }
    ui->fileList->setEnabled(true);
    ui->connectButton->setEnabled(false);
    ui->connectButton->setText(tr("Disconnect"));
    ui->status_plainTextEdit->clear();
    ui->status_plainTextEdit->insertPlainText(tr("Connecting to FTP server %1...").arg(ui->ftpServerLineEdit->text()));
}

void FTP_Client::processItem(QTreeWidgetItem *item, int column)
{
    QString name = item->text(0);
    m_storeAgnssFileDir =name;
//    ui->angssFileDir_lineEdit->setText(m_storeAgnssFileDir);

    if (m_bIsDirectory.value(name)) {
        ui->fileList->clear();
        m_bIsDirectory.clear();
        m_strCurrentPath += '/';
        m_strCurrentPath += name;
        m_pFtp->cd(name);
        m_pFtp->list();
        ui->cdToParentButton->setEnabled(true);
        ui->cdToParentButton->setVisible(true);
        setCursor(Qt::WaitCursor);
        return;
    }
}

void FTP_Client::enableDownloadButton()
{
    QTreeWidgetItem *current = ui->fileList->currentItem();
    if (current) {
        QString currentFile = current->text(0);
        ui->downloadButton->setEnabled(!m_bIsDirectory.value(currentFile));
    } else {
        ui->downloadButton->setEnabled(false);
    }
}

void FTP_Client::ftpCommandFinished(int commandId, bool error)
{
    setCursor(Qt::ArrowCursor);

    if (m_pFtp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::information(this, tr("FTP"),
            tr("Unable to connect to the FTP server "
            "at %1. Please check that the host "
            "name is correct.")
            .arg(ui->ftpServerLineEdit->text()));
            connectOrDisconnect();
            return;
        }
        ui->status_plainTextEdit->clear();
        ui->status_plainTextEdit->insertPlainText(tr("Logged onto %1.").arg(ui->ftpServerLineEdit->text()));

        ui->fileList->setFocus();
        ui->downloadButton->setDefault(true);
        ui->connectButton->setEnabled(true);
        return;
    }
    if (m_pFtp->currentCommand() == QFtp::Login)
    {
        qDebug()<<"QFtp::Login"<<error;
        if(error == false)
            m_pFtp->list();
    }
    if (m_pFtp->currentCommand() == QFtp::Get) {
        if (error) {
            ui->status_plainTextEdit->clear();
            ui->status_plainTextEdit->insertPlainText(tr("Canceled download of %1.").arg(m_pFile->fileName()));
            m_pFile->close();
            m_pFile->remove();
        } else {

            ui->status_plainTextEdit->clear();
            ui->status_plainTextEdit->insertPlainText(tr("Downloaded  to directory %1.").arg(m_pFile->fileName()));
            m_pFile->close();

            qDebug()<<"FTP FILE: File size:"<<m_pFile->size()<<" Need download file size:"<<m_file_total_size;

            if(m_pFile->size() < m_file_total_size)
            {
                m_pFile->remove();
                ui->status_plainTextEdit->insertPlainText(tr("File wasn't completely downloaded from FTP sever,delete %1.").arg(m_pFile->fileName()));
            }
        }
        enableDownloadButton();
        m_pProgressDialog->hide();

    }
    if (m_pFtp->currentCommand() == QFtp::List) {
        qDebug()<<"QFtp::List"<<error;
        if (m_bIsDirectory.isEmpty()) {
            ui->fileList->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
            ui->fileList->setEnabled(false);
        }
    }
}

void FTP_Client::addToList(const QUrlInfo &urlInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, urlInfo.name());
    item->setText(1, QString::number(urlInfo.size()));
    item->setText(2, urlInfo.owner());
    item->setText(3, urlInfo.group());
    item->setText(4, urlInfo.lastModified().toString("yyyy-MM-dd hh:mm"));

    QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
    item->setIcon(0, pixmap);

    m_bIsDirectory[urlInfo.name()] = urlInfo.isDir();
    ui->fileList->addTopLevelItem(item);
    if (!ui->fileList->currentItem()) {
        ui->fileList->setCurrentItem(ui->fileList->topLevelItem(0));
        ui->fileList->setEnabled(true);
    }
}

void FTP_Client::cdToParent()
{
  if(m_pFtp->state ()!=QFtp::State::LoggedIn)
  {
    return;
  }
  setCursor(Qt::WaitCursor);
  ui->fileList->clear();
  m_bIsDirectory.clear();
  m_strCurrentPath = m_strCurrentPath.left(m_strCurrentPath.lastIndexOf('/'));
  if (m_strCurrentPath.isEmpty()) {
    ui->cdToParentButton->setEnabled(false);
    m_pFtp->cd("/");
  } else {
    m_pFtp->cd(m_strCurrentPath);
  }
  m_pFtp->list();
}

void FTP_Client::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    m_pProgressDialog->setMaximum(totalBytes);
    m_pProgressDialog->setValue(readBytes);

    m_file_total_size = totalBytes;
    qDebug()<<"m_file_total_size need get is"<<m_file_total_size;
}
