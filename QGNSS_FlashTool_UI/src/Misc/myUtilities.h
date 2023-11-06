
#pragma once

#include <QObject>
#include <QMessageBox>
#include <QApplication>
#include<QFileDialog>
#include <QStyle>
#include <QDebug>
#include <QPalette>
#include <QIcon>
namespace Misc
{
/**
 * @brief The Utilities class
 *
 * The @c Utilitities module provides commonly used functionality to the rest of the
 * application. For example, showing a messagebox with a nice format or revealing files in
 * the operating system's preffered file manager.
 */
class Utilities : public QObject
{
    Q_OBJECT

public:
    // clang-format off
 static QSharedPointer<Utilities> instance();
    static void rebootApplication();
    Q_INVOKABLE bool askAutomaticUpdates();

    //clang-format on
    static QString getDir();
public Q_SLOTS:
    static void aboutQt();
    static void revealFile(const QString& pathToReveal);
    static int showMessageBox(const QString &text,
                              const QString &informativeText = "",
                              const QString &windowTitle = "",
                              const QMessageBox::StandardButtons &bt = QMessageBox::Ok);

     static QString getFile();

 static void revealexe(const QString& pathToReveal);
};
}
