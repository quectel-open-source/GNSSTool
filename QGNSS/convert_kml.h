#ifndef CONVERT_KML_H
#define CONVERT_KML_H

#include <QDialog>

namespace Ui {
class convert_KML;
}

class convert_KML : public QDialog
{
    Q_OBJECT

public:
    explicit convert_KML(QWidget *parent = nullptr);
    ~convert_KML();

    static bool extractNMEA(QString srcFileName, QString outputFileName);

private slots:
    void on_inputFileNameBrowseBtn_clicked();

    void on_buttonBox_accepted();

    void on_outputFileNameBrowseBtn_clicked();

    void on_buttonBox_rejected();

private:
    Ui::convert_KML *ui;
    QStringList inputFileNames_;
    QString outputFileName_;
    bool runGpsbabel(const QStringList& args, QString& errorString, QString& outputString);
    bool isOkToGo();
};

#endif // CONVERT_KML_H
