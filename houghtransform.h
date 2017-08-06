#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <QWidget>
#include "Pattern/facadeimage.h"
#include "canny.h"

namespace Ui {
class HoughTransform;
}

class HoughTransform : public FacadeImage {
    Q_OBJECT

public:
    explicit HoughTransform(FacadeImage *parent = 0);
    ~HoughTransform();

private slots:
    void on_pbCalculate_clicked();

private:
    Ui::HoughTransform *ui;

    float **accumulator;
    Canny *canny;
};

#endif // HOUGHTRANSFORM_H
