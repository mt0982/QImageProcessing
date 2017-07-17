#ifndef GAUSSKERNEL_H
#define GAUSSKERNEL_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class GaussKernel;
}

class GaussUnsharpFilter : public QWidget {
    Q_OBJECT

public:
    explicit GaussUnsharpFilter(QWidget *parent = 0);
    ~GaussUnsharpFilter();

    void setImage(const QImage &value);
    void gaussianFilterFast();
    void gaussianFilterFastCanny(int radius, int sigma);

private slots:
    void on_sbRadius_valueChanged(int arg1);
    void on_leSigma_textChanged(const QString &arg1);
    void on_pbCalculate_clicked();
    void on_pbUnsharp_clicked();

signals:
    void sendImage(QImage);

private:
    Ui::GaussKernel *ui;

    QImage image;
    QImage gaussImage;

    float mask_sum;

    void gaussianFilter();
    void unsharpFilter();
};

#endif // GAUSSKERNEL_H










