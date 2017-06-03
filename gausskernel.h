#ifndef GAUSSKERNEL_H
#define GAUSSKERNEL_H

#include <QWidget>

namespace Ui {
class GaussKernel;
}

class GaussKernel : public QWidget
{
    Q_OBJECT

public:
    explicit GaussKernel(QWidget *parent = 0);
    ~GaussKernel();

private slots:
    void on_sbRadius_valueChanged(int arg1);
    void on_leSigma_textChanged(const QString &arg1);

private:
    Ui::GaussKernel *ui;
};

#endif // GAUSSKERNEL_H
