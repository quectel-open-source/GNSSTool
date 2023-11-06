#pragma once
#include <QDialog>
#include <QDateTime>

#define Last_VersionURL "http://220.180.239.212:8177/updates.json"
#define OPEN_ONLINEMAP
//#define BETA

#define App_mainVersion "1.8"
#define App_subVersion "1"

const QDateTime buildDateTime();

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

private:
    Ui::About *ui;

protected:
    virtual void keyPressEvent(QKeyEvent *ev);
};

//#endif // ABOUT_H
