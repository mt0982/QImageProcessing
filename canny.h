#ifndef CANNY_H
#define CANNY_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QtMath>
#include <QQueue>
#include <gausskernel.h>
#include <Pattern/facadeimage.h>

namespace Ui {
class Canny;
}

class Canny : public FacadeImage {
    Q_OBJECT

public:
    explicit Canny(FacadeImage *parent = 0);
    ~Canny();

    void processImage();
    void setImage(const QImage &value);

private:
    Ui::Canny *ui;

    GaussUnsharpFilter *gaussUnsharpFilter;
    QVector<int> xSobel, ySobel;
    int radius_gauss, tmin, tmax;

    void findPath(QImage &iCanny, int x, int y);

public slots:
    void overloadImage(QImage value);

private slots:
    void on_pbCalculate_clicked();
};

#endif // CANNY_H












