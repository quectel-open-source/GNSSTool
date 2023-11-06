#include "convert_kml.h"
#include "ui_convert_kml.h"
#include "runmachine.h"
#include "common.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QTextStream>
#include <QDebug>

convert_KML::convert_KML(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::convert_KML)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Convert KML");
}

convert_KML::~convert_KML()
{
    delete ui;
}

bool convert_KML::extractNMEA(QString srcFileName, QString outputFileName)
{
    QFile srcFile(srcFileName);
    QFile outFile(outputFileName);
    QTextStream outStream(&outFile);
    QByteArray curSte;curSte.clear();
    QSet<QString> NMEA_prefix{"RMC", "GGA", "GLL", "GSV", "GSA", "VTG"};
    if(srcFile.open(QIODevice::ReadOnly)&&outFile.open(QIODevice::WriteOnly)){
        foreach (auto c, srcFile.readAll()) {
            if (curSte.length() != 0)
            {
                // NMEA Msg
                if (curSte.at(0) == '$')
                {
                    if (c == '\n' && curSte.length() > 6)
                    {
                        curSte.append(c);
                        outStream<<curSte;
                        curSte.clear();
                    }
                    else
                    {
                        curSte.append(c);
                        if (curSte.length() == 6)
                        {
                            if (!NMEA_prefix.contains(curSte.mid(3,3)))
                            {
                                qDebug() << curSte << "---is not a nmea statement prefix.";
                                curSte.clear();
                            }
                        }
                        // over length
                        if (curSte.length() > 1024)
                        {
                            curSte.clear();
                        }
                    }
                }
            }
            else if (c == '$')
            {
                curSte.append(c);
            }
        }
        return true;
    }
    return false;
}

void convert_KML::on_inputFileNameBrowseBtn_clicked()
{
    QSettings settings(SETTINGS_FILE_NAME, QSettings::IniFormat);
    QString startFile = settings.value("KML_INPUT_PATH", "./").toString();
    QStringList userList = QFileDialog::getOpenFileNames(nullptr, tr("Select one or more input files"), startFile);
    inputFileNames_ = userList;
    if (!userList.empty()) {
      QFileInfo finfo(userList[0]);
      startFile = finfo.dir().absolutePath();
      settings.setValue("KML_INPUT_PATH", startFile);
      ui->outputFileNameText->setText(startFile + "/" + finfo.baseName() + ".kml");
      outputFileName_ = ui->outputFileNameText->text();
      QString str = userList.takeFirst();
      if(!userList.empty())
      {
          foreach (auto v, userList) {
              str+=";";
              str+=v;
          }
      }
      ui->inputFileNameText->setText(str);
    }
}

bool convert_KML::runGpsbabel(const QStringList &args, QString &errorString, QString &outputString)
{
    QString name = "gpsbabel";
    QString program = QApplication::applicationDirPath() + '/' + name;
    RunMachine runMachine(this, program, args);
    int retStatus = runMachine.exec();

    errorString = runMachine.getErrorString();
    outputString = runMachine.getOutputString();
    return retStatus;
}

bool convert_KML::isOkToGo()
{
    if ((ui->inputFileNameText->text().isEmpty()) &&
        (inputFileNames_.empty())) {
      QMessageBox::information(nullptr, QString("Info"), tr("No input file specified."));
      return false;
    }
    if ((ui->outputFileNameText->text().isEmpty()) &&
        (outputFileName_.isEmpty())) {
      QMessageBox::information(nullptr, QString("Info"), tr("No output file specified."));
      return false;
    }
    foreach (auto var, inputFileNames_) {
        if(!extractNMEA(var, var+".txt")){
            QMessageBox::information(nullptr, QString("Info"), tr("Failed to generate a pure NMEA file."));
            return false;
        }
    }
    return true;
}

void convert_KML::on_buttonBox_accepted()
{
    outputFileName_ = ui->outputFileNameText->text();
    //TODO: isOkToGo?
    if(!isOkToGo())
        return;
    //gpsbabel -w -t -i nmea -f E:/排查问题/KML/None-0908_155929_COM3.log -f E:/排查问题/KML/single.log -o kml -F D:/None-0908_155929_COM3.kml
    QStringList args;
    args << "-w" << "-t"<< "-i"<< "nmea";
    foreach (auto var, inputFileNames_) {
        args << "-f";
        args << var + ".txt";
    }
    args <<"-o";
    args <<"kml";
    args <<"-F";

    args <<outputFileName_;

    QString errorString;
    QString outputString;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool x = runGpsbabel(args, errorString, outputString);
    QApplication::restoreOverrideCursor();
    if (x){
        ui->outputWindow->appendPlainText(tr("Convert completed."));
    }
    else {
        ui->outputWindow->appendPlainText(tr("Error : %1.\n").arg(errorString));
    }
}

void convert_KML::on_outputFileNameBrowseBtn_clicked()
{
    QString str = QFileDialog::getSaveFileName(this, tr("Output File Name"),QString(),tr("KML files (*.kml)"));
    if (str.length() != 0) {
      ui->outputFileNameText->setText(str);
      outputFileName_ = str;
    }
}

void convert_KML::on_buttonBox_rejected()
{
    close();
}
