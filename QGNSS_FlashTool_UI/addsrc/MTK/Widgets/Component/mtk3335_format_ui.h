#ifndef MTK3335_FORMAT_UI_H
#define MTK3335_FORMAT_UI_H

#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QAction>
#include <QFontMetrics>
#include <QRegExp>
#include <QRegExpValidator>
#include "qgnss_flashtool_help.h"
#include "base_groupbox.h"
#include "setting_cfg.h"
#include "mtk_brom_3335.h"
class MTK3335_Format_UI : public Base_GroupBox
{
    Q_OBJECT
    Q_PROPERTY(Format_mode currentmode READ currentmode WRITE setCurrentmode NOTIFY currentmodeChanged)
    Q_PROPERTY(format_Info* formatInfo READ formatInfo WRITE setFormatInfo NOTIFY formatInfoChanged)
    Q_PROPERTY(uint length READ length WRITE setLength NOTIFY lengthChanged)
public:
    explicit MTK3335_Format_UI(QWidget *parent = nullptr);
    explicit MTK3335_Format_UI(const QString &title, QWidget *parent = nullptr);
    ~MTK3335_Format_UI();
    struct format_Info{
        QString beginaddress;
        QString length;
    };

    typedef  MTK_BROM_3335::Format_mode  Format_mode;

    /*
    enum cfg_key{
        ManualFormatEnableShortcut
    };

    Q_ENUM (cfg_key)
    */
    void setup_ui();
    void cfg_ui();
    void connect_ui();
    Format_mode currentmode() const;

    format_Info* formatInfo() ;

    uint length() const;

Q_SIGNALS:

    void currentmodeChanged(Format_mode currentmode);

    void formatInfoChanged(format_Info* formatInfo);

    void lengthChanged(uint length);

public Q_SLOTS:

    void setCurrentmode(Format_mode currentmode);

    void setFormatInfo(format_Info* formatInfo);

    void setLength(uint length);

private:
    QButtonGroup *formatgroup_=nullptr;
    QRadioButton *radio_auto_format_=nullptr;
    QRadioButton *radio_total_format_=nullptr;
    QRadioButton *radio_manual_format_=nullptr;
    Base_GroupBox *groupbox_autoformat=nullptr;
    Base_GroupBox *groupbox_totalformat=nullptr;
    Base_GroupBox *groupbox_manualformat=nullptr;
    QButtonGroup *addressgroup_=nullptr;
    QRadioButton *radio_logical_=nullptr;
    QRadioButton *radio_physical_=nullptr;
    QLineEdit *linedit_beginaddress_=nullptr;
    QLineEdit *linedit_length_=nullptr;
    QAction *enable_physical_=nullptr;
    Format_mode m_currentmode;
    format_Info* m_formatInfo=nullptr;

   // QAction * enabel_manualformat_;
    uint m_length=1024;
};
Q_DECLARE_METATYPE (MTK3335_Format_UI::format_Info);
Q_DECLARE_METATYPE (MTK3335_Format_UI::format_Info*);

#endif // MTK3335_FORMAT_UI_H
