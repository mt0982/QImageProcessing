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
    /* Detect Edges */
    canny->setImage(image);
    canny->processImage(1, 55, 100);
    sendImage(image);

    /* Accumulator */
    int rows = floor(sqrt(pow(image.width(), 2) + pow(image.height(), 2)));     //rows = max(diagonal)
    accumulator = new float*[rows];
    for (int i = 0; i < rows; ++i) {
        accumulator[i] = new float[180];                                        //180 = angle possibilities
    }

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























