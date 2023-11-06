#ifndef CONFIGURATION_LG69TAA_H
#define CONFIGURATION_LG69TAA_H

#include <QDialog>

namespace Ui {
class configuration_LG69TAA;
}

class configuration_LG69TAA : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_LG69TAA(QWidget *parent = nullptr);
    ~configuration_LG69TAA();

private slots:
    void on_pushButton_pollVersion_clicked();

private:
    Ui::configuration_LG69TAA *ui;
};

#endif // CONFIGURATION_LG69TAA_H
