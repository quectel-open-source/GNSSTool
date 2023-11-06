#include "mainwindow.h"
#include "startupwidget.h"
#include "about.h"

#include <QDateTime>

#define QGNSS_LOGER_NAME "QGNSS"
#define QGNSS_LOG_FILE_NAME "./logFile/QGNSS(%1).log"
#define LOG_FILE_MAX_SIZE 1024 * 1024 * 200 // 200MB
#define LOG_MAX_FILES 5

uint32_t getCurrentInstanceNumber()
{
    QString appName(QCoreApplication::applicationName());
    QProcess process;
    process.start("tasklist");
    process.waitForFinished();
    QString tasksList = QString::fromLocal8Bit(process.readAllStandardOutput());
    return tasksList.count(appName);
}

void logHander(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    auto spdlog = spdlog::get(QGNSS_LOGER_NAME);
    //    QByteArray localMsg = msg.toLocal8Bit();
    //    QByteArray time = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss").toLocal8Bit();
    switch (type)
    {
    case QtDebugMsg:
        spdlog->debug(msg.toStdString());
        break;
    case QtInfoMsg:
        spdlog->info(msg.toStdString());
        break;
    case QtWarningMsg:
        spdlog->warn(msg.toStdString());
        break;
    case QtCriticalMsg:
        spdlog->critical(msg.toStdString());
        break;
    case QtFatalMsg:
        spdlog->error(msg.toStdString());
        break;
    }
}

int main(int argc, char *argv[])
{
    // create directory
    QDir dir("./");
    if (!dir.exists("./logFile"))
    {
        dir.mkdir("./logFile");
    }
    if (!dir.exists("./AGNSS_File"))
    {
        dir.mkdir("./AGNSS_File");
    }
    // Set up software information
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Quectel");
    QCoreApplication::setOrganizationDomain("quectel.com");
    QCoreApplication::setApplicationName("QGNSS");
    // start-up logo
    startupWidget sw;
    sw.show();
    qgnss_sleep(1288);
    sw.close();
    // main window
    MainWindow w;
//    w.setWindowTitle(QString("QGNSS(%1)_V").arg(getCurrentInstanceNumber()) + QString(App_mainVersion));
#ifdef BETA
    w.setWindowTitle(QString("QGNSS V%1.%2-Beta%3")
                         .arg(App_mainVersion)
                         .arg(App_subVersion)
                         .arg(buildDateTime().toString("MMdd")));
#else
    w.setWindowTitle(QString("QGNSS V") + QString(App_mainVersion));
#endif
    w.setAttribute(Qt::WA_QuitOnClose, true);
    w.show();
    QCoreApplication::processEvents();
    w.delayTask();
    // Install log handler
#ifdef OUTPUT_LOG_TO_FILE
    auto spdlog = spdlog::rotating_logger_st(QString(QGNSS_LOGER_NAME).toStdString(),
                                             QString(QGNSS_LOG_FILE_NAME).arg(getCurrentInstanceNumber()).toStdString(),
                                             LOG_FILE_MAX_SIZE,
                                             LOG_MAX_FILES);
    spdlog::set_level(spdlog::level::debug);
    qInstallMessageHandler(logHander);
#endif
    spdlog::flush_every(std::chrono::seconds(1));
    return a.exec();
}
