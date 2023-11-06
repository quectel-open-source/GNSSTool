#include "status_led.h"

status_led::status_led(QWidget *parent) :
    QLabel(parent),
    fTimer_led(new QTimer),
    m_blink_time(500),
    m_Continue_blink(false)
{
    fTimer_led->setInterval (m_blink_time);
    fTimer_led->start();
    connect(fTimer_led,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));
    setStyleSheet("background-color: gray");
}

void status_led::on_timer_timeout()
{
    if(m_Continue_blink){
        setStyleSheet(light?"background-color: gray":"background-color: rgb(0,255,0)");
        light = !light;
        m_Continue_blink = !m_Continue_blink;
    }
}

void status_led::setLight(bool value)
{
    light = value;
    m_Continue_blink = false;
    setStyleSheet(light?"background-color: rgb(0,255,0)":"background-color: gray ");
}

void status_led::setContinue_blink(bool Continue_blink)
{
    m_Continue_blink = Continue_blink;
}
