#ifndef MTK3335_READBACK_CFG_DIALOG_UI_H
#define MTK3335_READBACK_CFG_DIALOG_UI_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QIcon>
#include <QFileDialog>
#include <QDebug>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QMetaEnum>
#include "qgnss_flashtool_help.h"
    class MTK3335_ReadBack_CFG_Dialog_UI : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(CFG* cfg_Info READ cfg_Info WRITE setCfg_Info NOTIFY cfg_InfoChanged)
public:

        enum class AddressType :unsigned char
    {
        Physical,
        Logical,
        };
    Q_ENUM (AddressType)

    enum CFG_Mode
    {
        AddRow,
        RemoveRow,
        EditRow
    };
    Q_ENUM (CFG_Mode)

    typedef struct CFG_
    {
        CFG_Mode mode;
        int  CurrentRow;
        QString FileName;
        QString FilePath;
        AddressType ADRType;
        uint BeginAddress;
        uint Length;

    }CFG;

    MTK3335_ReadBack_CFG_Dialog_UI(QWidget *parent,CFG &cfg);

    CFG* cfg_Info() const;

    friend QDebug& operator<<(QDebug out, const CFG& info)
    {
        out <<"CurrentRow"    <<info.CurrentRow<<
            "Length"    <<info.FilePath<<
            "ADRType"   <<info.ADRType<<
            "BeginAddress"<<info.BeginAddress<<
            "Length"      <<info.Length;
        return out.maybeSpace ();
    }

Q_SIGNALS:
    void cfg_InfoChanged(CFG* cfg_Info);

public Q_SLOTS:
    void setCfg_Info(CFG* cfg_Info);

private:
    CFG  *cfg_=nullptr;
    void INI_UI();
    QString get_Name();
    QLineEdit *file_path_=nullptr;
    QCheckBox *logical_box_=nullptr;
    QCheckBox *physical_box_=nullptr;
    QLineEdit *begin_addresstxt_=nullptr;
    QLineEdit *readBack_lengthtxt_=nullptr;
    CFG* m_cfg_Info;

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};
Q_DECLARE_METATYPE (MTK3335_ReadBack_CFG_Dialog_UI::CFG*)
#endif // MTK3335_READBACK_CFG_DIALOG_UI_H
