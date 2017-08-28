#include "morfologicalfilter.h"
#include "ui_morfologicalfilter.h"

MorfologicalFilter::MorfologicalFilter(FacadeImage *parent): FacadeImage(parent), ui(new Ui::MorfologicalFilter)
{
    ui->setupUi(this);
    setWindowTitle("Morfological Filters");

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

QImage MorfologicalFilter::thinning(QImage &input)
{
    QImage output = QImage(input.width(), input.height(), input.format());
    QImage copy(input);

    /* Structural Element, 1 = 0, 0 = 255, -1 = doesn't matter */
    StructuralElement element[8];
    element[0] = {0, 0, 0, -1, 1, -1, 1, 1, 1}; //0
    element[1] = {-1, 0, 0, 1, 1, 0, 1, 1, -1}; //45
    element[2] = {1, -1, 0, 1, 1, 0, 1, -1, 0}; //90
    element[3] = {1, 1, -1, 1, 1, 0, -1, 0, 0}; //135
    element[4] = {1, 1, 1, -1, 1, -1, 0, 0, 0}; //180
    element[5] = {-1, 1, 1, 0, 1, 1, 0, 0, -1}; //225
    element[6] = {0, -1, 1, 0, 1, 1, 0, -1, 1}; //270
    element[7] = {0, 0, -1, 0, 1, 1, -1, 1, 1}; //315

    /* Foreach element */
    for (int angle = 0; angle < 8; ++angle) {
        output.fill(0);

        for (int y = 0; y < input.height(); ++y) {

            quint8 *ptr_output = output.scanLine(y);

            for (int x = 0; x < input.height(); ++x) {

                int sum = 0;
                int index = 0;

                /* Mask */
                for (int i = ystart; i <= yend; ++i) {

                    int yindex = (y + i >= input.height() - 1) ? (y - i) : abs(y + i);
                    quint8 *ptr_input = (quint8*)copy.scanLine(yindex);

                    for (int j = xstart; j <= xend; ++j) {
                        int xindex = (x + j >= input.width() - 1) ? (x - j) : abs(x + j);

                        if ((element[angle].data[index] == 0 && ptr_input[xindex] == 255) ||
                                (element[angle].data[index] == 1 && ptr_input[xindex] == 0)) {
                            sum++;
                        }
                        index++;
                    }
                }

                if (sum == 7) ptr_output[x] = 255;
            }
        }

        /* */
        quint8 *ptr_input = (quint8*)input.bits();
        quint8 *ptr_output = (quint8*)output.bits();
        for (int x = 0; x < input.width() * input.height(); ++x) {
            ptr_input[x] += ptr_output[x];
        }
        copy = input;
    }

    return input;
}

void MorfologicalFilter::skeletonization()
{
    /* Binarization */
    binarization();
    iBinary = output.convertToFormat(QImage::Format_Grayscale8);

    /* Skeletonization */
    QImage after, before;
    after = thinning(iBinary);

    while (before != after) {
        before = iBinary;
        after = thinning(iBinary);
    }

    /* Send Output */
    sendImage(after);
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

    skeletonization();
}









