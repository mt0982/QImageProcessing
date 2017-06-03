#ifndef GAUSSKERNEL_H
#define GAUSSKERNEL_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class GaussKernel;
}

class GaussKernel : public QWidget
{
    Q_OBJECT

public:
    explicit GaussKernel(QWidget *parent = 0);
    ~GaussKernel();

    void setImage(const QImage &value);

private slots:
    void on_sbRadius_valueChanged(int arg1);
    void on_leSigma_textChanged(const QString &arg1);
    void on_pbCalculate_clicked();

signals:
//    void signalCalculated(QString);

private:
    Ui::GaussKernel *ui;

    QImage image;
    void gaussianFilter();
};

#endif // GAUSSKERNEL_H
