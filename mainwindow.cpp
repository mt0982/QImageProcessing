#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), outputWindow(NULL)
{
    ui->setupUi(this);
    gaussUnsharpFilter = new GaussUnsharpFilter;
    staticFilter = new StaticFilter;
    cannyFilter = new Canny;

    connect(gaussUnsharpFilter, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
    connect(staticFilter, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
    connect(cannyFilter, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));

    QImage image("/home/asus/Obrazy/lena.png");
    //QImage output = QImage(image.width(), image.height(), QImage::Format_RGB32);
    ui->labelImage->setPixmap(QPixmap::fromImage(image));

    gaussUnsharpFilter->setImage(image);
    staticFilter->setImage(image);
    cannyFilter->setImage(image);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionGauss_triggered()
{
    gaussUnsharpFilter->show();
}

void MainWindow::setImage(QImage image)
{
    qDebug() << "MainWindow::setImage(QImage image)";
    //ui->labelImage->setPixmap(QPixmap::fromImage(image));

    /* It's necessay, fix some color problems */
    outputWindow = new QLabel;
    outputWindow->setPixmap(QPixmap::fromImage(image));
    outputWindow->show();
}

void MainWindow::on_actionStatic_Min_Max_Median_triggered()
{
    staticFilter->show();
}

void MainWindow::on_actionCanny_triggered()
{
    cannyFilter->show();

}

















