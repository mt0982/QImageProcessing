#include "gausskernel.h"
#include "ui_gausskernel.h"

GaussKernel::GaussKernel(QWidget *parent): QWidget(parent), ui(new Ui::GaussKernel)
{
    ui->setupUi(this);
    setWindowTitle("Kernel");

    /* Initialize, Minimum */
    ui->tableGauss->setRowCount(3);
    ui->tableGauss->setColumnCount(3);

    for (int i = 0; i < 3; ++i) {
        ui->tableGauss->setRowHeight(i, 40);
        ui->tableGauss->setColumnWidth(i, 40);
    }

    mask_sum = 0;
}

GaussKernel::~GaussKernel()
{
    delete ui;
}

void GaussKernel::on_sbRadius_valueChanged(int arg1)
{
    /* Clear */
    ui->leSigma->clear();
    ui->tableGauss->clear();

    arg1 = arg1 * 2 + 1;

    ui->tableGauss->setRowCount(arg1);
    ui->tableGauss->setColumnCount(arg1);

    for (int i = 0; i < arg1; ++i) {
        ui->tableGauss->setRowHeight(i, 40);
        ui->tableGauss->setColumnWidth(i, 40);
    }
}

void GaussKernel::on_leSigma_textChanged(const QString &arg1)
{
    /* Clear */
    ui->tableGauss->clear();

    /* Parameters */
    int radius = ui->sbRadius->text().toInt();
    double sigma = arg1.toInt();

    /* Init cell */
    QTableWidgetItem *item;

    for (int x = 0; x < ui->tableGauss->rowCount(); ++x) {
        for (int y = 0; y < ui->tableGauss->colorCount(); ++y) {

            float value = exp(-(pow(x - radius,2) + pow(y - radius,2)) / (2 * pow(sigma,2)));
            item = new QTableWidgetItem;
            ui->tableGauss->setItem(x,y,item);
            item->setText(QString::number(value, 'g', 2));
        }
    }
}

void GaussKernel::on_pbCalculate_clicked()
{
    qDebug() << "GaussKernel::on_pbCalculate_clicked()";
    gaussianFilter();
}

void GaussKernel::setImage(const QImage &value)
{
    image = value;
}

void GaussKernel::gaussianFilter()
{
    /* Parameters */
    int radius = ui->sbRadius->value();
    int sum_r, sum_b, sum_g;
    QImage output(image.width(), image.height(), QImage::Format_RGB32);

    /* Sum of the Mask */
    mask_sum = 0;
    for (int i = 0; i < ui->tableGauss->rowCount(); ++i) {
        for (int j = 0; j < ui->tableGauss->columnCount(); ++j) {
            float value = ui->tableGauss->item(i, j)->text().toFloat();
            mask_sum += value;
        }
    }

    /* Convolution */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_copy = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            sum_r = sum_b = sum_g = 0;

            for (int i = -radius; i <= radius; ++i) {
                QRgb *ptr_original = (QRgb*)image.scanLine(abs(y + i));

                for (int j = -radius; j <= radius; ++j) {

                    float value = ui->tableGauss->item(i + radius, j + radius)->text().toFloat();
                    sum_r += value * qRed(ptr_original[x + j]);
                    sum_g += value * qGreen(ptr_original[x + j]);
                    sum_b += value * qBlue(ptr_original[x + j]);
                }
            }

            sum_r /= mask_sum;
            sum_g /= mask_sum;
            sum_b /= mask_sum;

            ptr_copy[x] = qRgb(sum_r, sum_g, sum_b);
        }
    }

    emit sendImage(output);
}





















