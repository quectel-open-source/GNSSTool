#ifndef CONFIGURATIONLC29D_H
#define CONFIGURATIONLC29D_H

#include <QDialog>
#include "QTreeWidgetItem"
#include "QQueue"
#include "QByteArray"
#include "QStatusBar"
#include "QDateTime"

namespace Ui {
class ConfigurationLC29D;
}

class ConfigurationLC29D : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationLC29D(QWidget *parent = nullptr);
    ~ConfigurationLC29D();

    void parseBreamMsg(QQueue<QByteArray> &breamSets_Q);
signals:
    void  sendData(const char * data, unsigned int size);
private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_GC19_clicked();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_send_clicked();

    void on_pushButton_GC0_clicked();

    void on_pushButton_GC4_clicked();

    void on_pushButton_GC5_clicked();

    void on_pushButton_GC7_clicked();

    void on_pushButton_GC12_clicked();

    void on_pushButton_GC13_clicked();

    void on_pushButton_GC14_clicked();

    void on_pushButton_GC15_clicked();

    void on_pushButton_GC16_clicked();

    void on_pushButton_GC18_clicked();

    void on_pushButton_GC21_clicked();

    void on_pushButton_GC22_clicked();

    void on_pushButton_GC23_clicked();

    void on_pushButton_GC24_clicked();

    void on_radioButton_isFreq_toggled(bool checked);

    void on_radioButton_isLength_toggled(bool checked);

    void on_pushButton_GC25_clicked();

    void on_comboBox_reset_option_currentIndexChanged(int index);

    void on_pushButton_poll_clicked();

    void on_pushButton_GC2_clicked();

    void on_pushButton_CurrentTime_clicked();

    void on_pushButton_PQTMCFGEINSMSG_clicked();

private:
    Ui::ConfigurationLC29D *ui;
    QStatusBar *statusBar;
    QSet<int> hasPoll;
    QSet<int> hideIndex;
protected:
    virtual void keyPressEvent(QKeyEvent *ev);
};

#endif // CONFIGURATIONLC29D_H
