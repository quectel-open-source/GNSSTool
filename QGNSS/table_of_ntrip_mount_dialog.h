#ifndef TABLE_OF_NTRIP_MOUNT_DIALOG_H
#define TABLE_OF_NTRIP_MOUNT_DIALOG_H

#include <QDialog>
#include "QStandardItemModel"

namespace Ui {
class Table_Of_NTRIP_Mount_Dialog;
}

class Table_Of_NTRIP_Mount_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Table_Of_NTRIP_Mount_Dialog(QWidget *parent = nullptr);
    ~Table_Of_NTRIP_Mount_Dialog();

    void insertMountPointInfo2Table(QStringList &strList);

private:
    Ui::Table_Of_NTRIP_Mount_Dialog *ui;
    QStandardItemModel* model = nullptr;
};

#endif // TABLE_OF_NTRIP_MOUNT_DIALOG_H
