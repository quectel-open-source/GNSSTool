#ifndef QWAITING_H
#define QWAITING_H

#include <QDialog>

namespace Ui {
class QWaiting;
}

class QWaiting : public QDialog
{
    Q_OBJECT

public:
    explicit QWaiting(QWidget *parent = nullptr);
    ~QWaiting();

private:
    Ui::QWaiting *ui;
};

#endif // QWAITING_H
