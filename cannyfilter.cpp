#include "cannyfilter.h"

CannyFilter::CannyFilter(QWidget *parent) : QWidget(parent)
{
    gaussUnsharpFilter = new GaussUnsharpFilter;
    connect(gaussUnsharpFilter, SIGNAL(sendImage(QImage)), this, SLOT(overloadImage(QImage)));

//    mSobel = {1,  2,  0, -2,  -1,
//              4,  8,  0, -8,  -4,
//              6,  12, 0, -12, -6,
//              4,  8,  0, -8,  -4,
//              1,  2,  0, -2,  -1};

    mSobel = {-1, -2, -1,
               0,  0,  0,
               1,  2,  1};
}

void CannyFilter::setImage(QImage value)
{
    image = value;
    gaussUnsharpFilter->setImage(value);
}

void CannyFilter::processImage()
{
    int radius = 1, sum_r, sum_g, sum_b, mask_index;
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);

    /* Preprocessing - Gauss */
    gaussUnsharpFilter->gaussianFilterFastCanny(2, 3);

    /* Calculating Gradients */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_output = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            sum_r = sum_g = sum_b = mask_index = 0;

            for (int i = -radius; i <= radius; ++i) {
                int index = ((y + i) > image.height()) ? y - i : abs(y + i);
                QRgb *ptr_image = (QRgb*)image.scanLine(index);

                for (int j = -radius; j <= radius; ++j) {
                    index = ((x + j) > image.width()) ? x - j : abs(x + j);

                    int red = qRed(ptr_image[index]);
                    int green = qGreen(ptr_image[index]);
                    int blue = qBlue(ptr_image[index]);

                    sum_r += (red * mSobel[mask_index]);
                    sum_g += (green * mSobel[mask_index]);
                    sum_b += (blue * mSobel[mask_index]);

                    mask_index++;
                }
            }

            sum_r = (sum_r > 255) ? 255 : ((sum_r < 0) ? 0 : sum_r);
            sum_g = (sum_g > 255) ? 255 : ((sum_g < 0) ? 0 : sum_g);
            sum_b = (sum_b > 255) ? 255 : ((sum_b < 0) ? 0 : sum_b);

            ptr_output[x] = qRgb(sum_r, sum_g, sum_b);
        }
    }

    sendImage(output);
}

void CannyFilter::overloadImage(QImage value)
{
    image = value;
}



































