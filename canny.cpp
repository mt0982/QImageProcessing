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
        quint8 *ptr_idirection = (quint8*)iDirection.scanLine(y);

        for (int x = 0; x < image.width(); x++) {

            float gradX = 0;
            float gradY = 0;
            float index = 0;

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

            /* Direction */
            if (gradX == 0 && gradY == 0) ptr_idirection[x] = 0;
            else if (gradX == 0) ptr_idirection[x] = 135;
            else {
                float angle = 180.0f * atan(gradY / gradX) / M_PI;

                if (angle >= -22.5 && angle < 22.5) ptr_idirection[x] = 0;
                else if (angle >= 22.5 && angle < 67.5) ptr_idirection[x] = 45;
                else if (angle >= -67.5 && angle < -22.5) ptr_idirection[x] = 90;
                else ptr_idirection[x] = 135;
            }
        }
    }

    /* Nonmaximum supression */
    QImage iNonmaximum = QImage(image.width(), image.height(), image.format());
    for (int y = 0; y < image.height(); y++) {
        int yup = (y < 0) ? y : y - 1;
        int ydown = (y >= image.height()) ? y : y + 1;

        quint8 *ptr_igradient = (quint8*)iGradient.scanLine(y);
        quint8 *ptr_igradient_up = (quint8*)iGradient.scanLine(yup);
        quint8 *ptr_igradient_down = (quint8*)iGradient.scanLine(ydown);
        quint8 *ptr_idirection = (quint8*)iDirection.scanLine(y);
        quint8 *ptr_inonmaximum = (quint8*)iNonmaximum.scanLine(y);

        for (int x = 0; x < image.width(); x++) {
            int xleft = (x - 1 < 0) ? 0 : x;
            int xright = (x + 1 >= image.width() - 1) ? x : x + 1;

            if (ptr_idirection[x] == 0) {
                if (ptr_igradient[x] < ptr_igradient[xleft] || ptr_igradient[x] < ptr_igradient[xright])
                    ptr_inonmaximum[x] = 0;
                else ptr_inonmaximum[x] = ptr_igradient[x];
            }
            else if (ptr_idirection[x] == 45) {
                if (ptr_igradient[x] < ptr_igradient_up[xright] || ptr_igradient[x] < ptr_igradient_down[xleft])
                    ptr_inonmaximum[x] = 0;
                else ptr_inonmaximum[x] = ptr_igradient[x];
            }
            else if (ptr_idirection[x] == 135) {
                if (ptr_igradient[x] < ptr_igradient_up[xleft] || ptr_igradient[x] < ptr_igradient_down[xright])
                    ptr_inonmaximum[x] = 0;
                else ptr_inonmaximum[x] = ptr_igradient[x];
            }
            else {
                if (ptr_igradient[x] < ptr_igradient_up[x] || ptr_igradient[x] < ptr_igradient_down[x])
                    ptr_inonmaximum[x] = 0;
                else ptr_inonmaximum[x] = ptr_igradient[x];
            }
        }
    }

    sendImage(iNonmaximum);
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


















