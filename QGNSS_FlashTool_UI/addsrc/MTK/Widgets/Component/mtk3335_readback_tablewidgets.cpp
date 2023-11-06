#include "mtk3335_readback_tablewidgets.h"
#include "QMetaEnum"
MTK3335_ReadBack_TableWidgets::MTK3335_ReadBack_TableWidgets(QWidget *parent)
    : QTableWidget(parent)
{
    INI_Ui();
}

MTK3335_ReadBack_TableWidgets::MTK3335_ReadBack_TableWidgets(int rows, int columns, QWidget *parent)
    :QTableWidget(rows,columns,parent)
{
    INI_Ui();
}

void MTK3335_ReadBack_TableWidgets::MTK_addRow(int CurrentRow)
{
    cfg.mode=MTK3335_ReadBack_CFG_Dialog_UI::CFG_Mode::AddRow;
    cfg.CurrentRow=CurrentRow;
    cfgUI->show ();
}

void MTK3335_ReadBack_TableWidgets::MTK_editRow(int CurrentRow)
{
    cfg.mode=MTK3335_ReadBack_CFG_Dialog_UI::CFG_Mode::EditRow;
    QMetaEnum menum=QMetaEnum::fromType<MTK3335_ReadBack_CFG_Dialog_UI::AddressType>();
    cfg.CurrentRow=CurrentRow;
    cfg.FileName=item (currentRow (),1)->text ();
    cfg.BeginAddress= item (currentRow (),2)->text ().toUInt (nullptr,16);
    cfg.Length= item (currentRow (),3)->text ().toUInt (nullptr,16);
    cfg.ADRType=(MTK3335_ReadBack_CFG_Dialog_UI::AddressType)menum.keyToValue (item (currentRow (),4)->text ().toUtf8 ());
    cfg.FilePath=item (currentRow (),5)->text ();
    cfgUI->show ();
}

void MTK3335_ReadBack_TableWidgets::MTK_removeRow(int CurrentRow)
{
    cfg.mode=MTK3335_ReadBack_CFG_Dialog_UI::CFG_Mode::RemoveRow;
    if(CurrentRow>rowCount ())
    {
        return;
    }
    removeRow (CurrentRow);
}

void MTK3335_ReadBack_TableWidgets::INI_Ui()
{
    cfgUI=new MTK3335_ReadBack_CFG_Dialog_UI(this,cfg);
    connect (cfgUI,&MTK3335_ReadBack_CFG_Dialog_UI::cfg_InfoChanged,this,[&](MTK3335_ReadBack_CFG_Dialog_UI::CFG *cfg_)
            {
                switch (cfg_->mode)
                {
                case MTK3335_ReadBack_CFG_Dialog_UI::AddRow:
                {
                    insertRow (rowCount ());
                    setItem (rowCount ()-1,0,new QTableWidgetItem);
                    item (rowCount ()-1,0)->setCheckState (Qt::Checked);
                    setItem (rowCount ()-1,1,new QTableWidgetItem(cfg_->FileName));
                    setItem (rowCount ()-1,2,new QTableWidgetItem("0x"+QString::number (cfg_->BeginAddress,16).rightJustified (8,'0')));
                    setItem (rowCount ()-1,3,new QTableWidgetItem("0x"+QString::number (cfg_->Length,16).rightJustified (8,'0')));
                    QMetaEnum menum=QMetaEnum::fromType<MTK3335_ReadBack_CFG_Dialog_UI::AddressType>();
                    setItem (rowCount ()-1,4,new QTableWidgetItem(menum.valueToKey ((int)cfg_->ADRType)));
                    setItem (rowCount ()-1,5,new QTableWidgetItem(cfg_->FilePath));
                }
                    break;
                case MTK3335_ReadBack_CFG_Dialog_UI::RemoveRow:
                    break;
                case MTK3335_ReadBack_CFG_Dialog_UI::EditRow:
                {
                    setItem (cfg_->CurrentRow,1,new QTableWidgetItem(cfg_->FileName));
                    setItem (cfg_->CurrentRow,2,new QTableWidgetItem("0x"+QString::number (cfg_->BeginAddress,16).rightJustified (8,'0')));
                    setItem (cfg_->CurrentRow,3,new QTableWidgetItem("0x"+QString::number (cfg_->Length,16).rightJustified (8,'0')));
                    QMetaEnum menum=QMetaEnum::fromType<MTK3335_ReadBack_CFG_Dialog_UI::AddressType>();
                    setItem (cfg_->CurrentRow,4,new QTableWidgetItem(menum.valueToKey ((int)cfg_->ADRType)));
                    setItem (cfg_->CurrentRow,5,new QTableWidgetItem(cfg_->FilePath));
                }
                    break;

                }

            });
    headview_=new MTK_base_HeadView(Qt::Orientation::Horizontal);
    setColumnCount (6);
    setHorizontalHeader (headview_);
    headview_->setFirstisCheckable (true);
    this->setEditTriggers (QTableWidget::NoEditTriggers);
    this->setSelectionMode (SelectionMode::SingleSelection);
    horizontalHeader ()->setSectionResizeMode (QHeaderView::ResizeToContents);
    this->resizeColumnToContents (6);
    for(int i=1;i<=tabelheaderlabels_.size ();i++)
    {
        setHorizontalHeaderItem (i,new QTableWidgetItem(tabelheaderlabels_[i-1]));
    }

    connect (headview_,&Base_HeadView::sectionClicked,this,[&](int index)
            {
                if(index!=0){return;}
                if(rowCount ()<1)
                {
                    return;
                }
                for(int i=0;i<rowCount ();i++)
                {
                    item (i,0)->setCheckState (headview_->firstChecked ()?Qt::Checked:Qt::Unchecked);
                }

            });
    connect (this,&MTK3335_ReadBack_TableWidgets::itemClicked,this,[&](QTableWidgetItem *itm)
            {
                if(itm->column ()==0)
                {
                    bool selectedall=true;
                    for(int i=0;i<rowCount ();i++)
                    {
                        if(item(i,0)->checkState ()==Qt::Unchecked)
                        {
                            selectedall=false;
                        }
                    }
                    headview_->setFirstChecked (selectedall);
                }
            });
    connect (this,&MTK3335_ReadBack_TableWidgets::itemDoubleClicked,this,[&](QTableWidgetItem *itm)
            {
                if(itm->column ()>0)
                {
                    MTK_editRow(itm->row ());
                }
            });

    this->setAlternatingRowColors(true);
    this->horizontalHeader ()->setSectionResizeMode (QHeaderView::ResizeToContents);
    this->horizontalHeader ()->setMaximumSectionSize (600);
}
