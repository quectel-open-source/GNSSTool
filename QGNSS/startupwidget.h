#ifndef STARTUPWIDGET_H
#define STARTUPWIDGET_H

#include <QWidget>

namespace Ui {
class startupWidget;
}

class startupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit startupWidget(QWidget *parent = nullptr);
    ~startupWidget();

private:
    Ui::startupWidget *ui;
};

#endif // STARTUPWIDGET_H
