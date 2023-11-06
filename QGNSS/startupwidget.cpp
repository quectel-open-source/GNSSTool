#include "startupwidget.h"
#include "ui_startupwidget.h"

startupWidget::startupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::startupWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("Quectel");
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    ui->widget->setStyleSheet(QString("background-image:url(:/images/startLogo.png);"));
}

startupWidget::~startupWidget()
{
    delete ui;
}
