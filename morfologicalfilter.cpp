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
    /* Structural Element Offsets */
    int wallLength = (ui->sbRadius->value() * 2 + 1) - 1;
    ystart = -ui->sbY->value();
    yend = wallLength - ui->sbY->value();
    xstart = -ui->sbX->value();
    xend = wallLength - ui->sbX->value();

    /* Binarization */
    erosion();
}

void MorfologicalFilter::binarization()
{
    int threshold = 150;
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);       //Binarization

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_input = (QRgb*)image.scanLine(y);
        QRgb *ptr_output = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            int intensity = qGray(ptr_input[x]);
            ptr_output[x] = (intensity > threshold) ? qRgb(255,255,255) : qRgb(0,0,0);
        }
    }

    /* Send Output */
    //sendImage(output);
}

void MorfologicalFilter::erosion()
{
    /* Binarization */
    binarization();

    /* Erosion */
    QImage imageErosion(image.width(), image.height(), QImage::Format_RGB32);

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_output = (QRgb*)imageErosion.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            bool isNull = false;

            for (int i = ystart; i <= yend; ++i) {
                int index = ((y+i) >= image.height()) ? y-i : abs(y+i);
                QRgb *ptr_bin = (QRgb*)output.scanLine(index);

                for (int j = xstart; j <= xend; ++j) {
                    index = ((x+j) >= image.width()) ? x-j : abs(x+j);
                    if ((array[i + abs(ystart)][j + abs(xstart)] == 1) && (qGray(ptr_bin[index]) == 0)) isNull = true;
                }
            }

            ptr_output[x] = (!isNull) ? qRgb(255, 255, 255) : qRgb(0,0,0);
        }
    }

    /* Send Output */
    //sendImage(imageErosion);
    boundaryExtraction(imageErosion);
}

void MorfologicalFilter::boundaryExtraction(const QImage &imageErosion)
{
    QImage extraction = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QRgb *ptr_extraction = (QRgb*)extraction.bits();
    QRgb *ptr_erosion = (QRgb*)imageErosion.bits();
    QRgb *ptr_binary = (QRgb*)output.bits();

    for (int i = 0; i < image.width() * image.height(); ++i) {
        ptr_extraction[i] = ptr_binary[i] - ptr_erosion[i];
    }

    /* Send Output */
    sendImage(extraction);
}

void MorfologicalFilter::thinning()
{
    /* Binarization */
    binarization();

    /* Structural Element Sum */
    int structural_sum = 0;
    for (int i = 0; i < ui->sbRadius->value() * 2 + 1; ++i) {
        for (int j = 0; j < ui->sbRadius->value() * 2 + 1; ++j) {
            structural_sum += array[i][j];
        }
    }

    /* Angle Arrays */
    StructuralElement structuralElement[8];
    structuralElement[0] = {0, 0, 0, 0, 1, 0, 1, 1, 1}; //0
    structuralElement[1] = {0, 0, 0, 1, 1, 0, 1, 1, 0}; //45
    structuralElement[2] = {1, 0, 0, 1, 1, 0, 1, 0, 0}; //90
    structuralElement[3] = {1, 1, 0, 1, 1, 0, 0, 0, 0}; //135
    structuralElement[4] = {1, 1, 1, 0, 1, 0, 0, 0, 0}; //180
    structuralElement[5] = {0, 1, 1, 0, 1, 1, 0, 0, 0}; //225
    structuralElement[6] = {0, 0, 1, 0, 1, 1, 0, 0, 1}; //270
    structuralElement[7] = {0, 0, 0, 0, 1, 1, 0, 1, 1}; //315

    /* Thinning */
    QImage imageHitOrMiss(image.width(), image.height(), QImage::Format_RGB32);
    QImage imageSkeletonization = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage replacedOutput = output;

    /* While Is Different */
    //for (int pp = 0; pp < 5; ++pp) {
        //qDebug() << pp;

        /* Foreach Angle */
        for (int kk = 0; kk < 8; ++kk) {

            for (int y = 0; y < image.height(); ++y) {
                QRgb *ptr_output = (QRgb*)imageHitOrMiss.scanLine(y);

                for (int x = 0; x < image.width(); ++x) {

                    int sum = 0;
                    int structuralIndex = 0;

                    for (int i = ystart; i <= yend; ++i) {
                        int index = ((y+i) >= image.height()) ? y-i : abs(y+i);
                        QRgb *ptr_bin = (QRgb*)replacedOutput.scanLine(index);

                        for (int j = xstart; j <= xend; ++j) {
                            index = ((x+j) >= image.width()) ? x-j : abs(x+j);
                            if ((qGray(ptr_bin[index]) == 0) && structuralElement[kk].data[structuralIndex]) {
                                sum++;
                            }
                            structuralIndex++;
                        }
                    }

                    ptr_output[x] = (sum == 4) ? qRgb(255, 255, 255) : qRgb(0,0,0);
                }
            }
        }

        /* */
        QRgb *ptr_skeletonization = (QRgb*)imageSkeletonization.bits();
        QRgb *ptr_hitOrMis = (QRgb*)imageHitOrMiss.bits();
        QRgb *ptr_binary = (QRgb*)replacedOutput.bits();

        for (int i = 0; i < image.width() * image.height(); ++i) {
            ptr_skeletonization[i] = ptr_binary[i] - ptr_hitOrMis[i];
        }

        //replacedOutput = imageSkeletonization;
    //}

    /* Send Output */
    sendImage(imageSkeletonization);
    //skeletonization(imageHitOrMiss);
}

void MorfologicalFilter::skeletonization(const QImage &imageHitOrMiss)
{
//    QImage imageSkeletonization = QImage(image.width(), image.height(), QImage::Format_RGB32);
//    QRgb *ptr_skeletonization = (QRgb*)imageSkeletonization.bits();
//    QRgb *ptr_hitOrMis = (QRgb*)imageHitOrMiss.bits();
//    QRgb *ptr_binary = (QRgb*)output.bits();

//    for (int time = 0; time < 5; ++time) {
//        for (int i = 0; i < image.width() * image.height(); ++i) {
//            ptr_skeletonization[i] = ptr_binary[i] - ptr_hitOrMis[i];
//        }
//        image = imageSkeletonization;
//    }



//    /* Send Output */
//    sendImage(imageSkeletonization);
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

void MorfologicalFilter::on_pbSkeletonization_clicked()
{
    /* Structural Element Offsets */
    ystart = -1;
    yend = 1;
    xstart = -1;
    xend = 1;

    thinning();
}









