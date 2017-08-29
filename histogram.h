#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QtCharts>
#include <QDebug>
#include "Pattern/facadeimage.h"

using namespace QtCharts;

namespace Ui {
class Histogram;
}

class Histogram: public FacadeImage {
    Q_OBJECT

public:
    explicit Histogram(FacadeImage *parent = 0);
    ~Histogram();

    virtual void setImage(const QImage &value) override;

private slots:
    void on_cbColor_clicked();

private:
    Ui::Histogram *ui;

    QVector<int> vec_red, vec_green, vec_blue;
    QChart *chart;
    bool isCreated;

    void setBarChart();
    void setPieChart();
    void getImageData();
};

#endif // HISTOGRAM_H
















