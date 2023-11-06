#ifndef MT3333_UI_H
#define MT3333_UI_H

#include <QWidget>
#include "mtk_base_window.h"
#include "mtk_model_3333.h"
class MT3333_UI : public MTK_base_Window
{
    Q_OBJECT
public:
    explicit MT3333_UI(QWidget *parent = nullptr);
    ~MT3333_UI();
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
     MTK_MODEL_3333 *MTKMODEL3333_=nullptr;

     // MTK_base_Window interface
public:
     virtual void wgt_enable(bool Run) override;
};

#endif // MT3333_UI_H
