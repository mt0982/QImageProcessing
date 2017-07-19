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

    xSobel = {-1, -2, -1,
              0,  0,  0,
              1,  2,  1};

    ySobel = {-1,  0,  1,
              -2,  0,  2,
              -1,  0,  1};
}

void CannyFilter::setImage(QImage value)
{
    image = value;
    gaussUnsharpFilter->setImage(value);
}

void CannyFilter::processImage()
{
    int radius = 1, sum_rv, sum_gv, sum_bv, sum_rh, sum_gh, sum_bh, mask_index;
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_horizontal = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_vertical = QImage(image.width(), image.height(), QImage::Format_RGB32);

    /* Preprocessing - Gauss */
    gaussUnsharpFilter->gaussianFilterFastCanny(2, 1);

    /* Calculating Gradients */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_output = (QRgb*)output.scanLine(y);
        QRgb *ptr_horizontal = (QRgb*)output_horizontal.scanLine(y);
        QRgb *ptr_vertical = (QRgb*)output_vertical.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            sum_rv = sum_gv = sum_bv = sum_rh = sum_bh = sum_gh = mask_index = 0;

            for (int i = -radius; i <= radius; ++i) {
                int yindex = ((y + i) > image.height()) ? y - i : abs(y + i);
                QRgb *ptr_image = (QRgb*)image.scanLine(yindex);

                for (int j = -radius; j <= radius; ++j) {
                    int xindex = ((x + j) > image.width()) ? x - j : abs(x + j);

                    int red = qRed(ptr_image[xindex]);
                    int green = qGreen(ptr_image[xindex]);
                    int blue = qBlue(ptr_image[xindex]);

                    /* Horizontal - fx */
                    sum_rh += (red * xSobel[mask_index]);
                    sum_gh += (green * xSobel[mask_index]);
                    sum_bh += (blue * xSobel[mask_index]);

                    /* Vertical - fy */
                    sum_rv += (red * ySobel[mask_index]);
                    sum_gv += (green * ySobel[mask_index]);
                    sum_bv += (blue * ySobel[mask_index]);

                    mask_index++;

                }
            }

            int magnitude = sqrt(pow(x, 2) + (pow(y, 2)));
            float direction = (x != 0 && y != 0) ? atan(y / x) : 0;

            sum_rv = (sum_rv > 255) ? 255 : ((sum_rv < 0) ? 0 : sum_rv);
            sum_gv = (sum_gv > 255) ? 255 : ((sum_gv < 0) ? 0 : sum_gv);
            sum_bv = (sum_bv > 255) ? 255 : ((sum_bv < 0) ? 0 : sum_bv);

            sum_rh = (sum_rh > 255) ? 255 : ((sum_rh < 0) ? 0 : sum_rh);
            sum_gh = (sum_gh > 255) ? 255 : ((sum_gh < 0) ? 0 : sum_gh);
            sum_bh = (sum_bh > 255) ? 255 : ((sum_bh < 0) ? 0 : sum_bh);

            ptr_vertical[x] = qRgb(sum_rv, sum_gv, sum_bv);
            ptr_horizontal[x] = qRgb(sum_rh, sum_gh, sum_bh);
            ptr_output[x] = qRgb(sum_rv, sum_gv, sum_bv);
        }
    }

    QLabel *windowA = new QLabel;
    windowA->setPixmap(QPixmap::fromImage(output_horizontal));
    windowA->show();

    QLabel *windowB = new QLabel;
    windowB->setPixmap(QPixmap::fromImage(output_vertical));
    windowB->show();

    sendImage(/*output*/image);
}

void CannyFilter::overloadImage(QImage value)
{
    image = value;
}



































