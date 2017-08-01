#ifndef GAUSSKERNEL_H
#define GAUSSKERNEL_H

#include <QWidget>
#include <QDebug>
#include <Pattern/facadeimage.h>

namespace Ui {
class GaussKernel;
}

class GaussUnsharpFilter : public FacadeImage {
    Q_OBJECT

public:
    explicit GaussUnsharpFilter(FacadeImage *parent = 0);
    ~GaussUnsharpFilter();

    void gaussianFilterFast();
    void gaussianFilterFastCanny(int radius, int sigma);

private slots:
    void on_sbRadius_valueChanged(int arg1);
    void on_leSigma_textChanged(const QString &arg1);
    void on_pbCalculate_clicked();
    void on_pbUnsharp_clicked();

private:
    Ui::GaussKernel *ui;

    QImage gaussImage;

    float mask_sum;

    void gaussianFilter();
    void unsharpFilter();
};

#endif // GAUSSKERNEL_H










