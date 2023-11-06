#ifndef SIGNALLEVELWINDOW_H
#define SIGNALLEVELWINDOW_H

#define MAX_SIGNAL 160

#include <QWidget>
#include <QMap>
#include <QSet>

#include "DataParser/nmea_parser.h"
#include "signalform.h"
#include "common.h"

namespace Ui
{
    class SignalLevelWindow;
}

class SignalLevelWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SignalLevelWindow(QWidget *parent = nullptr);
    ~SignalLevelWindow();

//    void updateGNSSView(GSV_t &gsv_t);
    void updateGNSSView(Solution_t &sol);
    //    bool prn2National(SAT_Info_t var, NATIONAL &national,QString &signalFreq);

private:
    Ui::SignalLevelWindow *ui;


    QSet<int> displaySet{SatSys::GPS, SatSys::QZSS,
                         SatSys::SBAS, SatSys::IRNSS,
                         SatSys::BeiDou, SatSys::GLONASS, SatSys::Galileo};
};

#endif // SIGNALLEVELWINDOW_H
