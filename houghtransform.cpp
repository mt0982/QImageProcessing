#include "houghtransform.h"
#include "ui_houghtransform.h"

HoughTransform::HoughTransform(FacadeImage *parent) : FacadeImage(parent), ui(new Ui::HoughTransform)
{
    ui->setupUi(this);

    canny = new Canny;
    connect(canny, &Canny::sendImage, [this](const QImage &value) {
        image = value;
    });
}

HoughTransform::~HoughTransform()
{
    delete ui;
}

void HoughTransform::on_pbCalculate_clicked()
{
    /* Detect Edges - Canny */
    QImage iOutput(image);
    canny->setImage(image);
    canny->processImage(1, 55, 100);

    /* Accumulato */
    int height = floor(sqrt(pow(image.width(), 2) + pow(image.height(), 2)));       //rows = max(diagonal)
    QImage iAccumulator(180, 2 * height, QImage::Format_Grayscale8);                //-R < height < R, R = diagonal
    iAccumulator.fill(0);

    for (int y = 0; y < image.height(); ++y) {

        quint8 *ptr_image = image.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            if (ptr_image[x] != 0) {
                for (int m = 0; m < 180 * 3; m++) {
                    int r = (int)(x * cos((m * M_PI) / 180) + y * sin((m * M_PI) / 180));
                    if (r + height > 0) {
                        quint8 *ptr_iaccumulator = iAccumulator.scanLine(r + height);
                        ptr_iaccumulator[m] += (ptr_iaccumulator[m] + 1 < 255) ? 1 : 0;
                    }
                }
            }
        }
    }

    iAccumulator = iAccumulator.scaled(image.width(), image.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    sendImage(iAccumulator);

//    /* */
//    for (int y = 0; y < image.height(); ++y) {

//        quint8 *ptr_iaccumulator = iAccumulator.scanLine(y);
//        quint8 *ptr_ioutput = iOutput.scanLine(y);

//        for (int x = 0; x < image.width(); ++x) {

//            if (ptr_iaccumulator[x] > 240)
//                ptr_ioutput[x] = qRgb(255,255,255);
//        }
//    }

//    sendImage(iOutput);

//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < 180; ++j) {
//            accumulator[i][j] =
//        }
//    }

//    /* */
//    for (int y = 0; y < image.height(); ++y) {

//        for (int x = 0; x < image.width(); ++x) {

//            double angle = atan2(y, x) * double(180/M_PI);          //0 < angle < PI
//            double distance = x * cos(angle) + y * sin(angle);      //-R < p < R, R = diagonal
//            //qDebug() << angle;


//        }
//    }
}























