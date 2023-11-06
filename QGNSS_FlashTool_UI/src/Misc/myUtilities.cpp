
#include <QDir>
#include <QUrl>
#include <QPalette>
#include <QProcess>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QAbstractButton>
#include <QDesktopServices>

#include "AppInfo.h"
#include "myUtilities.h"
namespace Misc{
/**
 * Returns a pointer to the only instance of the class
 */
QSharedPointer<Utilities> Utilities::instance()
{
  static QSharedPointer<Utilities> singleton=QSharedPointer<Utilities>(new Utilities);
  return singleton;
}

/**
 * Restarts the application - with macOS specific code to make it work
 */
void Utilities::rebootApplication()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->exit();
    QProcess::startDetached(qApp->arguments().first(), qApp->arguments());
#else
#    ifdef Q_OS_MAC
    auto bundle = qApp->applicationDirPath() + "/../../";
    QProcess::startDetached("open", { "-n", "-a", bundle });
#    else
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
#    endif
    qApp->exit();
#endif
}

/**
 * Asks the user if he/she wants the application to check for updates automatically
 */
bool Utilities::askAutomaticUpdates()
{
    const int result = showMessageBox(tr("Check for updates automatically?"),
                                      tr("Should %1 automatically check for updates? "
                                         "You can always check for updates manually from "
                                         "the \"Help\" menu")
                                          .arg(APP_NAME),
                                      APP_NAME, QMessageBox::Yes | QMessageBox::No);
    return result == QMessageBox::Yes;
}

/**
 * Shows a macOS-like message box with the given properties
 */
int Utilities::showMessageBox(const QString &text, const QString &informativeText,
                                    const QString &windowTitle,
                                    const QMessageBox::StandardButtons &bt)
{
    QMessageBox box;
    box.setAutoFillBackground (true);
   // box.setIcon (QMessageBox::Icon::Warning);
    box.setStandardButtons(bt);
    box.setWindowTitle(windowTitle);
    box.setText("<h3>" + text + "</h3>");
    box.setInformativeText(informativeText);

    // Add button translations
    if (bt & QMessageBox::Ok)
        box.button(QMessageBox::Ok)->setText(tr("Ok"));
    if (bt & QMessageBox::Save)
        box.button(QMessageBox::Save)->setText(tr("Save"));
    if (bt & QMessageBox::SaveAll)
        box.button(QMessageBox::SaveAll)->setText(tr("Save all"));
    if (bt & QMessageBox::Open)
        box.button(QMessageBox::Open)->setText(tr("Open"));
    if (bt & QMessageBox::Yes)
        box.button(QMessageBox::Yes)->setText(tr("Yes"));
    if (bt & QMessageBox::YesToAll)
        box.button(QMessageBox::YesToAll)->setText(tr("Yes to all"));
    if (bt & QMessageBox::No)
        box.button(QMessageBox::No)->setText(tr("No"));
    if (bt & QMessageBox::NoToAll)
        box.button(QMessageBox::NoToAll)->setText(tr("No to all"));
    if (bt & QMessageBox::Abort)
        box.button(QMessageBox::Abort)->setText(tr("Abort"));
    if (bt & QMessageBox::Retry)
        box.button(QMessageBox::Retry)->setText(tr("Retry"));
    if (bt & QMessageBox::Ignore)
        box.button(QMessageBox::Ignore)->setText(tr("Ignore"));
    if (bt & QMessageBox::Close)
        box.button(QMessageBox::Close)->setText(tr("Close"));
    if (bt & QMessageBox::Cancel)
        box.button(QMessageBox::Cancel)->setText(tr("Cancel"));
    if (bt & QMessageBox::Discard)
        box.button(QMessageBox::Discard)->setText(tr("Discard"));
    if (bt & QMessageBox::Help)
        box.button(QMessageBox::Help)->setText(tr("Help"));
    if (bt & QMessageBox::Apply)
        box.button(QMessageBox::Apply)->setText(tr("Apply"));
    if (bt & QMessageBox::Reset)
        box.button(QMessageBox::Reset)->setText(tr("Reset"));
    if (bt & QMessageBox::RestoreDefaults)
        box.button(QMessageBox::RestoreDefaults)->setText(tr("Restore defaults"));

    // Show message box & return user decision to caller
    return box.exec();
}

QString Utilities::getFile()
{
  QString fileName = QFileDialog::getOpenFileName(nullptr,
         tr("select file"), "./", tr(" file (*.bin)"));
  return fileName;
}

QString Utilities::getDir()
{
  return QFileDialog::getExistingDirectory(nullptr, "选择目录", "./", QFileDialog::ShowDirsOnly);
}

/**
 * Displays the about Qt dialog
 */
void Utilities::aboutQt()
{
    qApp->aboutQt();
}

/**
 * Reveals the file contained in @a pathToReveal in Explorer/Finder.
 * On GNU/Linux, this function shall open the file directly with the desktop
 * services.
 *
 * Hacking details:
 * http://stackoverflow.com/questions/3490336/how-to-reveal-in-finder-or-show-in-explorer-with-qt
 */
void Utilities::revealFile(const QString &pathToReveal)
{
#if defined(Q_OS_WIN)
    QStringList param;
    const QFileInfo fileInfo(pathToReveal);
    if (!fileInfo.isDir())
        param += QLatin1String("/select,");
    param += QDir::toNativeSeparators(fileInfo.canonicalFilePath());
    QProcess::startDetached("explorer.exe", param);
#elif defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1(
                      "tell application \"Finder\" to reveal POSIX file \"%1\"")
                      .arg(pathToReveal);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(pathToReveal));
#endif
}

void Utilities::revealexe(const QString &pathToReveal)
{
  QProcess::startDetached(pathToReveal);
}


}
