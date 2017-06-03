#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gausskernel.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GaussKernel *gaussKernel = new GaussKernel;
    gaussKernel->show();

    QImage image("/home/asus/Obrazy/lena.png");
    QImage output = QImage(image.width(), image.height(), QImage::Format_RGB32);
    ui->labelImage->setPixmap(QPixmap::fromImage(image));

    int radius = 1;
    double sigma = 10;
    int size = pow((pow(radius,2)+1), 2);
    double kernel[size];
    qDebug() << size;

    /* Gauss Kernel */
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            qDebug() << exp(-(pow(i,2) + pow(j,2)) / (2 * pow(sigma,2)));
        }
    }

    for (int y = 0; y < image.height(); y++) {

//        QRgb *prt_output = (QRgb*)output.scanLine(y);

//        for (int x = 0; x < image.width(); x++) {

//            for (int i = -radius; i <= radius; i++) {
//                for (int j = -radius; j <= radius; j++) {

//                    QRgb *prt_original = (QRgb*)image.scanLine(abs(y + j));
//                }
//            }

//        }
    }

    ui->labelImage->setPixmap(QPixmap::fromImage(output));
}

MainWindow::~MainWindow()
{
    delete ui;
}
