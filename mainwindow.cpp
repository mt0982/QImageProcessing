#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gaussKernel = new GaussKernel;

    //connect(gaussKernel, SIGNAL(signalCalculated(QString)), this, SLOT(close()));

    QImage image("/home/asus/Obrazy/lena.png");
    QImage output = QImage(image.width(), image.height(), QImage::Format_RGB32);
    ui->labelImage->setPixmap(QPixmap::fromImage(image));

    gaussKernel->setImage(image);

    ui->labelImage->setPixmap(QPixmap::fromImage(output));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionGauss_triggered()
{
    gaussKernel->show();
}

















