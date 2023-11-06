/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk3335_readback_ui.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-22      victor.gong
* ***************************************************************************/

#include "mtk3335_readback_ui.h"
MTK3335_ReadBack_Ui::MTK3335_ReadBack_Ui(QWidget *parent)
    : Base_GroupBox(parent)
{
    cfgTableWidgets();
}

MTK3335_ReadBack_Ui::MTK3335_ReadBack_Ui(const QString &title, QWidget *parent)
    : Base_GroupBox(title,parent)
{
    cfgTableWidgets();
}

void MTK3335_ReadBack_Ui::cfgTableWidgets()
{
    tablewidgets_=new MTK3335_ReadBack_TableWidgets(this);
    QVBoxLayout *Vlayout_=new QVBoxLayout;
    QPushButton *addbt_=new QPushButton(tr("Add"));
    QPushButton *debt_=new QPushButton(tr("Remove"));
    connect (addbt_,&QPushButton::clicked,this,[&](bool checked)
            {
                Q_UNUSED (checked)
                tablewidgets_->MTK_addRow (tablewidgets_->currentRow ());
            });
    connect (debt_,&QPushButton::clicked,this,[&](bool checked)
            {
                Q_UNUSED (checked)
                tablewidgets_->removeRow (tablewidgets_->currentRow ());
            });
    QHBoxLayout *hlayout_=new  QHBoxLayout;
    hlayout_->addWidget (addbt_);
    hlayout_->addWidget (debt_);
    hlayout_->addStretch ();

    Vlayout_->addLayout (hlayout_);
    Vlayout_->addWidget (tablewidgets_);
    setLayout (Vlayout_);
}
QVector<MTK_BROM_3335::Struct_ReadBack> MTK3335_ReadBack_Ui::Get_Struct_ReadBack()
{
    int rowcout=tablewidgets_->rowCount ();
    QVector<MTK_BROM_3335::Struct_ReadBack> strReadBack(rowcout);
    for(int i=0;i<rowcout;i++)
    {
        strReadBack[i].Select=tablewidgets_->item (i,0)->checkState ()==Qt::CheckState::Checked?true:false;
        strReadBack[i].Name=tablewidgets_->item (i,1)->text ();
        strReadBack[i].BeginAddress= tablewidgets_->item (i,2)->text ().toUInt (nullptr,16);
        strReadBack[i].ReadSize= tablewidgets_->item (i,3)->text ().toUInt (nullptr,16);
        QMetaEnum menum=QMetaEnum::fromType<MTK_BROM_3335::Address_Type>();
        strReadBack[i].AddressType=(MTK_BROM_3335::Address_Type)menum.keyToValue (tablewidgets_->item (i,4)->text ().toUtf8 ());
        strReadBack[i].FileDir=tablewidgets_->item (i,5)->text ();
        qDebug()<<strReadBack[i];
    }
    return strReadBack;
}
