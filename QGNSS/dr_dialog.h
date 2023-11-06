#ifndef DR_DIALOG_H
#define DR_DIALOG_H

#include <QDialog>
#include <DataParser/nmea_parser.h>
#include "QTimer"

namespace Ui {
class DR_Dialog;
}

class DR_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit DR_Dialog(QWidget *parent = nullptr);
    ~DR_Dialog();

    void setNMEA_Data_t(NMEA_t *value);

private slots:
    void on_timer_timeout();
private:
    Ui::DR_Dialog *ui;
    NMEA_t * NMEA_Data_t = nullptr;
    QTimer *fTimer;
};

#endif // DR_DIALOG_H
