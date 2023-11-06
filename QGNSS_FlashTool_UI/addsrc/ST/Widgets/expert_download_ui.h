#ifndef EXPERT_DOWNLOAD_UI_H
#define EXPERT_DOWNLOAD_UI_H

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

struct xldr_binimg_info_t;

class Expert_Download_UI : public QDialog
{
  Q_OBJECT
 public:
  explicit Expert_Download_UI(xldr_binimg_info_t *xldr,
                              QWidget *parent = nullptr);

  ~Expert_Download_UI();
  void CreatUI();
 Q_SIGNALS:
 public Q_SLOTS:

void  Resert(xldr_binimg_info_t *Opt);

private:
 xldr_binimg_info_t *defaultxldr_;//
 xldr_binimg_info_t *ptrxldr_;

 QLineEdit *fileSize_edt;
 QComboBox *bootMode_cbx;
 QLineEdit *crc32_edt;
 QLineEdit *destinationAddress_edt;
 QLineEdit *entryPoint_edt;
 QComboBox *eraseOption_cbx;
 QCheckBox *eraseOnly_chk;
 QCheckBox *programOnly_chk;
 QCheckBox *subSector_chk;
 QCheckBox *sta8090fg_chk;
 QLineEdit *NVMOffset_edt;
 QLineEdit *NVMSize_edt;



 QPushButton *OK_;
 QPushButton *Reset_;
};

#endif // EXPERT_DOWNLOAD_UI_H
