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

    QVector<QPoint> lines;
    for (int y = 0; y < image.height(); ++y) {

        quint8 *ptr_image = image.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            if (ptr_image[x] != 0) {
                for (int m = 0; m <= 360; m++) {
                    int r = (int)(x * cos((m * M_PI) / 180) + y * sin((m * M_PI) / 180));
                    if (r + height > 0) {
                        quint8 *ptr_iaccumulator = iAccumulator.scanLine(r + height);
                        ptr_iaccumulator[m] += (ptr_iaccumulator[m] + 1 < 255) ? 1 : 0;

                        if (ptr_iaccumulator[m] > 220 && !lines.contains(QPoint(r, m)))
                            lines.push_back(QPoint(r, m));
                    }
                }
            }
        }
    }

    iAccumulator = iAccumulator.scaled(image.width(), image.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    sendImage(iAccumulator);

    /* */
    QPainter iPainter(&iOutput);
    QBrush iBrush(Qt::red);
    iPainter.setBrush(iBrush);

    for (int i = 0; i < lines.size(); ++i) {
        float rho = lines[i].rx(), theta = lines[i].ry();
        QPoint pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1 = QPoint((x0 + 1000*(-b)), (y0 + 1000*(a)));
        pt2 = QPoint((x0 - 1000*(-b)), (y0 - 1000*(a)));
        iPainter.drawLine(pt1, pt2);
    }

    sendImage(iOutput);
    sendImage(image);
}























