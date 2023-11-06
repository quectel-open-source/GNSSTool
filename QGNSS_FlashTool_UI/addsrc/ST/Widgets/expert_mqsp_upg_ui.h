#ifndef EXPERT_MQSP_UPG_UI_H
#define EXPERT_MQSP_UPG_UI_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QGridLayout>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDebug>
#include <QDialog>

struct GNSS_FW_INFO;
struct APP_FW_INFO;

class Expert_MQSP_UPG_UI : public QDialog
{
  Q_OBJECT
 public:
   Expert_MQSP_UPG_UI(GNSS_FW_INFO *GNSSFWINFO,
                      QWidget *parent = nullptr);

   Expert_MQSP_UPG_UI(APP_FW_INFO *APPFWINFO,
                      QWidget *parent = nullptr);

   void CreatGNSSUI();
   void CreatAPPUI();
 Q_SIGNALS:

 public Q_SLOTS:

 private:
  GNSS_FW_INFO *defaultGNSSFWINFO_;
  GNSS_FW_INFO *GNSSFWINFO_;

  APP_FW_INFO  *defaultAPPFWINFO_;
  APP_FW_INFO  *APPFWINFO_;

  QLabel * firmwareSize_lb;
  QLineEdit *firmwareSize_l;

  QLabel * firmwareCRC_lb;
  QLineEdit *firmwareCRC_l;

  QLabel *destaddr_lb;
  QLineEdit* destaddr_l;

  QLabel *EntryPoint_lb;
  QLineEdit*EntryPoint_l;

  QLabel *NVM_Offset_lb;
  QLineEdit *NVM_Offset_l;

  QLabel *NVM_Erase_Size_lb;
  QLineEdit *NVM_Erase_Size_l;

  QLabel* EraseNVM_lb;
  QCheckBox *EraseNVM_ck;

  QLabel *EraseOnly_lb;
  QCheckBox *EraseOnly_ck;

  QLabel *ProgramOnly_lb;
  QCheckBox* ProgramOnly_ck;


  QPushButton *OK_;
  QPushButton *Reset_;

  bool gnssResert(GNSS_FW_INFO *GNSSFWINFO);
  bool appResert(APP_FW_INFO  *APPFWINFO);
};

#endif // EXPERT_MQSP_UPG_UI_H
