#include "console.h"
#include "ui_console.h"
#include "QDebug"
#include "QDateTime"
#include "RTKLib/rtklib.h"

Console::Console(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Console)
{
    ui->setupUi(this);
    ui->plainTextEdit->setMaximumBlockCount(1024*40);
    ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
}

Console::~Console()
{
    delete ui;
    qInfo()<<"Delete console "<< this->windowTitle();
}

void Console::displayHex(QByteArray data)
{
    if(ui->pushButton_pause->isChecked())
        return;
    QByteArray displayString;
    if((uint8_t)data.at(0) == 0xd3){
        displayString.append(
                    QString("RTCM3:%1    Size:%2")
                    .arg(getbitu((uint8_t *)data.constData(),24,12))
                    .arg(data.size()));
    }
    displayString.append("\r\n");
    QByteArray lineString;
    for (int i = 0; i <= data.size() / 16; i++) {
        QByteArray lineHex = data.mid(16*i,16);
        QByteArray lineHexStr = data.mid(16*i,16).toHex(' ').toUpper();
        lineString.append(QString("").sprintf("%04X    %-48s",i,lineHexStr.data()));
        lineString.append("    ");
        displayString.append(lineString+"\r\n");
        lineString.clear();
    }
    outputString(displayString);
}

void Console::displayText(QByteArray data)
{
    if(ui->pushButton_pause->isChecked())
        return;
    QString putString = data;
    if(ui->pushButton_timeStamp->isChecked())
        putString = QDateTime::currentDateTime().toString("[MM-dd hh:mm:ss.zzz]") + data;
    if(ui->comboBox_filter->currentText ().isEmpty()){
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        ui->plainTextEdit->insertPlainText(putString);
    }
    else if(data.contains(ui->comboBox_filter->currentText ().toLocal8Bit())){
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        ui->plainTextEdit->insertPlainText(putString);
    }
//    outputString(data);
}

void Console::displayDump(QByteArray data)
{
    if(ui->pushButton_pause->isChecked())
        return;
    QByteArray lineString;
    QByteArray displayString("\r\n");
    for (int i = 0; i <= data.size() / 16; i++) {
        QByteArray lineHex = data.mid(16*i,16);
        QByteArray lineHexStr = data.mid(16*i,16).toHex(' ').toUpper();
        lineString.append(QString("").sprintf("%04X|%-48s",i,lineHexStr.data()));
        lineString.append("|");
        lineString.append(getAsciiRepresentation(lineHex));
        displayString.append(lineString+"\r\n");
        lineString.clear();
    }
    outputString(displayString);
}

void Console::on_clearText_clicked()
{
    ui->plainTextEdit->clear();
    ui->comboBox_filter->setCurrentText("");
}

void Console::outputString(QByteArray &str)
{
    QString putString = str;
    if(ui->pushButton_timeStamp->isChecked())
        putString = QDateTime::currentDateTime().toString("[MM-dd hh:mm:ss.zzz]") + str;
    if(ui->comboBox_filter->currentText ().isEmpty()){
//        ui->plainTextEdit->moveCursor(QTextCursor::End);
//        ui->plainTextEdit->insertPlainText(putString);
//        ui->plainTextEdit->appendHtml(putString);
        ui->plainTextEdit->appendPlainText(putString);
    }
    else if(str.contains(ui->comboBox_filter->currentText ().toLocal8Bit())){
//        ui->plainTextEdit->moveCursor(QTextCursor::End);
//        ui->plainTextEdit->insertPlainText(putString);
        ui->plainTextEdit->appendPlainText(putString);
    }
}

QString Console::getAsciiRepresentation(QByteArray &array) const
{
    //    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    //    str = codec->toUnicode(array);

        for(char &c : array){
    //        const bool printAsIs = c.isLetterOrNumber() ||
    //                c.isPunct();

            if(c > 126 || c < 33){
                c = '.';
            }
        }
        QString str(array);
        return str;
}
