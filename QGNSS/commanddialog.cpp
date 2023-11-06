/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         commanddialog.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-09-22      Dallas.xu
* ***************************************************************************/
#include "commanddialog.h"
#include "ui_commanddialog.h"
#include "QMessageBox"

commandDialog::commandDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::commandDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->checkBox_DTR->setHidden(true);
    ui->checkBox_RTS->setHidden(true);
    g_checkBox = 1;
    /* NTRIP client timer */
    connect(&m_cycleTimer,SIGNAL(timeout()),this,SLOT(cycleTimer_Timeout()));
    for(int i=0; i<14; ++i)
    {
        on_pushButton_add_clicked();
    }
}

commandDialog::~commandDialog()
{
    delete ui;
}

void commandDialog::update_command_cycle_list()
{
    m_lineEdit_List          .clear();
    m_HexCheckBox_List       .clear();
    m_CRLFCheckBox_List      .clear();
    m_delayTime_lineEdit_List.clear();

    QList<QCheckBox *> cycleCheckBox_l         = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("cycle_checkBox");
    QList<QLineEdit *> lineEdit_l              = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
    QList<QCheckBox *> hexCheckBox_l           = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_Hex");
    QList<QCheckBox *> checkBoxCRLF_l          = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_CRLF");
    QList<QLineEdit *> delayTime_lineEdit_List = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("delayTime_lineEdit");


    for(int i=0; i<ui->verticalLayout->count(); ++i)
    {
        if(cycleCheckBox_l[i]->checkState())
        {
            m_lineEdit_List          .append(lineEdit_l[i]);
            m_HexCheckBox_List       .append(hexCheckBox_l[i]);
            m_CRLFCheckBox_List      .append(checkBoxCRLF_l[i]);
            m_delayTime_lineEdit_List.append(delayTime_lineEdit_List[i]);
        }
    }
    qDebug()<<"checkBox chec count"<<m_lineEdit_List.count();

    for(int i=0; i<m_lineEdit_List.count(); i++)
    {
        qDebug()<<"lineedit checked text :"<<m_lineEdit_List.at(i)->text();
    }
}

void commandDialog::qsettingWrite()
{    

    QList<QCheckBox *> cycleCheckBox_l      = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("cycle_checkBox");
    QList<QLineEdit *> lineEdit_l           = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
    QList<QCheckBox *> hexCheckBox_l        = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_Hex");
    QList<QCheckBox *> checkBoxCRLF_l       = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_CRLF");
    QList<QLineEdit *> delayTime_lineEdit_l = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("delayTime_lineEdit");
    QDateTime datetime;
    QString timestr=datetime.currentDateTime().toString("yyyy-MM-dd-HH-mm-ss");
    QString m_Path;
    bool flag = false;
    for(int i=0; i<ui->verticalLayout->count(); ++i){
        if(lineEdit_l[i]->text() != ""){
            flag = true;
        }
    }
    if(flag){
        m_Path = QFileDialog::getSaveFileName(this, tr("Save As"), "./config/commandConsole.ini", tr("Files (*.*)"));
        if(!m_Path.isEmpty()){
            QSettings settings(m_Path, QSettings::IniFormat);
            QString cycleCheckBox_tmp;
            QString hexCheckBox_tmp;
            QString checkBoxCRLF_tmp;
            settings.beginWriteArray("projectConfiguration");
            for (int i=0; i<ui->verticalLayout->count(); i++)
            {
                settings.setArrayIndex(i);
                if(cycleCheckBox_l[i]->checkState())
                {
                    cycleCheckBox_tmp = "1";
                }else{
                    cycleCheckBox_tmp = "0";
                }
                settings.setValue("cycle_checkBox_statu",cycleCheckBox_tmp);
                settings.setValue("linetext", lineEdit_l[i]->text());
                if(hexCheckBox_l[i]->checkState())
                {
                    hexCheckBox_tmp = "1";
                }else{
                    hexCheckBox_tmp = "0";
                }
                settings.setValue("hex_checkBox_statu",hexCheckBox_tmp);
                if(checkBoxCRLF_l[i]->checkState())
                {
                    checkBoxCRLF_tmp = "1";
                }else{
                    checkBoxCRLF_tmp = "0";
                }
                settings.setValue("checkBox_CRLF_statu",checkBoxCRLF_tmp);
                settings.setValue("delayTime_linetext",delayTime_lineEdit_l[i]->text());
            }
            settings.endArray();
            QString path = QDir::currentPath();
            QMessageBox::information(this,"Success",path+m_Path+" File saved successfully.");

        }
    }else{
        QMessageBox::warning(this,"Warning","Unable to save blank file!");
    }

}

void commandDialog::qsettingINIlength()
{
    QString     replayFileDir=nullptr;
    replayFileDir = QFileDialog::getOpenFileName(this,tr("Select File to load configuration")
                                                ,"./config",tr("json Files(*.ini);;cfg Files(*.cfg);;All File(*)"));
    if(!replayFileDir.isEmpty())
    {
        QSettings settings(replayFileDir, QSettings::IniFormat);
        QList<QCheckBox *> cycleCheckBox_l      = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("cycle_checkBox");
        QList<QLineEdit *> lineEdit_l           = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
        QList<QCheckBox *> hexCheckBox_l        = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_Hex");
        QList<QCheckBox *> checkBoxCRLF_l       = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_CRLF");
        QList<QLineEdit *> delayTime_lineEdit_l = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("delayTime_lineEdit");
        int size = settings.beginReadArray("projectConfiguration");
        if(size>ui->verticalLayout->count())
        {
            int num_tmp = size - ui->verticalLayout->count();
            for(int i=0; i<num_tmp; ++i)
            {
                on_pushButton_add_clicked();
            }
        }
        settings.endArray();
        qsettingRead(replayFileDir);
    }
}

void commandDialog::qsettingRead(QString full_file_name)
{
    on_pushButton_ClearAll_clicked();
    QSettings settings(full_file_name, QSettings::IniFormat);
    QList<QCheckBox *> cycleCheckBox_l      = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("cycle_checkBox");
    QList<QLineEdit *> lineEdit_l           = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
    QList<QCheckBox *> hexCheckBox_l        = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_Hex");
    QList<QCheckBox *> checkBoxCRLF_l       = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_CRLF");
    QList<QLineEdit *> delayTime_lineEdit_l = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("delayTime_lineEdit");
    int size = settings.beginReadArray("projectConfiguration");

    for (int i=0; i<size; ++i)
    {
        int cycle_checkBox_statu_tmp;
        QString linetext_tmp;
        int hex_checkBox_statu_tmp;
        int checkBox_CRLF_statu_tmp;
        QString delayTime_linetext_tmp;

        settings.setArrayIndex(i);
        cycle_checkBox_statu_tmp = settings.value("cycle_checkBox_statu").toInt();
        linetext_tmp             = settings.value("linetext").toString();
        hex_checkBox_statu_tmp   = settings.value("hex_checkBox_statu").toInt();
        checkBox_CRLF_statu_tmp  = settings.value("checkBox_CRLF_statu").toInt();
        delayTime_linetext_tmp   = settings.value("delayTime_linetext").toString();

        if(cycle_checkBox_statu_tmp)
        {
            cycleCheckBox_l[i]->setCheckState(Qt::Checked);
        }else{
            cycleCheckBox_l[i]->setCheckState(Qt::Unchecked);
        }

        lineEdit_l[i]->setText(linetext_tmp);

        if(hex_checkBox_statu_tmp)
        {
            hexCheckBox_l[i]->blockSignals(true);
            hexCheckBox_l[i]->setCheckState(Qt::Checked);
            hexCheckBox_l[i]->blockSignals(false);

        }else{
            hexCheckBox_l[i]->setCheckState(Qt::Unchecked);
        }

        if(checkBox_CRLF_statu_tmp)
        {
            checkBoxCRLF_l[i]->blockSignals(true);
            checkBoxCRLF_l[i]->setCheckState(Qt::Checked);
            checkBoxCRLF_l[i]->blockSignals(false);

        }else{
            checkBoxCRLF_l[i]->setCheckState(Qt::Unchecked);
        }
        delayTime_lineEdit_l[i]->setText(delayTime_linetext_tmp);
    }
    settings.endArray();
}

void commandDialog::on_pushButton_clicked()
{
    qsettingWrite();
}

void commandDialog::on_Load_configuration_pushButton_clicked()
{
    qsettingINIlength();
}

void commandDialog::cycleTimer_Timeout()
{
    QList<CommandCheckBox *> widgets = ui->verticalLayout->findChildren<CommandCheckBox *>();

    qDebug()<<widgets.size();
    m_cycleTimer.stop();
    /* When the Run button is pressed, the button text changes to "Stop". */
    if(ui->sendButton->text() == "Stop")
    {
        if( m_lineEdit_List.count()>0 &&  m_lineEdit_List.count() > m_cycleCurrentIndex)
        {   /* set colour */
            for(int  i=0; i<m_lineEdit_List.count(); ++i)
            {
                if( i==m_cycleCurrentIndex)
                {
                    m_lineEdit_List.at(m_cycleCurrentIndex)->setStyleSheet("color: red;");
                }else
                {
                    m_lineEdit_List.at(i)->setStyleSheet("color: black;");
                }
            }
            //hex
            if( m_HexCheckBox_List.at(m_cycleCurrentIndex)->checkState())
            {
                QString string_tmp = " 0D 0A";
                if(m_CRLFCheckBox_List.at(m_cycleCurrentIndex)->isChecked()){
                    QByteArray  text = QByteArray::fromHex((m_lineEdit_List.at(m_cycleCurrentIndex)->text()+string_tmp).toLatin1());
                    emit sendData(text);

                }else{
                    QByteArray  text = QByteArray::fromHex((m_lineEdit_List.at(m_cycleCurrentIndex)->text()).toLatin1());
                    emit sendData(text);
                }
            }else
            {
                QString string_tmp1 = "\r\n";
                if(m_CRLFCheckBox_List.at(m_cycleCurrentIndex)->checkState()){

                    emit sendData((m_lineEdit_List.at(m_cycleCurrentIndex)->text()+string_tmp1).toLatin1());
                }else{
                    //  AscII
                    emit sendData((m_lineEdit_List.at(m_cycleCurrentIndex)->text()).toLatin1());
                }
            }
        }

        if( m_lineEdit_List.count()>0)
        {
            m_cycleCurrentIndex++;
            //find next0.
            if( (m_cycleCurrentIndex)>=m_lineEdit_List.count())
            {
                m_cycleCurrentIndex = 0;
            }
            m_cycleTimer.start(m_delayTime_lineEdit_List.at((m_cycleCurrentIndex+1)>=m_lineEdit_List.count()?0:(m_cycleCurrentIndex+1))->text().toInt());
        }
    }
}

void commandDialog::on_sendButton_clicked()
{
    /* set colour */
    for(int  i=0; i<m_lineEdit_List.count(); i++)
    {
        m_lineEdit_List.at(i)->setStyleSheet("color: black;");
    }

    if(ui->sendButton->text()=="Run")
    {
        ui->sendButton->setText("Stop");
        update_command_cycle_list();

        m_cycleCurrentIndex = 0;
        if( m_lineEdit_List.count()>0)
        {
            m_cycleTimer.start(100);
        }
        qDebug()<<"m_cycleTimer started";
        // stop timer
        // start timer
    }
    else{
        ui->sendButton->setText("Run");
        m_cycleCurrentIndex = 0;
        m_HexCheckBox_List       .clear();
        m_CRLFCheckBox_List      .clear();
        m_lineEdit_List          .clear();
        m_delayTime_lineEdit_List.clear();

        // stop timer
        m_cycleTimer.stop();
        qDebug()<<"m_cycleTimer stop";
    }
}

void commandDialog::on_chooseAll_stateChanged(int arg1)
{
    QList<QLineEdit *> listEdit = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
    QList<QCheckBox *> cycle_checkBox_l = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("cycle_checkBox");
    for(int i=0; i<ui->verticalLayout->count(); i++)
    {
        if(arg1)
        {
            if(!listEdit[i]->text().isEmpty())
            {
                cycle_checkBox_l[i]->setCheckState(Qt::Checked);
            }
        }
        else{
            cycle_checkBox_l[i]->setCheckState(Qt::Unchecked);
        }
    }
}

void commandDialog::on_pushButton_ClearAll_clicked()
{
    QList<QCheckBox *> hexCheckBox_clear             = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_Hex");
    QList<QLineEdit *> lineEdit_clear                = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
    QList<QLineEdit *> delayTime_lineEdit_List_clear = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("delayTime_lineEdit");
    QString tmp = "1000";
    ui->chooseAll->setCheckState(Qt::Unchecked);
    ui->checkBox_HexAll->setCheckState(Qt::Unchecked);
    ui->checkBox_DTR->setCheckState(Qt::Unchecked);
    ui->checkBox_RTS->setCheckState(Qt::Unchecked);
    foreach (QLineEdit *var, lineEdit_clear) {
        var->clear();
    }
    foreach (QLineEdit *var, delayTime_lineEdit_List_clear) {
        var->setText(tmp);
    }
    foreach (QCheckBox *var, hexCheckBox_clear) {
        var->setCheckState(Qt::Unchecked);
    }
}

void commandDialog::on_checkBox_HexAll_stateChanged(int arg1)
{
    QList<QLineEdit *> listEdit_hex = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
    QList<QCheckBox *> checkBox_hex = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_Hex");
    for(int i=0; i<ui->verticalLayout->count(); i++)
    {
        if(arg1)
        {
            if(!listEdit_hex[i]->text().isEmpty())
            {
                checkBox_hex[i]->setCheckState(Qt::Checked);
            }
        }
        else{
            checkBox_hex[i]->setCheckState(Qt::Unchecked);
        }
    }
}

void commandDialog::on_pushButton_add_clicked()
{
    CommandCheckBox *widget = new CommandCheckBox();
    if(g_checkBox<100){

        ui->verticalLayout->addWidget(widget);
        widget->setCheckBoxInfo(QString("%1").arg(g_checkBox, 2, 10, QLatin1Char('0')));
        g_checkBox++;
        connect(widget,SIGNAL(sendCMD( const char *, unsigned int)),this,SIGNAL(sendData(const  char *, unsigned int)));
        connect(widget,SIGNAL(sendCMD( QByteArray)),this,SIGNAL(sendData(QByteArray)));
    }else{
        QMessageBox::warning(this,"Command add failed","Failed to increase the number of commands, only 99 commands are supported!");
    }
}
//DTR
void commandDialog::on_checkBox_stateChanged(int arg1)
{
    if(arg1)
    {
        emit sendDTRStatu(true);
    }else{
        emit sendDTRStatu(false);
    }
}
//RTS
void commandDialog::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1)
    {
        emit sendRTSStatu(true);
    }else{
        emit sendRTSStatu(false);
    }
}

void commandDialog::on_carriage_char_checkBox_stateChanged(int arg1)
{
    QList<QLineEdit *> listEdit_hex   = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
    QList<QCheckBox *> checkBoxCRLF_l = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_CRLF");
    for(int i=0; i<ui->verticalLayout->count(); i++)
    {
        if(arg1)
        {
            checkBoxCRLF_l[i]->setCheckState(Qt::Checked);
        }
        else{
            checkBoxCRLF_l[i]->setCheckState(Qt::Unchecked);
        }
    }
}

int addNMEA_C(QString &NMEA_Ste)
{
    char sum = 0;
    QString str;
    for(int i = 1;i<NMEA_Ste.length();i++){
        sum^=NMEA_Ste.at(i).toLatin1();
    }
    NMEA_Ste.append(str.sprintf("*%02X",sum));
    return sum;
}

void commandDialog::on_pushButton_NMEACS_clicked()
{
    QList<QLineEdit *> listEdit_hex = ui->scrollAreaWidgetContents->findChildren<QLineEdit *>("command_LineEdit");
    QList<QCheckBox *> checkBox_hex = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>("checkBox_Hex");
    for(int i=0; i<ui->verticalLayout->count(); i++)
    {
        if((listEdit_hex[i]->text().startsWith("$")) && (listEdit_hex[i]->text()!="") && (!checkBox_hex[i]->isChecked()))
        {

            QString text;
            if((!listEdit_hex[i]->text().contains('*', Qt::CaseSensitive)))
            {
                text = listEdit_hex[i]->text();
            }else{
                text = listEdit_hex[i]->text().split('*')[0];
            }
            addNMEA_C(text);
            listEdit_hex[i]->setText(text);
        }
    }
}
