#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>

namespace Ui {
class Console;
}

class Console : public QWidget
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = nullptr);
    ~Console();

public slots:
    void displayHex(QByteArray data);
    void displayText(QByteArray data);
    void displayDump(QByteArray data);
    void on_clearText_clicked();

private slots:


private:
    Ui::Console *ui;

    void outputString(QByteArray &str);
    QString getAsciiRepresentation(QByteArray &array) const;
};

#endif // CONSOLE_H
