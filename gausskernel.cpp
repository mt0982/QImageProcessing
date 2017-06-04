#include "gausskernel.h"
#include "ui_gausskernel.h"

GaussKernel::GaussKernel(QWidget *parent): QWidget(parent), ui(new Ui::GaussKernel)
{
    ui->setupUi(this);
    setWindowTitle("Kernel");

    /* Initialize, Minimum */
    ui->tableGauss->setRowCount(3);
    ui->tableGauss->setColumnCount(3);
    ui->tableGauss->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
            int value_int = value * 100;
            item = new QTableWidgetItem;
            ui->tableGauss->setItem(x,y,item);
            item->setText(QString::number(value_int));
        }
    }
}

void GaussKernel::on_pbCalculate_clicked()
{
    qDebug() << "GaussKernel::on_pbCalculate_clicked()";
    //gaussianFilter();
    gaussianFilterFast();
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
            //float value = ui->tableGauss->item(i, j)->text().toFloat();
            int value = ui->tableGauss->item(i, j)->text().toInt();
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

                    //float value = ui->tableGauss->item(i + radius, j + radius)->text().toFloat();
                    int value = ui->tableGauss->item(i + radius, j + radius)->text().toInt();
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

void GaussKernel::gaussianFilterFast()
{
    /* Parameters */
    int radius = ui->sbRadius->value();
    int sum_r, sum_b, sum_g;
    QImage output(image.width(), image.height(), QImage::Format_RGB32);

    /* 1D */
    int total = radius * 2 + 1;
    /*float*/ int array[total];

    /* Init 1D Aray */
    mask_sum = 0;
    for (int i = 0; i < total; ++i) {
        //array[i] = ui->tableGauss->item(total / 2, i)->text().toFloat();
        array[i] = ui->tableGauss->item(total / 2, i)->text().toInt();
        mask_sum += array[i];
    }

    /* Horizontal */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_original = (QRgb*)image.scanLine(y);
        QRgb *ptr_copy = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {
            sum_r = sum_g = sum_b = 0;

            for (int i = 0; i < total; ++i) {

                sum_r += qRed(ptr_original[abs(x+i-radius)]) * array[i];
                sum_g += qGreen(ptr_original[abs(x+i-radius)]) * array[i];
                sum_b += qBlue(ptr_original[abs(x+i-radius)]) * array[i];
            }

            sum_r /= mask_sum;
            sum_g /= mask_sum;
            sum_b /= mask_sum;

            sum_r = (sum_r > 255) ? 255 : ((sum_r < 0) ? 0 : sum_r);
            sum_g = (sum_g > 255) ? 255 : ((sum_g < 0) ? 0 : sum_g);
            sum_b = (sum_b > 255) ? 255 : ((sum_b < 0) ? 0 : sum_b);

            ptr_copy[x] = qRgb(sum_r, sum_g, sum_b);
        }
    }

    QTransform transform;
    transform.rotate(90);
    output = output.transformed(transform);
    QImage second_output = output;

    /* Vertical, Second Pass */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_original = (QRgb*)second_output.scanLine(y);
        QRgb *ptr_copy = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {
            sum_r = sum_g = sum_b = 0;

            for (int i = 0; i < total; ++i) {

                sum_r += qRed(ptr_original[abs(x+i-radius)]) * array[i];
                sum_g += qGreen(ptr_original[abs(x+i-radius)]) * array[i];
                sum_b += qBlue(ptr_original[abs(x+i-radius)]) * array[i];
            }

            sum_r /= mask_sum;
            sum_g /= mask_sum;
            sum_b /= mask_sum;

            sum_r = (sum_r > 255) ? 255 : ((sum_r < 0) ? 0 : sum_r);
            sum_g = (sum_g > 255) ? 255 : ((sum_g < 0) ? 0 : sum_g);
            sum_b = (sum_b > 255) ? 255 : ((sum_b < 0) ? 0 : sum_b);

            ptr_copy[x] = qRgb(sum_r, sum_g, sum_b);
        }
    }

    transform.rotate(180);
    output = output.transformed(transform);
    gaussImage = output;

    /* Emit Output */
    emit sendImage(output);
}

void GaussKernel::unsharpFilter()
{
    gaussianFilterFast();

    /* Calculate Mask */
    QImage mask(image.width(), image.height(), QImage::Format_RGB32);
    QImage output(image.width(), image.height(), QImage::Format_RGB32);
    int alfa = ui->labelAlfa->text().toInt();
    qDebug() << alfa;

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_mask = (QRgb*)mask.scanLine(y);
        QRgb *ptr_gauss = (QRgb*)gaussImage.scanLine(y);
        QRgb *ptr_original = (QRgb*)image.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            int r = qRed(ptr_original[x]) - qRed(ptr_gauss[x]);
            int g = qGreen(ptr_original[x]) - qGreen(ptr_gauss[x]);
            int b = qBlue(ptr_original[x]) - qBlue(ptr_gauss[x]);

            r = (r < 0) ? 0 : r;
            g = (g < 0) ? 0 : g;
            b = (b < 0) ? 0 : b;

            ptr_mask[x] = qRgb(r, g, b);
        }
    }

    /* Unsharp = Original + α * mask */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_mask = (QRgb*)mask.scanLine(y);
        QRgb *ptr_output = (QRgb*)output.scanLine(y);
        QRgb *ptr_original = (QRgb*)image.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            int r = qRed(ptr_original[x]) + alfa * qRed(ptr_mask[x]);
            int g = qGreen(ptr_original[x]) + alfa * qGreen(ptr_mask[x]);
            int b = qBlue(ptr_original[x]) + alfa * qBlue(ptr_mask[x]);

            r = (r > 255) ? 255 : r;
            g = (g > 255) ? 255 : g;
            b = (b > 255) ? 255 : b;

            ptr_output[x] = qRgb(r, g, b);
        }
    }

    /* Emit Output */
    emit sendImage(output);
}

void GaussKernel::on_pbUnsharp_clicked()
{
    unsharpFilter();
}






















