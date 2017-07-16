#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gaussKernel = new GaussKernel;
    staticFilter = new StaticFilter;

    connect(gaussKernel, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
    connect(staticFilter, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));

    QImage image("/home/asus/Obrazy/rachel.png");
    //QImage output = QImage(image.width(), image.height(), QImage::Format_RGB32);
    ui->labelImage->setPixmap(QPixmap::fromImage(image));

    gaussKernel->setImage(image);
    staticFilter->setImage(image);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionGauss_triggered()
{
    gaussKernel->show();
}

void MainWindow::setImage(QImage image)
{
    qDebug() << "MainWindow::setImage(QImage image)";
    ui->labelImage->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::on_actionStatic_Min_Max_Median_triggered()
{
    staticFilter->show();
}

















