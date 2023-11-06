#ifndef EXPERT_UPG_UI_H
#define EXPERT_UPG_UI_H

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

struct ImageOptions;
QDebug operator<<(QDebug debug,const ImageOptions &opt);

class Expert_UPG_UI : public QDialog
{
  Q_OBJECT
 public:
  explicit Expert_UPG_UI(ImageOptions *fakeOpt,
                         QWidget *parent = nullptr);
  ~Expert_UPG_UI();
  void CreatUI();
 Q_SIGNALS:
 public Q_SLOTS:
  void  Resert(ImageOptions *fakeOpt);
 private:
  ImageOptions *DefaultOpt_;//未修改前
  ImageOptions *ReferOpt_=nullptr;//地址
  QLabel * eraseNVM_lb;
  QCheckBox * eraseNVM_ck;

  QLabel * programOnly_lb;
  QCheckBox * programOnly_ck;

  QLabel * firmwareSize_lb;
  QLineEdit *firmwareSize_l;

  QLabel * firmwareCRC_lb;
  QLineEdit *firmwareCRC_l;

  QLabel * nvmAddressOffset_lb;
  QLineEdit *nvmAddressOffset_l;

  QLabel * nvmSize_lb;
  QLineEdit *nvmSize_l;

  QPushButton *OK_;
  QPushButton *Reset_;
};

#endif // EXPERT_UPG_UI_H
