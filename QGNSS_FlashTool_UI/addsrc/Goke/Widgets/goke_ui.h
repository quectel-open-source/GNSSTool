#ifndef GOKE_UI_H
#define GOKE_UI_H

#include <QWidget>
#include "goke_base_window.h"
#include "goke_model.h"
class Goke_UI : public Goke_base_window
{
   Q_OBJECT
 public:
  Goke_UI(QWidget *parent = nullptr);
  ~Goke_UI();
  enum cfg_key{
    baudrate,
    downmode,
    DApath,
    ROMpath
  };
  Q_ENUM (cfg_key)

  Q_INVOKABLE void start() override;
  Q_INVOKABLE void stop() override;
  Q_INVOKABLE  void ini_IO(IODvice *io) override;

  void cfg_widget() override;
  void cfg_construct();
  void open_file(bool DAbt) ;
  void load_file(bool DAbt) ;

  bool event(QEvent *event) override;
  Goke_Model *GokeModel=nullptr;

  // MTK_base_Window interface
 public:
  virtual void wgt_enable(bool Run) override;
};

#endif // GOKE_UI_H
