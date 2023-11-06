#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QSettings>


namespace Ui {
class Preferences_Dialog;
}

class Preferences_Dialog : public QDialog
{
    Q_OBJECT

public:
    enum SettingsType {
    };
    Q_ENUM(SettingsType)

    explicit Preferences_Dialog(QWidget *parent = nullptr);
    ~Preferences_Dialog();

signals:
    void applyMapSettings();
    void applySettings();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_checkBox_Other_stateChanged(int arg1);

    void on_groupBox_Size_clicked(bool checked);

    void on_groupBox_Daily_clicked(bool checked);

    void on_listWidget_currentRowChanged(int currentRow);

private:
    Ui::Preferences_Dialog *ui;
    QSettings *settings;
};

#endif // PREFERENCES_DIALOG_H
