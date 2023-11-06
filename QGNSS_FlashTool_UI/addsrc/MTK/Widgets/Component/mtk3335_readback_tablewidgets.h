#ifndef MTK3335_READBACK_TABLEWIDGETS_H
#define MTK3335_READBACK_TABLEWIDGETS_H

#include <QObject>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "mtk_base_headview.h"
#include "mtk3335_readback_cfg_dialog_ui.h"
class MTK3335_ReadBack_TableWidgets : public QTableWidget
{
    Q_OBJECT
public:
    explicit MTK3335_ReadBack_TableWidgets(QWidget *parent = nullptr);
    explicit MTK3335_ReadBack_TableWidgets(int rows, int columns,
                                           QWidget *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:
    void MTK_addRow(int CurrentRow);
    void MTK_editRow(int CurrentRow);
    void MTK_removeRow(int CurrentRow);
private:
    void INI_Ui();
    MTK_base_HeadView *headview_;
    MTK3335_ReadBack_CFG_Dialog_UI::CFG cfg;
    MTK3335_ReadBack_CFG_Dialog_UI *cfgUI=nullptr;
    QList<QString> tabelheaderlabels_
        {
            tr("Name"),tr("Begin Address"),
            tr("Length"),tr("Address Type"),
            tr("File Path")
        };
};

#endif // MTK3335_READBACK_TABLEWIDGETS_H
