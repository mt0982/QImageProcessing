#include "histogram.h"
#include "ui_histogram.h"

Histogram::Histogram(FacadeImage *parent) : FacadeImage(parent), ui(new Ui::Histogram),
    vec_red(256), vec_green(256), vec_blue(256), isCreated(false)
{
    ui->setupUi(this);
    setWindowTitle("Data Visualisation");

    chart = new QChart();

    /* Set Theme Connect */
    connect(ui->cbTheme, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](const int index) {

        switch (index) {
        case 0:
            chart->setTheme(QChart::ChartThemeLight);
            break;
        case 1:
            chart->setTheme(QChart::ChartThemeBlueCerulean);
            break;
        case 2:
            chart->setTheme(QChart::ChartThemeDark);
            break;
        case 3:
            chart->setTheme(QChart::ChartThemeBrownSand);
            break;
        case 4:
            chart->setTheme(QChart::ChartThemeBlueNcs);
            break;
        case 5:
            chart->setTheme(QChart::ChartThemeHighContrast);
            break;
        case 6:
            chart->setTheme(QChart::ChartThemeBlueIcy);
            break;
        case 7:
            chart->setTheme(QChart::ChartThemeQt);
            break;
        default:
            break;
        }

        if (ui->cbChartType->currentIndex() == 0) setBarChart();
        else if (ui->cbChartType->currentIndex() == 1) setPieChart();
    });

    /* Set Chart Type Connect */
    connect(ui->cbChartType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](const int index) {

        //if (index == 0) setBarChart();
        //else if (index == 1) setPieChart();
    });

    ui->chartView->setRenderHints(QPainter::Antialiasing);
    ui->chartView->setChart(chart);
}

Histogram::~Histogram()
{
    delete ui;
}

void Histogram::setBarChart()
{
    /* Clear Chart Series */
    chart->removeAllSeries();

    QSplineSeries *series_red = new QSplineSeries();
    QSplineSeries *series_green = new QSplineSeries();
    QSplineSeries *series_blue = new QSplineSeries();

    /* Set Colors: Custom | Standard */
    if (ui->cbColor->isChecked()) {
        series_red->setColor(Qt::red);
        series_green->setColor(Qt::green);
        series_blue->setColor(Qt::blue);
    }

    /* Initialize Series */
    for (int value = 0; value < 256; ++value) {

        series_red->append(value, vec_red[value]);
        series_green->append(value, vec_green[value]);
        series_blue->append(value, vec_blue[value]);
    }

    /* Chart */
    chart->addSeries(series_red);
    chart->addSeries(series_green);
    chart->addSeries(series_blue);
    //chart->setTitle("Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    /* Add Axes Only Once */
    if (!isCreated) {

        /* Axis Category */
        QValueAxis *axisY = new QValueAxis;
        QValueAxis *axisX = new QValueAxis;

        chart->addAxis(axisY, Qt::AlignLeft);
        chart->addAxis(axisX, Qt::AlignBottom);

        series_red->attachAxis(axisX);
        series_red->attachAxis(axisY);
    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    isCreated = true;
}

void Histogram::setPieChart()
{
//    /* Clear Chart Series */
//    chart->removeAllSeries();

//    QPieSeries *series = new QPieSeries();
//    series->append("Jane", 1);
//    series->append("Joe", 2);
//    series->append("Andy", 3);
//    series->append("Barbara", 4);
//    series->append("Axel", 5);

//    QPieSlice *slice = series->slices().at(1);
//    slice->setExploded();
//    slice->setLabelVisible();
//    slice->setPen(QPen(Qt::darkGreen, 2));
//    slice->setBrush(Qt::green);

//    chart->addSeries(series);
//    //chart->setTitle("Simple piechart example");
//    //chart->legend()->hide();
}

void Histogram::getImageData()
{
    vec_red.fill(0);
    vec_green.fill(0);
    vec_blue.fill(0);

    QRgb *ptr_image = (QRgb*)image.bits();
    for (int i = 0; i < image.width() * image.height(); ++i) {
        vec_red[qRed(ptr_image[i])]++;
        vec_green[qGreen(ptr_image[i])]++;
        vec_blue[qBlue(ptr_image[i])]++;
    }
}

void Histogram::on_cbColor_clicked()
{
    setBarChart();
}

void Histogram::setImage(const QImage &value)
{
    image = value;
    getImageData();
    setBarChart();
}


















