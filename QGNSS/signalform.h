#ifndef SIGNALFORM_H
#define SIGNALFORM_H

#include <QWidget>
#include "DataParser/nmea_parser.h"

//enum NATIONAL {
//    USA,
//    RUSSIA,
//    CHINA,
//    EUROPE,
//    JAPAN,
//    INDIA,
//    Sabs,
//    GLOBAL
//};

namespace Ui {
class SignalForm;
}

class SignalForm : public QWidget
{
    Q_OBJECT

public:
    explicit SignalForm(QWidget *parent = nullptr);
    ~SignalForm();

//    void drowFlag(const SAT_Info_t info,const NATIONAL national,const QString &band);
    void drowSat(const SignalInfo_t sat);
    void clearData(void);

private:
    Ui::SignalForm *ui;

    SAT_Info_t svsinfo;
    SignalInfo_t SatelliteInfo;

    /* MSM signal ID table -------------------------------------------------------*/
    const char *Freq_sig_gps[32] = {
        /* GPS: ref [17] table 3.5-91 */
        "1", "L1", "L1", "L1", "5", "6", "7", "L2", "L2", "L2", "11", "12",
        "13", "14", "L2", "L2", "L2", "18", "19", "20", "21", "L5", "L5", "L5",
        "25", "26", "27", "28", "29", "L1", "L1", "L1"};
    const char *Freq_sig_glo[32] = {
        /* GLONASS: ref [17] table 3.5-96 */
        "1", "G1", "G1", "4", "5", "6", "7", "G2", "G2", "10", "11", "12",
        "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24",
        "25", "26", "27", "28", "29", "30", "31", "32"};
    const char *Freq_sig_gal[32] = {
        /* Galileo: ref [17] table 3.5-99 */
        "1", "E1", "E1", "E1", "E1", "E1", "7", "E6", "E6", "E6", "E6", "E6",
        "13", "E5B", "E5B", "E5B", "17", "E5", "E5", "E5", "21", "E5A", "E5A", "E5A",
        "25", "26", "27", "28", "29", "30", "31", "32"};
    const char *Freq_sig_qzs[32] = {
        /* QZSS: ref [17] table 3.5-105 */
        "1", "L1", "3", "4", "5", "6", "7", "8", "LEX", "LEX", "LEX", "12",
        "13", "14", "L2", "L2", "L2", "18", "19", "20", "21", "L5", "L5", "L5",
        "25", "26", "27", "28", "29", "L1", "L1", "L1"};
    const char *Freq_sig_sbs[32] = {
        /* SBAS: ref [17] table 3.5-102 */
        "1", "L1", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12",
        "13", "14", "15", "16", "17", "18", "19", "20", "21", "L5", "L5", "L5",
        "25", "26", "27", "28", "29", "30", "31", "32"};
    const char *Freq_sig_cmp[32] = {
        /* BeiDou: ref [17] table 3.5-108 */
        "1", "B1", "B1", "B1", "5", "6", "7", "B3", "B3", "B3", "11", "12",
        "13", "B2", "B2", "B2", "17", "18", "19", "20", "21", "B2a", "B2a", "B2a",
        "25", "26", "27", "28", "29", "B1c", "B1c", "B1c"};
    const char *Freq_sig_irn[32] = {
        /* NavIC/IRNSS: ref [17] table 3.5-108.3 */
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12",
        "13", "14", "15", "16", "17", "18", "19", "20", "21", "L5", "23", "24",
        "25", "26", "27", "28", "29", "30", "31", "32"};
//    NATIONAL national;

    QMap<char, QString> gpNumOfBaud = {{0, "-"}, {1, "L1"}, {2, "L1"}, {3, "L1"}, {4, "L2"}, {5, "L2"}, {6, "L2"}, {7, "L5"}, {8, "L5"}};

    QMap<char, QString> gqNumOfBaud = {{0, "-"}, {1, "L1"}, {2, "L1"}, {3, "L1"}, {4, "LIS"}, {5, "L2"}, {6, "L2"}, {7, "L5"}, {8, "L5"}, {9, "L6"}, {10, "L6"}};

    QMap<char, QString> glNumOfBaud = {{0, "-"}, {1, "G1"}, {2, "G1"}, {3, "G2"}, {4, "G2"}};

    QMap<char, QString> gbNumOfBaud = {{0, "-"}, {1, "B1I"}, {2, "B1Q"}, {3, "B1C"}, {4, "B1A"}, {5, "B2a"}, {6, "B2b"}, {7, "B2a+b"}, {8, "B3I"}, {9, "B3Q"}, {10, "B3A"}, {12, "B2I"}, {13, "B2Q"}};

    QMap<char, QString> gaNumOfBaud = {{0, "-"}, {1, "E5a"}, {2, "E5b"}, {3, "E5a+b"}, {4, "E6-A"}, {5, "E6-BC"}, {6, "L1-A"}, {7, "L1-BC"}};

    QMap<char, QString> giNumOfBaud = {{0, "-"}, {1, "L5"}, {2, "S"}, {3, "L5"}, {4, "S"}, {5, "L1"}};
protected:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // SIGNALFORM_H
