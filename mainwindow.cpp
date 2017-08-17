#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), outputWindow(NULL)
{
    ui->setupUi(this);
    gaussUnsharpFilter = new GaussUnsharpFilter;
    staticFilter = new StaticFilter;
    cannyFilter = new Canny;
    morfologicalFilter = new MorfologicalFilter;
    houghTransform = new HoughTransform;
    fftwConvolution = new FFTW;

    connect(gaussUnsharpFilter, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
    connect(staticFilter, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
    connect(cannyFilter, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
    connect(morfologicalFilter, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
    connect(houghTransform, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
    connect(fftwConvolution, SIGNAL(sendImage(QImage)), this, SLOT(setImage(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setImage(const QImage &image)
{
    qDebug() << "MainWindow::setImage(QImage image)";
    //ui->labelImage->setPixmap(QPixmap::fromImage(image));

    /* It's necessay, fix some color problems */
    if (!outputWindow) outputWindow = new QLabel;
    outputWindow->setPixmap(QPixmap::fromImage(image));
    outputWindow->show();
}

void MainWindow::on_actionGauss_triggered()
{
    gaussUnsharpFilter->show();
}

void MainWindow::on_actionStatic_Min_Max_Median_triggered()
{
    staticFilter->show();
}

void MainWindow::on_actionCanny_triggered()
{
    cannyFilter->show();
}

void MainWindow::on_actionMorfological_triggered()
{
    morfologicalFilter->show();
}

void MainWindow::on_actionHough_triggered()
{
    houghTransform->show();
}

void MainWindow::on_actionFFTW_Convolution_triggered()
{
    fftwConvolution->show();
}

void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                    "/home/asus/Obrazy", tr("Image Files (*.png *.jpg *.bmp)"));

    QImage image(filePath);
    ui->labelImage->setPixmap(QPixmap::fromImage(image));

    gaussUnsharpFilter->setImage(image);
    staticFilter->setImage(image);
    cannyFilter->setImage(image);
    morfologicalFilter->setImage(image);
    houghTransform->setImage(image);
    fftwConvolution->setImage(image);
}
