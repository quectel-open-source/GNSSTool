#include "logplot_mainwindow.h"
//#include "FilterOptionsDialog.h"
#include <QApplication>
#include "qnmea.h"
#include "nmea_dt.h"
#include "qlog_data_center.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    logPlot_MainWindow w;
    w.show();
    set_qlog_data_cb(logPlot_MainWindow::get_log_data_cb,(void *)&w);
    set_qlog_status_cb(logPlot_MainWindow::get_log_status_cb,(void *)&w);
    Set_Read_PKG(1024*1024);
    if((w.firstOpenHelpRead()>0) || (w.firstOpenHelpReadTwo() !=  QHostInfo::localHostName())){
        QMessageBox MyBox(QMessageBox::Question,"","");
        MyBox.setParent(&w);
        MyBox.setWindowFlags(Qt::Dialog);
        MyBox.setWindowTitle("Novice Guide");
        MyBox.setText("If you are using this software for the first time, click the ''YES'' button to jump to the help manual interface. If not, please click ''NO''.");
        w.agreeBut = MyBox.addButton("YES", QMessageBox::AcceptRole);
        w.disagreeBut = MyBox.addButton("NO", QMessageBox::RejectRole);
        QObject::connect(&MyBox,&QMessageBox::buttonClicked,&w,&logPlot_MainWindow::buttonClicked);
        MyBox.exec();
    }
    return a.exec();
}
