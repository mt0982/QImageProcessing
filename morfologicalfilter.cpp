#include "morfologicalfilter.h"
#include "ui_morfologicalfilter.h"

MorfologicalFilter::MorfologicalFilter(QWidget *parent): QWidget(parent), ui(new Ui::MorfologicalFilter)
{
    ui->setupUi(this);
}

MorfologicalFilter::~MorfologicalFilter()
{
    delete ui;
}

void MorfologicalFilter::setImage(const QImage &value)
{
    image = value;
}

void MorfologicalFilter::on_pbContourExtraction_clicked()
{
    thresholding();
}

void MorfologicalFilter::thresholding()
{
    int threshold = 150;
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_input = (QRgb*)image.scanLine(y);
        QRgb *ptr_output = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            int intensity = qGray(ptr_input[x]);
            ptr_output[x] = (intensity > threshold) ? qRgb(255,255,255) : qRgb(0,0,0);
        }
    }

    /* Send Output */
    sendImage(output);
}




























