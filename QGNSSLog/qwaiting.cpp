#include "qwaiting.h"
#include "ui_qwaiting.h"
#include "qmovie.h"

QWaiting::QWaiting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QWaiting)
{
    ui->setupUi(this);
    this->setWindowOpacity(1);
    this->setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::WindowModal);
    ui->label->setStyleSheet("background-color: transparent;");
    QMovie *movie = new QMovie(":/images/movie4.gif");
    ui->label->setMovie(movie);
    ui->label->setScaledContents(true);
    movie->start();
}

QWaiting::~QWaiting()
{
    delete ui;
}
