#ifndef CONFIG_LC26G_H
#define CONFIG_LC26G_H

#include "common.h"

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
class Config_LC26G;
}

class Config_LC26G : public QDialog
{
    Q_OBJECT

public:
    explicit Config_LC26G(QWidget *parent = nullptr, const quectel_gnss::QGNSS_Model &value = quectel_gnss::QGNSS_Model::NONE);
    ~Config_LC26G();
signals:
    void sendCMD(QByteArray);

private slots:
    void on_pushButton_GC_baudrate_clicked();

    void on_pushButton_send_clicked();

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_GC_SEARCH_clicked();

private:
    Ui::Config_LC26G *ui;
};

#endif // CONFIG_LC26G_H
