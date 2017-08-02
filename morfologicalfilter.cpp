#include "morfologicalfilter.h"
#include "ui_morfologicalfilter.h"

MorfologicalFilter::MorfologicalFilter(FacadeImage *parent): FacadeImage(parent), ui(new Ui::MorfologicalFilter)
{
    ui->setupUi(this);

    array = new int*[ui->sbRadius->value() * 2 + 1];
    for (int i = 0; i < ui->sbRadius->value() * 2 + 1; ++i) {
        array[i] = new int[ui->sbRadius->value() * 2 + 1];
        for (int j = 0; j < ui->sbRadius->value() * 2 + 1; ++j) {
            array[i][j] = 0;
        }
    }

    ui->sbY->setMaximum(ui->tableWidget->rowCount() - 1);
    ui->sbX->setMaximum(ui->tableWidget->columnCount() - 1);
}

MorfologicalFilter::~MorfologicalFilter()
{
    delete ui;
}

void MorfologicalFilter::on_pbContourExtraction_clicked()
{
    thresholding();

    int wallLength = (ui->sbRadius->value() * 2 + 1) - 1;
    int ystart = -ui->sbY->value();
    int yend = wallLength - ui->sbY->value();
    int xstart = -ui->sbX->value();
    int xend = wallLength - ui->sbX->value();

    qDebug() << ystart << yend << xstart << xend;
}

void MorfologicalFilter::thresholding()
{
    int threshold = 150;
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_input = (QRgb*)image.scanLine(y);
        QRgb *ptr_output = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            int intensity = qGray(ptr_input[x]);
            ptr_output[x] = (intensity > threshold) ? qRgb(255,255,255) : qRgb(0,0,0);
        }
    }

    /* Send Output */
    sendImage(output);
}

void MorfologicalFilter::on_sbRadius_valueChanged(int arg1)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(arg1 * 2 + 1);
    ui->tableWidget->setColumnCount(arg1 * 2 + 1);

    array = new int*[ui->sbRadius->value() * 2 + 1];
    for (int i = 0; i < ui->sbRadius->value() * 2 + 1; ++i) {
        array[i] = new int[ui->sbRadius->value() * 2 + 1];
        for (int j = 0; j < ui->sbRadius->value() * 2 + 1; ++j) {
            array[i][j] = 0;
        }
    }

    ui->sbY->setMaximum(ui->tableWidget->rowCount() - 1);
    ui->sbX->setMaximum(ui->tableWidget->columnCount() - 1);
}

void MorfologicalFilter::on_tableWidget_cellClicked(int row, int column)
{
    QModelIndex modelIndex = ui->tableWidget->model()->index(row, column);

    if (array[row][column] == 1){
        array[row][column] = 0;
        ui->tableWidget->model()->setData(modelIndex, QVariant(QBrush(Qt::white)), Qt::BackgroundRole);
    }
    else {
        array[row][column] = 1;
        ui->tableWidget->model()->setData(modelIndex, QVariant(QBrush(Qt::blue)), Qt::BackgroundRole);
    }

//    qDebug() << array[0][0] << array[0][1] << array[0][2];
//    qDebug() << array[1][0] << array[1][1] << array[1][2];
//    qDebug() << array[2][0] << array[2][1] << array[2][2];
//    qDebug() << "";
}













