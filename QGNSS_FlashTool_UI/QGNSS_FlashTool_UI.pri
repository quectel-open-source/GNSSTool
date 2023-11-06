MOC_DIR=moc
OBJECTS_DIR=object
UI_DIR=ui
RCC_DIR=rcc

#TRANSLATIONS = \
#assets/translations/*ts

SOURCES += \
    $$PWD/addsrc/Allystar/MODEL/allystar_model.cpp \
    $$PWD/addsrc/Allystar/Widgets/Component/allystar_base_window.cpp \
    $$PWD/addsrc/Allystar/Widgets/allystar_ui.cpp \
    $$PWD/addsrc/BaseWidgets/base_action.cpp \
    $$PWD/addsrc/BaseWidgets/base_button.cpp \
    $$PWD/addsrc/BaseWidgets/base_dialog.cpp \
    $$PWD/addsrc/BaseWidgets/base_groupbox.cpp \
    $$PWD/addsrc/BaseWidgets/base_headview.cpp \
    $$PWD/addsrc/BaseWidgets/base_label.cpp \
    $$PWD/addsrc/BaseWidgets/base_mainwindow.cpp \
    $$PWD/addsrc/Broadcom/MODEL/broadcom_model.cpp \
    $$PWD/addsrc/Broadcom/Widgets/Component/broadcom_base_window.cpp \
    $$PWD/addsrc/Broadcom/Widgets/broadcom_ui.cpp \
    $$PWD/addsrc/Goke/MODEL/goke_model.cpp \
    $$PWD/addsrc/Goke/Widgets/Component/goke_base_window.cpp \
    $$PWD/addsrc/Goke/Widgets/goke_ui.cpp \
    $$PWD/addsrc/MTK/DATACENTER/mtk_datacenter.cpp \
    $$PWD/addsrc/MTK/MODEL/mtk_model_3333.cpp \
    $$PWD/addsrc/MTK/MODEL/mtk_model_3335.cpp \
    $$PWD/addsrc/MTK/MODEL/mtk_model_3352.cpp \
    $$PWD/addsrc/MTK/Widgets/Component/mt3335_cfg_ui.cpp \
    $$PWD/addsrc/MTK/Widgets/Component/mtk3335_format_ui.cpp \
    $$PWD/addsrc/MTK/Widgets/Component/mtk3335_readback_cfg_dialog_ui.cpp \
    $$PWD/addsrc/MTK/Widgets/Component/mtk3335_readback_tablewidgets.cpp \
    $$PWD/addsrc/MTK/Widgets/Component/mtk3335_readback_ui.cpp \
    $$PWD/addsrc/MTK/Widgets/Component/mtk_base_headview.cpp \
    $$PWD/addsrc/MTK/Widgets/Component/mtk_base_window.cpp \
    $$PWD/addsrc/MTK/Widgets/Component/mtk_tablewidget.cpp \
    $$PWD/addsrc/MTK/Widgets/mt3333_ui.cpp \
    $$PWD/addsrc/CommonWidgets/status_wgt.cpp \
    $$PWD/addsrc/MTK/Widgets/mt3335_ui.cpp \
    $$PWD/addsrc/MTK/Widgets/mt3352_ui.cpp \
    $$PWD/addsrc/ST/MODEL/st_model.cpp \
    $$PWD/addsrc/ST/MODEL/st_mqsp_model.cpp \
    $$PWD/addsrc/ST/Widgets/Component/st_base_window.cpp \
    $$PWD/addsrc/ST/Widgets/expert_download_ui.cpp \
    $$PWD/addsrc/ST/Widgets/expert_mqsp_upg_ui.cpp \
    $$PWD/addsrc/ST/Widgets/expert_upg_ui.cpp \
    $$PWD/addsrc/ST/Widgets/st_mqsp_ui.cpp \
    $$PWD/addsrc/ST/Widgets/st_pm_discardqt_ui.cpp \
    $$PWD/addsrc/ST/Widgets/st_ui.cpp \
    $$PWD/addsrc/Unicorecomm/MODEL/uc_model.cpp \
    $$PWD/addsrc/Unicorecomm/Widgets/Component/uc_base_window.cpp \
    $$PWD/addsrc/Unicorecomm/Widgets/uc_ui.cpp \
    $$PWD/addsrc/qgnss_flashtool_help.cpp \
    $$PWD/addsrc/Control/qgnss_flashtool.cpp  \
    $$PWD/src/Misc/myUtilities.cpp            \
    $$PWD/src/Misc/setting_cfg.cpp



HEADERS += \
    $$PWD/addsrc/Allystar/MODEL/allystar_model.h \
    $$PWD/addsrc/Allystar/Widgets/Component/allystar_base_window.h \
    $$PWD/addsrc/Allystar/Widgets/allystar_ui.h \
    $$PWD/addsrc/BaseWidgets/base_action.h \
    $$PWD/addsrc/BaseWidgets/base_button.h \
    $$PWD/addsrc/BaseWidgets/base_dialog.h \
    $$PWD/addsrc/BaseWidgets/base_groupbox.h \
    $$PWD/addsrc/BaseWidgets/base_headview.h \
    $$PWD/addsrc/BaseWidgets/base_label.h \
    $$PWD/addsrc/BaseWidgets/base_mainwindow.h \
    $$PWD/addsrc/Broadcom/MODEL/broadcom_model.h \
    $$PWD/addsrc/Broadcom/Widgets/Component/broadcom_base_window.h \
    $$PWD/addsrc/Broadcom/Widgets/broadcom_ui.h \
    $$PWD/addsrc/Goke/MODEL/goke_model.h \
    $$PWD/addsrc/Goke/Widgets/Component/goke_base_window.h \
    $$PWD/addsrc/Goke/Widgets/goke_ui.h \
    $$PWD/addsrc/MTK/DATACENTER/mtk_datacenter.h \
    $$PWD/addsrc/MTK/MODEL/mtk_model_3333.h \
    $$PWD/addsrc/MTK/MODEL/mtk_model_3335.h \
    $$PWD/addsrc/MTK/MODEL/mtk_model_3352.h \
    $$PWD/addsrc/Control/qgnss_flashtool.h \
    $$PWD/addsrc/MTK/Widgets/Component/mt3335_cfg_ui.h \
    $$PWD/addsrc/MTK/Widgets/Component/mtk3335_format_ui.h \
    $$PWD/addsrc/MTK/Widgets/Component/mtk3335_readback_cfg_dialog_ui.h \
    $$PWD/addsrc/MTK/Widgets/Component/mtk3335_readback_tablewidgets.h \
    $$PWD/addsrc/MTK/Widgets/Component/mtk3335_readback_ui.h \
    $$PWD/addsrc/MTK/Widgets/Component/mtk_base_headview.h \
    $$PWD/addsrc/MTK/Widgets/Component/mtk_base_window.h \
    $$PWD/addsrc/MTK/Widgets/Component/mtk_tablewidget.h \
    $$PWD/addsrc/MTK/Widgets/mt3333_ui.h \
    $$PWD/addsrc/CommonWidgets/status_wgt.h \
    $$PWD/addsrc/MTK/Widgets/mt3335_ui.h \
    $$PWD/addsrc/MTK/Widgets/mt3352_ui.h \
    $$PWD/addsrc/ST/MODEL/st_model.h \
    $$PWD/addsrc/ST/MODEL/st_mqsp_model.h \
    $$PWD/addsrc/ST/Widgets/Component/st_base_window.h \
    $$PWD/addsrc/ST/Widgets/expert_download_ui.h \
    $$PWD/addsrc/ST/Widgets/expert_mqsp_upg_ui.h \
    $$PWD/addsrc/ST/Widgets/expert_upg_ui.h \
    $$PWD/addsrc/ST/Widgets/st_mqsp_ui.h \
    $$PWD/addsrc/ST/Widgets/st_pm_discardqt_ui.h \
    $$PWD/addsrc/ST/Widgets/st_ui.h \
    $$PWD/addsrc/Unicorecomm/MODEL/uc_model.h \
    $$PWD/addsrc/Unicorecomm/Widgets/Component/uc_base_window.h \
    $$PWD/addsrc/Unicorecomm/Widgets/uc_ui.h \
    $$PWD/addsrc/qgnss_flashtool_help.h \
    $$PWD/addsrc/thread_help.h \
    $$PWD/src/Misc/AppInfo.h \
    $$PWD/src/Misc/myUtilities.h \
    $$PWD/src/Misc/setting_cfg.h


INCLUDEPATH += \
    $$PWD/src/Misc \
    $$PWD/addsrc/Control        \
    $$PWD/addsrc/MTK/DATACENTER \
    $$PWD/addsrc/MTK/MODEL \
    $$PWD/addsrc/MTK/Widgets \
    $$PWD/addsrc/MTK/Widgets/Component \
    $$PWD/addsrc/Broadcom/DATACENTER \
    $$PWD/addsrc/Broadcom/MODEL \
    $$PWD/addsrc/Broadcom/Widgets \
    $$PWD/addsrc/Broadcom/Widgets/Component \
    $$PWD/addsrc/Goke/MODEL \
    $$PWD/addsrc/Goke/Widgets \
     $$PWD/addsrc/Goke/Widgets/Component \
    $$PWD/addsrc         \
    $$PWD/addsrc/Console \
    $$PWD/addsrc/CommonWidgets \
    $$PWD/src \
    $$PWD/addsrc/BaseWidgets     \
    $$PWD/addsrc/ST/MODEL \
    $$PWD/addsrc/ST/Widgets \
    $$PWD/addsrc/ST/Widgets/Component \
    $$PWD/addsrc/Allystar/MODEL \
    $$PWD/addsrc/Allystar/Widgets \
    $$PWD/addsrc/Allystar/Widgets/Component \
    $$PWD/addsrc/Unicorecomm/MODEL \
    $$PWD/addsrc/Unicorecomm/Widgets \
    $$PWD/addsrc/Unicorecomm/Widgets/Component \

RESOURCES+= $$PWD/assets.qrc




