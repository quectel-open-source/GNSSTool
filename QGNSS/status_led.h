#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <QLabel>
#include <QTimer>

class status_led : public QLabel
{
    Q_OBJECT
public:
    explicit status_led(QWidget *parent = nullptr);

    void setContinue_blink(bool Continue_blink);

    void setLight(bool value);

signals:

public slots:
    void on_timer_timeout();

private:
    QTimer *fTimer_led;
    int m_blink_time;
    bool m_Continue_blink;
    bool light;
};

#endif // STATUS_LED_H
