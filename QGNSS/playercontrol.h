#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H

#include <QDialog>
#include <QStyle>
#include <QFile>
#include <QTimer>
#include <QSlider>

#define D_SLIDER_MAX_VALUE 100
#define D_READ_SIZE 2048

namespace Ui {
class PlayerControl;
}

class PlayerControl : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerControl(QWidget *parent = nullptr);
    ~PlayerControl();

    bool IsPlayMode() const;

    bool getLogFileName();
protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::PlayerControl *ui;
    QSlider *m_QSlider;
    QStyle *style;
    QTimer *m_replayFile_Timer = nullptr;
    QFile m_replayFile;
    bool isPlayMode = false;


private slots:
    void readFileEvent();

    void on_pushButton_exit_clicked();

    void on_pushButton_play_toggled(bool checked);

    void on_horizontalSlider_sliderPressed();

    void on_horizontalSlider_sliderReleased();

signals:
    void sendData2Parser(QByteArray data);
};

#endif // PLAYERCONTROL_H
