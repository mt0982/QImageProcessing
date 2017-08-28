#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QtCharts>
#include <QDebug>

using namespace QtCharts;

namespace Ui {
class Histogram;
}

class Histogram: public QWidget {
    Q_OBJECT

public:
    explicit Histogram(QWidget *parent = 0);
    ~Histogram();

private:
    Ui::Histogram *ui;

    QVector<int> vec_red, vec_green, vec_blue;
    QChart *chart;

    void configureBarChart();
    void getImageData();
};

#endif // HISTOGRAM_H
















