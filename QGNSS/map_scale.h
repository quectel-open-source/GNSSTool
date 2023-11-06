#ifndef MAP_SCALE_H
#define MAP_SCALE_H
#include <QWidget>

class Map_Scale : public QWidget
{
    Q_OBJECT
public:
    explicit Map_Scale(QWidget *parent = nullptr);

    void setTickSize(double tickSize);

protected:
    void paintEvent(QPaintEvent *event);

private:
    double mTickSize;

signals:

};

#endif // MAP_SCALE_H
