#ifndef GOKE_BASE_WINDOW_H
#define GOKE_BASE_WINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QActionGroup>
#include <QSettings>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QCheckBox>
#include <QToolButton>
#include <QProgressBar>
#include <QFileDialog>

#include "status_wgt.h"
#include "base_mainwindow.h"
#include "mtk_tablewidget.h"
#include "qgnss_flashtool_help.h"
#include "mtk_datacenter.h"
#include "mtk_base_headview.h"
#include "goke_base.h"
class Goke_base_window: public Base_MainWindow
{
  Q_OBJECT
  Q_PROPERTY(int currentBaudrate READ currentBaudrate WRITE setCurrentBaudrate NOTIFY currentBaudrateChanged)
  Q_PROPERTY(int currentDownmode READ currentDownmode WRITE setCurrentDownmode NOTIFY currentDownmodeChanged)

 public:
  explicit Goke_base_window(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
  virtual  ~Goke_base_window();

  void cfg_model(Goke_Base *Base);
  virtual Q_INVOKABLE void start()=0;
  virtual Q_INVOKABLE void stop()=0;
  virtual  void ini_IO(IODvice *io) =0;

  virtual  void cfg_parameter();
  virtual void cfg_widget()=0;
  void construct_ui();
  virtual void wgt_enable(bool Run)=0;

  int currentDownmode() const;
  int currentBaudrate() const;
 Q_SIGNALS:
  void currentDownmodeChanged(int currentDownmode);
  void currentBaudrateChanged(int currentBaudrate);

 public Q_SLOTS:

  void setCurrentDownmode(int currentDownmode);
  void setCurrentBaudrate(int currentBaudrate);

 private:
  int m_currentDownmode=0;
  int m_currentBaudrate=0;

 protected:
  QToolBar *toolbar_;
  QMenuBar *menubar_;
  QMenu *menu_setting_;
  QMenu *menu_baudrate_;
  //QMenu *menu_downmode_;
  QActionGroup *baudrate_group_;
 // QActionGroup *downmode_group_;
  QWidget  *content_;
  QVBoxLayout   *mainVlayout_;
  QGroupBox  *groupbox_;
  QVBoxLayout   *groupVlayout_;
  MTK_TableWidget *rom_table_;
  MTK_base_HeadView *headview_;
  QTableWidgetItem *beginitem;
  QTableWidgetItem *enditem;
  QTableWidgetItem *loaclitem;
  QPushButton *romlb_;
  Status_WGT *statuswgt_;
  //statusbar
  QStatusBar *statusbar_;
  QLabel *downrate_;
  QLabel *portName_;
  QLabel *downmodetxt_;
  QLabel *downbuadrate_;
  QLabel *downloadtime_;
  QLabel *flashInfo_;



  Goke_Base* base=nullptr;
  QSharedPointer<QGNSS_FLASHTOOL_HELP> qgnss_help_;

  // QSettings settings_;

  QList<QString> baudrates_;

  QList<QString> downmode_;

  QList<QString> tabelheaderlabels_;
};

#endif // GOKE_BASE_WINDOW_H
