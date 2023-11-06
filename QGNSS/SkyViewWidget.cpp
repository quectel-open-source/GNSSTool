#include "SkyViewWidget.h"
#include "ui_SkyViewWidget.h"
#include "RTKLib/rtklib.h"

SkyViewWidget::SkyViewWidget(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::SkyViewWidget)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/skyMap_windows_background.png"));
    resize(500, 330);
    init_skymap_widget();
}

SkyViewWidget::~SkyViewWidget()
{
    delete ui;
}

void SkyViewWidget::init_skymap_widget(void)
{
    QColor hourColor(127, 0, 127);

    //![3]
    QPainterPath starPath;
    starPath.moveTo(28, 15);
    for (int i = 1; i < 6; ++i)
    {
        starPath.lineTo(14 + 14 * qCos(0.8 * i * M_PI),
                        15 + 14 * qSin(0.8 * i * M_PI));
    }
    starPath.closeSubpath();

    QImage star(30, 30, QImage::Format_ARGB32);
    star.fill(Qt::transparent);

    // points data prepare
    // QColor hourColor(127, 0, 127);
    QRgb mRgb = qRgb(hourColor.red(), hourColor.green(), hourColor.blue());
    QPainter painter(&star);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QRgb(mRgb));
    painter.setBrush(painter.pen().color());
    painter.drawPath(starPath);
}

void SkyViewWidget::update_skymap_view()
{
    //    if (gsv_t->received || gsv_t->timer.elapsed() > 80)
    DrawPlot(ui->Disp1, ui->constellationsLabel, 2, 1);
}

// draw text ----------------------------------------------------------------
void SkyViewWidget::DrawText1(QPainter *c, int x, int y, const QString &s,
                              const QColor &color, int align)
{
    int flags = 0;
    switch (align)
    {
    case 0:
        flags |= Qt::AlignLeft;
        break;
    case 1:
        flags |= Qt::AlignHCenter;
        break;
    case 2:
        flags |= Qt::AlignRight;
        break;
    }

    QFont font;
    font.setFamily("Arial");
    font.setPointSize(8);
    font.setBold(false);
    c->setFont(font);

    QRectF off = c->boundingRect(QRectF(), flags, s);

    c->setPen(color);

    c->translate(x, y);
    c->drawText(off, s);
    c->translate(-x, -y);
}

// draw satellites in skyplot -----------------------------------------------
void SkyViewWidget::DrawSat(QPainter *c, QPainter *c2, int w, int h, int x0, int y0,
                            int index, int freq)
{
    QString s;
    QPoint p(w / 2, h / 2);
    double r = MIN(w * 0.95, h * 0.95) / 2;
    int i, k, l, d, x[MAXSAT], y[MAXSAT], ns = 0, f = !freq ? 0 : freq - 1;
    char id[16], sys[] = "GREJCS", *q;

    QStringList gps_categories;
    QStringList glo_categories;
    QStringList gal_categories;
    QStringList bds_categories;
    QStringList qzss_categories;
    QStringList sbas_categories;
    QStringList navic_categories;

    QRectF target(0, 0, 16, 16);
    QRectF source(0.0, 0.0, 218, 218);

    int iconWidthHeight = (r / 10 < 20) ? 20 : r / 10;
    int labelIconWidthHeight = 18;

    DrawSky(c, w, h, x0, y0);

    QFont font;
    font.setFamily("Arial");
    font.setPointSize(8);
    font.setBold(false);
    c->setFont(font);
    i = 0;
    foreach (auto var, sol->SatInfo)
    {
        if (var.Elevation <= 0.0 || var.SNR <= 0)
            continue;
        y[i] = static_cast<int>(p.y() - r * (90 - var.Elevation) / 90 * cos(var.Azimuth * D2R)) + y0;
        x[i] = static_cast<int>(p.x() + r * (90 - var.Elevation) / 90 * sin(var.Azimuth * D2R)) + x0;
        d = SATSIZE / 2;
        c->setOpacity(1);
        QImage im;
        target.setRect(x[i] - d, y[i] - d, iconWidthHeight, iconWidthHeight);
        switch (var.Sys)
        {
        case SatSys::GPS:
        {
            c->setPen(Qt::magenta);
            c->setBrush(Qt::magenta); //(QColor(0xf1,0xD0,0xD0));
            im.load(":/images/DM_USA_Cycle.png");
            c->drawImage(target, im, source);
            c->setOpacity(1);
            DrawText1(c, x[i] - 10 + iconWidthHeight / 2, y[i] - d + iconWidthHeight, QString("G%1").arg(var.PRN), Qt::magenta, 1);
            gps_categories.append(QString("G%1").arg((int)var.PRN));
            break;
        }
        case SatSys::QZSS:
        {
            c->setPen(Qt::yellow);
            c->setBrush(Qt::yellow); //(QColor(0xf1,0xD0,0xD0));
            im.load(":/images/DM_JAPAN_Cycle.png");
            c->drawImage(target, im, source);
            c->setOpacity(1);
            DrawText1(c, x[i] - 10 + iconWidthHeight / 2, y[i] - d + iconWidthHeight, QString("Q%1").arg(var.PRN % 100), Qt::yellow, 1);
            /*if CN0>0, counter constellations sv numbers  */
            qzss_categories.append(QString("Q%1").arg(var.PRN));
            break;
        }
        case SatSys::SBAS:
        {
            c->setPen(Qt::blue);
            c->setBrush(Qt::blue); //(QColor(0xf1,0xD0,0xD0));
            im.load(":/images/DM_SBAS_Cycle.png");
            c->drawImage(target, im, source);
            c->setOpacity(1);
            DrawText1(c, x[i] - 10 + iconWidthHeight / 2, y[i] - d + iconWidthHeight, QString("S%1").arg(var.PRN % 100), Qt::blue, 1);
            /*if CN0>0, counter constellations sv numbers  */
            sbas_categories.append(QString("S%1").arg(var.PRN));
        }
            break;
        case SatSys::IRNSS:
        {
            c->setPen(Qt::cyan);
            c->setBrush(Qt::cyan); //(QColor(0xf1,0xD0,0xD0));
            im.load(":/images/DM_INDIAN_Cycle.png");
            c->drawImage(target, im, source);
            c->setOpacity(1);
            DrawText1(c, x[i] - 10 + iconWidthHeight / 2, y[i] - d + iconWidthHeight, QString("I%1").arg(var.PRN % 100), Qt::cyan, 1);
            /*if CN0>0, counter constellations sv numbers  */
            navic_categories.append(QString("I%1").arg(var.PRN));
            break;
        }
        case SatSys::BeiDou:
        {
            c->setPen(Qt::red);
            c->setBrush(Qt::red); //(QColor(0xf1,0xD0,0xD0));
            im.load(":/images/DM_China_Cycle.png");
            c->drawImage(target, im, source);
            c->setOpacity(1);
            DrawText1(c, x[i] - 10 + iconWidthHeight / 2, y[i] - d + iconWidthHeight, QString("B%1").arg(var.PRN % 100), Qt::red, 1);
            /*if CN0>0, counter constellations sv numbers  */
            bds_categories.append(QString("B%1").arg(var.PRN));
            break;
        }
        case SatSys::GLONASS:
        {
            c->setPen(Qt::black);
            c->setBrush(Qt::black); //(QColor(0xf1,0xD0,0xD0));
            im.load(":/images/DM_Russian_Cycle.png");
            c->drawImage(target, im, source);
            c->setOpacity(1);
            DrawText1(c, x[i] - 10 + iconWidthHeight / 2, y[i] - d + iconWidthHeight, QString("R%1").arg(var.PRN % 100), Qt::black, 1);
            /*if CN0>0, counter constellations sv numbers  */
            glo_categories.append(QString("R%1").arg(var.PRN));
            break;
        }
        case SatSys::Galileo:
        {
            c->setPen(Qt::green);
            c->setBrush(Qt::green); //(QColor(0xf1,0xD0,0xD0));
            im.load(":/images/DM_Europe_Cycle.png");
            c->drawImage(target, im, source);
            c->setOpacity(1);
            DrawText1(c, x[i] - 10 + iconWidthHeight / 2, y[i] - d + iconWidthHeight, QString("E%1").arg(var.PRN % 100), Qt::green, 1);
            /*if CN0>0, counter constellations sv numbers  */
            gal_categories.append(QString("E%1").arg(var.PRN));
            break;
        }
        default:
            break;
        }
    }


    QSet<QString> gps_set = gps_categories.toSet();
    QSet<QString> glo_set = glo_categories.toSet();
    QSet<QString> gal_set = gal_categories.toSet();
    QSet<QString> bds_set = bds_categories.toSet();
    QSet<QString> qzss_set = qzss_categories.toSet();
    QSet<QString> sbas_set = sbas_categories.toSet();
    QSet<QString> navic_set = navic_categories.toSet();

    /* constellations image & label */
    QImage im_china;
    im_china.load(":/images/DM_China_Cycle.png");
    // c2->drawImage(x0+2,y0+10+ 0,im_china,0, 0, 16, 16);
    target.setRect(x0 + 2, y0 + 10 + 0, labelIconWidthHeight, labelIconWidthHeight);
    c2->drawImage(target, im_china, source);
    DrawText1(c2, x0 + 30, y0 + 10 + 0, QString("BDS :%1").arg((int)bds_set.count()), Qt::red, 0);

    QImage im_russia;
    im_russia.load(":/images/DM_Russian_Cycle.png");
    // c2->drawImage(x0+2,y0+10+ 16*2,im_russia,0, 0, 16, 16);
    target.setRect(x0 + 2, y0 + 10 + 16 * 2, labelIconWidthHeight, labelIconWidthHeight);
    c2->drawImage(target, im_russia, source);
    DrawText1(c2, x0 + 30, y0 + 10 + 16 * 2, QString("GLO :%1").arg((int)glo_set.count()), Qt::black, 0);

    QImage im_usa;
    im_usa.load(":/images/DM_USA_Cycle.png");
    // c2->drawImage(x0+2,y0+10+ 16*4,im_usa,0, 0, 16, 16);
    target.setRect(x0 + 2, y0 + 10 + 16 * 4, labelIconWidthHeight, labelIconWidthHeight);
    c2->drawImage(target, im_usa, source);

    DrawText1(c2, x0 + 30, y0 + 10 + 16 * 4, QString("GPS :%1").arg((int)gps_set.count()), Qt::magenta, 0);

    QImage im_europe;
    im_europe.load(":/images/DM_Europe_Cycle.png");
    // c2->drawImage(x0+2,y0+10+ 16*6,im_europe,0, 0, 16, 16);
    target.setRect(x0 + 2, y0 + 10 + 16 * 6, labelIconWidthHeight, labelIconWidthHeight);
    c2->drawImage(target, im_europe, source);

    DrawText1(c2, x0 + 30, y0 + 10 + 16 * 6, QString("GAL :%1").arg((int)gal_set.count()), Qt::blue, 0);

    QImage im_japan;
    im_japan.load(":/images/DM_JAPAN_Cycle.png");
    // c2->drawImage(x0+2,y0+10+ 16*8,im_japan,0, 0, 16, 16);
    target.setRect(x0 + 2, y0 + 10 + 16 * 8, labelIconWidthHeight, labelIconWidthHeight);
    c2->drawImage(target, im_japan, source);
    /* TODO: add QZSS size here */

    DrawText1(c2, x0 + 30, y0 + 10 + 16 * 8, QString("QZSS:%1").arg((int)qzss_set.count()), Qt::blue, 0);

    QImage im_navic;
    im_navic.load(":/images/DM_INDIAN_Cycle.png");
    // c2->drawImage(x0+2,y0+10+ 16*10,im_navic,0, 0, 16, 16);
    target.setRect(x0 + 2, y0 + 10 + 16 * 10, labelIconWidthHeight, labelIconWidthHeight);
    c2->drawImage(target, im_navic, source);
    /* TODO: add NAVIC size here */
    DrawText1(c2, x0 + 30, y0 + 10 + 16 * 10, QString("NAVIC:%1").arg((int)navic_set.count()), Qt::blue, 0);

    QImage im_sbas;
    im_sbas.load(":/images/DM_SBAS_Cycle.png");
    // c2->drawImage(x0+2,y0+10+ 16*12,im_sbas,0, 0, 16, 16);
    target.setRect(x0 + 2, y0 + 10 + 16 * 12, labelIconWidthHeight, labelIconWidthHeight);
    c2->drawImage(target, im_sbas, source);
    /* TODO: add SBAS size here */
    DrawText1(c2, x0 + 30, y0 + 10 + 16 * 12, QString("SBAS:%1").arg((int)sbas_set.count()), Qt::blue, 0);
}

// draw skyplot -------------------------------------------------------------
void SkyViewWidget::DrawSky(QPainter *c, int w, int h, int x0, int y0)
{
    QColor minuteColor(0, 127, 127, 191);
    QString label[] = {tr("N"), tr("E"), tr("S"), tr("W")};
    QPoint p(x0 + w / 2, y0 + h / 2);
    double r = MIN(w * 0.95, h * 0.95) / 2;
    int a, e, d, x, y;
    char ele_mark[12] = "";

    c->setBrush(Qt::transparent);
    c->setOpacity(1);
    for (e = 0; e < 90; e += 30)
    {
        d = static_cast<int>(r * (90 - e) / 90);
        c->setPen(minuteColor);
        c->drawEllipse(p.x() - d, p.y() - d, 2 * d + 1, 2 * d + 1);
    }
    /* elevation 10 degree */

    c->setBrush(Qt::transparent);
    d = static_cast<int>(r * (90 - 10) / 90);
    c->setPen(minuteColor);
    c->drawEllipse(p.x() - d, p.y() - d, 2 * d + 1, 2 * d + 1);

    c->setBrush(Qt::transparent);
    c->setOpacity(1); /* filled  */
    for (a = 0; a < 360; a += 30)
    {
        x = static_cast<int>(r * sin(a * D2R));
        y = static_cast<int>(r * cos(a * D2R));
        c->setPen(minuteColor);
        c->setOpacity(1);
        c->drawLine(p.x(), p.y(), p.x() + x, p.y() - y);

        /*  draw N E S W */
        if (a == 0)
        {
            DrawText1(c, p.x() + x + 8, p.y() - y, label[a / 90], Qt::darkMagenta, 1);
        }
        else if (a == 90)
        {
            DrawText1(c, p.x() + x - 2, p.y() - y, label[a / 90], Qt::darkMagenta, 1);
        }
        else if (a == 180)
        {
            DrawText1(c, p.x() + x - 4, p.y() - y - 12, label[a / 90], Qt::darkMagenta, 1);
        }
        else if (a == 270)
        {
            DrawText1(c, p.x() + x + 8, p.y() - y - 12, label[a / 90], Qt::darkMagenta, 1);
        }

        /*  draw zaimuth: 30,60  120,150 210,240  300,330*/
        if (a == 30)
        {
            DrawText1(c, p.x() + x + 2, p.y() - y - 14, tr("30"), Qt::darkMagenta, 1);
        }
        else if (a == 60)
        {
            DrawText1(c, p.x() + x + 10, p.y() - y - 5, tr("60"), Qt::darkMagenta, 1);
        }
        else if (a == 120)
        {
            DrawText1(c, p.x() + x + 10, p.y() - y, tr("120"), Qt::darkMagenta, 1);
        }
        else if (a == 150)
        {
            DrawText1(c, p.x() + x + 10, p.y() - y, tr("150"), Qt::darkMagenta, 1);
        }
        else if (a == 210)
        {
            DrawText1(c, p.x() + x - 10, p.y() - y, tr("210"), Qt::darkMagenta, 1);
        }
        else if (a == 240)
        {
            DrawText1(c, p.x() + x - 10, p.y() - y, tr("240"), Qt::darkMagenta, 1);
        }
        else if (a == 300)
        {
            DrawText1(c, p.x() + x - 15, p.y() - y - 10, tr("300"), Qt::darkMagenta, 1);
        }
        else if (a == 330)
        {
            DrawText1(c, p.x() + x - 10, p.y() - y - 15, tr("330"), Qt::darkMagenta, 1);
        }
    }

    /*draw elevation:90,60,30,10,0 degree */

    DrawText1(c, p.x() - 10, p.y(), tr("90"), Qt::darkMagenta, 1);
    DrawText1(c, p.x() - 10, p.y() - r / 3.0, tr("60"), Qt::darkMagenta, 1);
    DrawText1(c, p.x() - 10, p.y() - r * (2.0 / 3.0), tr("30"), Qt::darkMagenta, 1);
    DrawText1(c, p.x() - 10, p.y() - r * (8.0 / 9.0), tr("10"), Qt::gray, 1);
    DrawText1(c, p.x() - 10, p.y() - r, tr(" 0"), Qt::darkMagenta, 1);
}

// draw solution plot -------------------------------------------------------
void SkyViewWidget::DrawPlot(QLabel *plot, QLabel *plot2, int type, int freq)
{

    if (!plot)
        return;

    QPixmap buffer(plot->size());

    if (buffer.isNull())
        return;

    buffer.fill(Qt::transparent);

    QPainter *c = new QPainter(&buffer);
    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(10);
    c->setFont(font);
    int w = buffer.size().width() - 10, h = buffer.height() - 10;

    // c->setBrush(Qt::white);
    // c->fillRect(buffer.rect(),QBrush(Qt::white));
    c->setBrush(Qt::transparent);
    c->fillRect(buffer.rect(), QBrush(Qt::transparent));

    // draw constellation images
    if (!plot2)
        return;

    QPixmap buffer2(plot2->size());

    if (buffer2.isNull())
        return;

    buffer2.fill(Qt::transparent);

    QPainter *c2 = new QPainter(&buffer2);
    QFont font2;
    font2.setFamily("Arial");
    font2.setPixelSize(10);
    c2->setFont(font2);
    c2->setBrush(Qt::transparent);
    c2->fillRect(buffer2.rect(), QBrush(Qt::transparent));

    DrawSat(c, c2, w, h, 0, 0, 0, freq);

    plot2->setPixmap(buffer2);
    plot->setPixmap(buffer);

    delete c;
    delete c2;
}

void SkyViewWidget::setSol(Solution_t *value)
{
    sol = value;
}

void SkyViewWidget::setGsv_t(GSV_t *value)
{
    gsv_t = value;
}

void SkyViewWidget::resizeEvent(QResizeEvent *event)
{
    update_skymap_view();
}
