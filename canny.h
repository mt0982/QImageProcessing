#ifndef CANNY_H
#define CANNY_H

#include <QWidget>
#include <QImage>
#include <QDebug>
#include <QLabel>
#include <QtMath>
#include <QQueue>
#include <gausskernel.h>

namespace Ui {
class Canny;
}

class Canny : public QWidget {
    Q_OBJECT

public:
    explicit Canny(QWidget *parent = 0);
    ~Canny();

    void setImage(QImage value);
    void processImage();

private:
    Ui::Canny *ui;

    QImage image;
    GaussUnsharpFilter *gaussUnsharpFilter;
    QVector<int> xSobel, ySobel;
    int radius_gauss, tmin, tmax;

public slots:
    void overloadImage(QImage value);

signals:
    void sendImage(QImage);

private slots:
    void on_pbCalculate_clicked();
};

#endif // CANNY_H












