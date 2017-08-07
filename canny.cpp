#include "canny.h"
#include "ui_canny.h"

Canny::Canny(FacadeImage *parent) : FacadeImage(parent), ui(new Ui::Canny)
{
    ui->setupUi(this);

    gaussUnsharpFilter = new GaussUnsharpFilter;
    connect(gaussUnsharpFilter, SIGNAL(sendImage(QImage)), this, SLOT(overloadImage(QImage)));

    ySobel = {1,  2,  1,
              0,  0,  0,
             -1,- 2, -1};

    xSobel = {-1,  0,  1,
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

void Canny::processImage(int radius, int tmin, int tmax)
{
    this->radius_gauss = radius;
    this->tmin = tmin;
    this->tmax = tmax;
    processImage();
}

void Canny::findPath(QImage &iCanny, int x, int y)
{
    int radius = 1;

    quint8 *ptr_icanny = (quint8*)iCanny.scanLine(y);

    if (ptr_icanny[x] != 255) return;

    for (int i = -radius; i <= radius; i++) {

        int offY = y + i;

        if (offY < 0 || offY >= image.height())
            continue;

        quint8 *ptr_icanny_next = (quint8*)iCanny.scanLine(y + i);

        for (int j = -radius; j <= radius; j++) {

            int offX = x + j;

            if (i == j || offY < 0 || offY >= image.width())
                continue;

            if (ptr_icanny_next[offX] == 127) {
                ptr_icanny_next[offX] = 255;
                findPath(iCanny, offX, offY);
            }
        }
    }
}

void Canny::processImage()
{
    /* Gauss */
    gaussUnsharpFilter->gaussianFilterFastCanny(radius_gauss, 1);

    /* Gradient & Direction */
    image = image.convertToFormat(QImage::Format_Grayscale8);
    QImage iGradient = QImage(image.width(), image.height(), image.format());
    QImage iDirection = QImage(image.width(), image.height(), image.format());

    for (int y = 0; y < image.height(); y++) {
        quint8 *ptr_idirection = (quint8*)iDirection.scanLine(y);
        quint8 *ptr_igradient = (quint8*)iGradient.scanLine(y);
        quint8 *ptr_image = image.scanLine(y);

        quint8 *ptr_image_up = (y < 1) ? ptr_image : ptr_image - image.width();
        quint8 *ptr_image_down = (y >= image.height() - 1) ? ptr_image : ptr_image + image.width();

        for (int x = 0; x < image.width(); x++) {
            int xleft = (x < 1) ? x : x - 1;
            int xright = (x >= image.width() - 1) ? x : x + 1;

            /* Static Radius = 1 */
            double gradX = ptr_image_up[xright]
                    + 2 * ptr_image[xright]
                    + ptr_image_down[xright]
                    - ptr_image_up[xleft]
                    - 2 * ptr_image[xleft]
                    - ptr_image_down[xleft];

            double gradY = ptr_image_up[xleft]
                    + 2 * ptr_image_up[x]
                    + ptr_image_up[xright]
                    - ptr_image_down[xleft]
                    - 2 * ptr_image_down[x]
                    - ptr_image_down[xright];

            ptr_igradient[x] = qAbs(gradX) + qAbs(gradY);

            /* Direction */
            if (gradX == 0 && gradY == 0) ptr_idirection[x] = 0;
            else if (gradX == 0) ptr_idirection[x] = 135;
            else {
                double angle = 180.0f * atan(gradY / gradX) / M_PI;

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
        int yup = (y < 1) ? y : y - 1;
        int ydown = (y >= image.height() - 1) ? y : y + 1;

        quint8 *ptr_igradient = (quint8*)iGradient.scanLine(y);
        quint8 *ptr_igradient_up = (quint8*)iGradient.scanLine(yup);
        quint8 *ptr_igradient_down = (quint8*)iGradient.scanLine(ydown);
        quint8 *ptr_idirection = (quint8*)iDirection.scanLine(y);
        quint8 *ptr_inonmaximum = (quint8*)iNonmaximum.scanLine(y);

        for (int x = 0; x < image.width(); x++) {
            int xleft = (x < 1) ? 0 : x - 1;
            int xright = (x >= image.width() - 1) ? x : x + 1;

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

    /* Threshold */
    QImage iThreshold = QImage(image.width(), image.height(), image.format());
    for (int y = 0; y < image.height(); y++) {
        quint8 *ptr_ithreshold = (quint8*)iThreshold.scanLine(y);
        quint8 *ptr_inonmaximum = (quint8*)iNonmaximum.scanLine(y);

        for (int x = 0; x < image.width(); x++) {
            ptr_ithreshold[x] = (ptr_inonmaximum[x] < tmin) ? 0 : ((ptr_inonmaximum[x] > tmax) ? 255 : 127);
        }
    }

    /* Hysteresis */
    QImage iCanny(iThreshold);
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            findPath(iCanny, x, y);
        }
    }

    quint8 *ptr_iCanny = iCanny.bits();
    for (int i = 0; i < image.height() * image.width(); i++) {
        if (ptr_iCanny[i] == 127) ptr_iCanny[i] = 0;
    }

    /* Send Output */
    //sendImage(iGradient);
    //sendImage(iDirection);
    //sendImage(iNonmaximum);
    //sendImage(iThreshold);
    sendImage(iCanny);
}

void Canny::overloadImage(QImage value)
{
    image = value;
}

void Canny::on_pbCalculate_clicked()
{
    radius_gauss = ui->sbRadius->value();
    tmin = ui->sbTmin->value();
    tmax = ui->sbTmax->value();
    processImage();
}


















