#ifndef FILTEROPTIONSDIALOG_H
#define FILTEROPTIONSDIALOG_H

#include <QDialog>
#include "qlog_data_center.h"
#include "qnmea.h"
namespace Ui {
class FilterOptionsDialog;
}

class FilterOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterOptionsDialog(QWidget *parent = nullptr);
    ~FilterOptionsDialog();
    void filterOptionsDialogRead();
    void filterOptionsDialogWrite();

private slots:


    void on_pushButton_OK_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_Apply_clicked();

private:
    Ui::FilterOptionsDialog *ui;
};

#endif // FILTEROPTIONSDIALOG_H
