#include "surveyview.h"
#include "ui_surveyview.h"
#include "mainwindow.h"
#include "lla_xyz_component.h"

#include <QMessageBox>
#include <QDebug>

SurveyView::SurveyView(QWidget *parent) : QWidget(parent),
                                          ui(new Ui::SurveyView)
{
    ui->setupUi(this);
    qDebug()<<"Survey new.";
    QRegExp exp("([1-9]|[1-5]\\d{0,4}|60000|6\\d{0,3})");
    QValidator *Validator = new QRegExpValidator(exp);
    ui->lineEdit_miniDuration->setValidator(Validator);
    ui->widget_autoLLAXYZ->setLineEditReadOnly(true);
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat(QString("%1%").arg(QString::number(0, 'f', 2)));

    theModel = new QStandardItemModel(this);

    ui->listView->setModel(theModel);
//    ui->listView->setSelectionMode(theSelection);

    //    ui->listView->setMovement(QListView::Free);
    //    ui->listView->setSpacing(2);

    ui->widget_input->setLineLLA_Enable(false);
    typeGroup = new QButtonGroup(this);
    typeGroup->addButton(ui->radioButton_lla, 0);
    typeGroup->addButton(ui->radioButton_xyz, 1);
    connect(typeGroup, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
            [=](int id, bool checked)
            {
                int cur_id = typeGroup->checkedId();
                ui->widget_input->setLineEditEnable(false);
                if (cur_id == 0)
                {
                    ui->widget_input->setLineLLA_Enable(true);
                }
                else
                {
                    ui->widget_input->setLineXYZ_Enable(true);
                }
            });

    readIniFile();
}

SurveyView::~SurveyView()
{
    delete typeGroup;
    delete ui;
}

void SurveyView::updateSurveyInfo()
{
    if (mainForm->NMEA_Data.SurveyInStatus.valid)
    {
        double curP = mainForm->NMEA_Data.SurveyInStatus.obs * 100.0 / mainForm->NMEA_Data.SurveyInStatus.CfgDur;
        ui->progressBar->setValue(curP * 100.0);
        ui->progressBar->setFormat(QString("%1%").arg(QString::number(curP, 'f', 2)));
    }
    else
    {
        ui->progressBar->setValue(0);
        ui->progressBar->setFormat(QString("%1%").arg(QString::number(0, 'f', 2)));
    }
    if (mainForm->NMEA_Data.SurveyInStatus.valid == 2)
    {
        double p[] = {
            mainForm->NMEA_Data.SurveyInStatus.MeanX,
            mainForm->NMEA_Data.SurveyInStatus.MeanY,
            mainForm->NMEA_Data.SurveyInStatus.MeanZ};
        ui->widget_autoLLAXYZ->showXYZ_Pos(p);
    }
}

void SurveyView::closeEvent(QCloseEvent *event)
{
    qDebug()<<"Survey close.";
    QString setV;
    for (int i = 0;i < theModel->rowCount(); i++) {
        auto value = theModel->data(theModel->index(i,0), Qt::UserRole);
        QString key = theModel->data(theModel->index(i,0), Qt::DisplayRole).toString();
        auto pos = value.value<SaveCoordinate>();
        pos.name = key;
        setV.append(pos.name + "\t");
        setV.append(pos.lla[0] + "\t");
        setV.append(pos.lla[1] + "\t");
        setV.append(pos.lla[2] + "\t");
        setV.append(pos.xyz[0] + "\t");
        setV.append(pos.xyz[1] + "\t");
        setV.append(pos.xyz[2] + "\n");
    }
    qDebug()<<setV;
    QFile saveFile(POS_FIILENAME);
    saveFile.open(QIODevice::WriteOnly);
    saveFile.write(setV.toLocal8Bit());
    saveFile.close();
}

void SurveyView::on_pushButton_AutoSurvey_clicked()
{
    int CfgDur = 60000;
    CfgDur = ui->lineEdit_miniDuration->text().toInt();
    auto cmd = QString("$PQTMSETRECVMODE,1,%1,0,0,0,0").arg(CfgDur);
    addNMEA_CS(cmd);
    sendData(cmd.toLocal8Bit());
    sendData("$PQTMCFGMSGRATE,W,PQTMSVINSTATUS,1,1*58\r\n");
    ui->widget_autoLLAXYZ->clearLineEdit();
}

void SurveyView::on_pushButton_assign_clicked()
{
    QString msg = ui->widget_input->inputPosCheck(typeGroup->checkedId());
    if (msg == "")
    {
        ui->widget_input->fillAllPos(typeGroup->checkedId());

        //        int CfgDur = 60000;
        //        CfgDur = ui->lineEdit_miniDuration->text().toInt();
        auto cmd = QString("$PQTMSETRECVMODE,2,90,0,%1,%2,%3")
                       .arg(ui->widget_input->xyz_pos[0], 0, 'f')
                       .arg(ui->widget_input->xyz_pos[1], 0, 'f')
                       .arg(ui->widget_input->xyz_pos[2], 0, 'f');
        addNMEA_CS(cmd);
        sendData(cmd.toLocal8Bit());
    }
    else
    {
        QMessageBox::warning(this, "Basic Station", msg);
    }
}

void SurveyView::on_pushButton_save_clicked()
{
    theModel->insertRow(theModel->rowCount(), new QStandardItem(QString("Point #%1").arg(theModel->rowCount())));
    QVariant v;
    v.setValue(ui->widget_autoLLAXYZ->getSaveCoodinate());
    theModel->setData(theModel->index(theModel->rowCount() - 1, 0), v, Qt::UserRole);
}

void SurveyView::on_listView_doubleClicked(const QModelIndex &index)
{
    QVariant v = theModel->data(index, Qt::UserRole);
    auto pos = v.value<SaveCoordinate>();
    ui->widget_input->setSaveCoodinate(pos);
}

void SurveyView::readIniFile()
{
    QFile saveFile(POS_FIILENAME);
    if(saveFile.open(QIODevice::ReadOnly)){
        QString fileData = saveFile.readAll();
        QStringList rowDataList = fileData.split('\n',QString::SkipEmptyParts);
        foreach (auto var, rowDataList) {
            QStringList rowData = var.split('\t');
            if(rowData.count() == 7){
                qDebug()<<rowData;
                theModel->insertRow(theModel->rowCount(), new QStandardItem(rowData[0]));
                QVariant v;
                SaveCoordinate sc;
                sc.lla[0] = rowData[1];
                sc.lla[1] = rowData[2];
                sc.lla[2] = rowData[3];
                sc.xyz[0] = rowData[4];
                sc.xyz[1] = rowData[5];
                sc.xyz[2] = rowData[6];
                v.setValue(sc);
                theModel->setData(theModel->index(theModel->rowCount() - 1, 0), v, Qt::UserRole);
            }
        }
    }
    else{
        qInfo()<<"File open error!!"<<saveFile.errorString();
    }
}

void SurveyView::on_pushButton_clear_Item_clicked()
{
    theModel->removeRow(ui->listView->currentIndex().row());
}
