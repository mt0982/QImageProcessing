#include "histogram.h"
#include "ui_histogram.h"

Histogram::Histogram(QWidget *parent) : QWidget(parent), ui(new Ui::Histogram), vec_red(255), vec_green(255), vec_blue(255)
{
    ui->setupUi(this);

    chart = new QChart();

    vec_red.fill(0);
    vec_green.fill(0);
    vec_blue.fill(0);

    getImageData();
    configureBarChart();

    ui->barChart->setRenderHints(QPainter::Antialiasing);
    ui->barChart->setChart(chart);
}

Histogram::~Histogram()
{
    delete ui;
}

void Histogram::configureBarChart()
{
    QSplineSeries *series_red = new QSplineSeries();
    QSplineSeries *series_green = new QSplineSeries();
    QSplineSeries *series_blue = new QSplineSeries();

    series_red->setColor(Qt::red);
    series_green->setColor(Qt::green);
    series_blue->setColor(Qt::blue);

    for (int value = 0; value < 256; ++value) {

        series_red->append(value, vec_red[value]);
        series_green->append(value, vec_green[value]);
        series_blue->append(value, vec_blue[value]);
    }

    /* Axis Category */
    QValueAxis *axisY = new QValueAxis;
    QValueAxis *axisX = new QValueAxis;

    /* Chart */
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
}

void Histogram::getImageData()
{
    QImage image("/home/asus/Programy/Qt/Projekty/Project_PoC/Pictures/lena.png");

    QRgb *ptr_image = (QRgb*)image.bits();
    for (int i = 0; i < image.width() * image.height(); ++i) {
        vec_red[qRed(ptr_image[i])]++;
        vec_green[qGreen(ptr_image[i])]++;
        vec_blue[qBlue(ptr_image[i])]++;
    }
}













