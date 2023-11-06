/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mt3335_ui.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-06      victor.gong
* ***************************************************************************/
#ifndef MT3335_UI_H
#define MT3335_UI_H

#include <QWidget>
#include <QLineEdit>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QToolButton>
#include "mtk_base_window.h"
#include "mtk_model_3335.h"
#include "mtk3335_format_ui.h"
#include "mtk3335_readback_ui.h"
#include "base_action.h"
    /**
 * @brief The MT3335_UI class AG3335-UI
 */
    class MT3335_UI : public MTK_base_Window
{
    Q_OBJECT
    Q_PROPERTY(QString cfgPTH READ cfgPTH WRITE setCfgPTH NOTIFY cfgPTHChanged)
    Q_PROPERTY(UI_mode currentmode READ currentmode WRITE setCurrentmode NOTIFY currentmodeChanged)

        public:
        explicit MT3335_UI(QWidget *parent = nullptr);
    ~MT3335_UI();
    enum cfg_key{
        baudrate,
        cfgpth,
        FormatEnableShortcut,
        ReadBackShortcut
        };
    Q_ENUM (cfg_key)

    enum UI_mode{
        Download,
        Format,
        ReadBack
    };
    Q_ENUM (UI_mode)

    Q_INVOKABLE void start() override;
    Q_INVOKABLE void stop() override;
    Q_INVOKABLE void ini_IO(IODvice *io) override;

    bool event(QEvent *event) override;
    QString cfgPTH() const;

    UI_mode currentmode() const;
    MTK_MODEL_3335 *MTKMODEL3335_=nullptr;
    Q_SIGNALS:

    void cfgPTHChanged(QString cfgPTH);

    void currentmodeChanged(UI_mode currentmode);

    public Q_SLOTS:

    void setCfgPTH(QString cfgPTH);

    void setCurrentmode(UI_mode currentmode);

    private:
    QAction * menu_dacfg_=nullptr;
    //Config File
    QLabel *cfg_name_=nullptr;
    QLineEdit *label_cfg_pth_;
    QHBoxLayout *cfgfile_Glayout_=nullptr;

    MTK3335_Format_UI *formatUI_=nullptr;
    MTK3335_ReadBack_Ui *readbackUI_=nullptr;
    QStackedWidget *stackWidget_=nullptr;
    QToolButton *downloadbt_=nullptr;
    QToolButton *formatbt_=nullptr;
    QToolButton *readbackbt_=nullptr;
    // cfg
    void cfg_widget() override;
    void cfg_parameter() override;
    void cfg_connect();
    void cfg_construct();
    void load_cfg();
    void load_table_content();
    QString m_cfgPTH;
    QString da921600pth_=":/assets/cfg/MT3335/DAFILE/921600/slave_da_UART.bin";
    QString da115200pth_=":/assets/cfg/MT3335/DAFILE/115200/slave_da_UART.bin";
    UI_mode m_currentmode;

    QAction * enabel_format_=nullptr;
    QAction * enabel_readback_=nullptr;
    // MTK_base_Window interface
    public:
    virtual void wgt_enable(bool Run) override;
    };

#endif // MT3335_UI_H
