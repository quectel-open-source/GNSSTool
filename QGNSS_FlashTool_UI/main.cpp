#include<stdio.h>
#include<QApplication>
#include<QDebug>
#include<QFont>
#include<QTextCodec>
#include "qgnss_flashtool.h"


int main(int arg,char*args[]){


#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif
  QApplication app(arg,args);


  auto AppMange_=QGNSS_FLASHTOOL::get_instance ();
  //AppMange_->quickWidget_show ();
  return app.exec();
}
