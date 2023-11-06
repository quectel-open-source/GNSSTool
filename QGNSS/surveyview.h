#ifndef SURVEYVIEW_H
#define SURVEYVIEW_H

#include <QWidget>
#include <QButtonGroup>
#include <QStandardItemModel>

#define POS_FIILENAME "./config/SurveyPos.txt"

namespace Ui {
class SurveyView;
}

class SurveyView : public QWidget
{
    Q_OBJECT

public:
    explicit SurveyView(QWidget *parent = nullptr);
    ~SurveyView();
    void updateSurveyInfo();
signals:
    void sendData(QByteArray);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_AutoSurvey_clicked();

    void on_pushButton_assign_clicked();

    void on_pushButton_save_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

    void on_pushButton_clear_Item_clicked();

private:
    void readIniFile();
    Ui::SurveyView *ui;
    QButtonGroup *typeGroup;
    QStringList list;
    QStandardItemModel *theModel;
};

#endif // SURVEYVIEW_H
