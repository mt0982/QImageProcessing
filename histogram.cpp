#include "histogram.h"
#include "ui_histogram.h"

Histogram::Histogram(QWidget *parent) : QWidget(parent), ui(new Ui::Histogram)
{
    ui->setupUi(this);

    QImage image("/home/asus/Programy/Qt/Projekty/Project_PoC/Pictures/lena.png");

    QVector<int> red(255), green(255), blue(255);
    red.fill(0);
    green.fill(0);
    blue.fill(0);

    QRgb *ptr_image = (QRgb*)image.bits();
    for (int i = 0; i < image.width() * image.height(); ++i) {
        red[qRed(ptr_image[i])]++;
        green[qGreen(ptr_image[i])]++;
        blue[qBlue(ptr_image[i])]++;
    }

    QSplineSeries *series_red = new QSplineSeries();
    QSplineSeries *series_green = new QSplineSeries();
    QSplineSeries *series_blue = new QSplineSeries();

    series_red->setColor(Qt::red);
    series_green->setColor(Qt::green);
    series_blue->setColor(Qt::blue);

    for (int value = 0; value < 256; ++value) {

        series_red->append(value, red[value]);
        series_green->append(value, green[value]);
        series_blue->append(value, blue[value]);
    }

    /* Axis Category */
//    QCategoryAxis *category = new QCategoryAxis;
//    category->append("Low", 5);
//    category->append("Medium", 800);
//    category->append("High", 1200);
    QValueAxis *axisY = new QValueAxis;
    QValueAxis *axisX = new QValueAxis;

    /* Chart */
    QChart *chart = new QChart();
    chart->addSeries(series_red);
    chart->addSeries(series_green);
    chart->addSeries(series_blue);
    //chart->setTitle("Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addAxis(axisX, Qt::AlignBottom);
    series_red->attachAxis(axisX);
    series_red->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->chart->setRenderHints(QPainter::Antialiasing);
    ui->chart->setChart(chart);
}

Histogram::~Histogram()
{
    delete ui;
}













