#include "canny.h"
#include "ui_canny.h"

Canny::Canny(FacadeImage *parent) : FacadeImage(parent), ui(new Ui::Canny)
{
    ui->setupUi(this);

    gaussUnsharpFilter = new GaussUnsharpFilter;
    connect(gaussUnsharpFilter, SIGNAL(sendImage(QImage)), this, SLOT(overloadImage(QImage)));

    xSobel = {1,  2,  1,
              0,  0,  0,
             -1,- 2, -1};

    ySobel = {-1,  0,  1,
              -2,  0,  2,
              -1,  0,  1};
}

Canny::~Canny()
{
    delete ui;
}

void Canny::setImage(const QImage &value)
{
    image = value;
    gaussUnsharpFilter->setImage(value);
}

void Canny::processImage()
{
    int radius = 1;

    /* Gradient & Direction */
    image = image.convertToFormat(QImage::Format_Grayscale8);
    QImage iGradient = QImage(image.width(), image.height(), image.format());
    QImage iDirection = QImage(image.width(), image.height(), image.format());

    for (int y = 0; y < image.height(); y++) {
        quint8 *ptr_igradient = (quint8*)iGradient.scanLine(y);

        for (int x = 0; x < image.width(); x++) {

            int gradX = 0;
            int gradY = 0;
            int index = 0;

            for (int i = -radius; i <= radius; ++i) {
                int yindex = ((y + i) >= image.height()) ? image.height() - i : ((y + i) < 0 ? abs(y + i) : y + i);
                quint8 *ptr_image = (quint8*)image.scanLine(yindex);

                for (int j = -radius; j <= radius; ++j) {
                    int xindex = ((x + j) >= image.height()) ? image.width() - j : ((x + j) < 0 ? abs(x + j) : x + j);

                    gradX += qGray(ptr_image[xindex]) * xSobel[index];
                    gradY += qGray(ptr_image[xindex]) * ySobel[index];
                    index++;
                }
            }

            ptr_igradient[x] = qAbs(gradX) + qAbs(gradY);
        }
    }

    sendImage(iGradient);
}

void Canny::overloadImage(QImage value)
{
    image = value;
}

void Canny::on_pbCalculate_clicked()
{
//    radius_gauss = ui->sbRadius->value();
//    tmin = ui->sbTmin->value();
//    tmax = ui->sbTmax->value();
    processImage();
}


















