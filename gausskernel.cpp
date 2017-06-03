#include "gausskernel.h"
#include "ui_gausskernel.h"

GaussKernel::GaussKernel(QWidget *parent): QWidget(parent), ui(new Ui::GaussKernel)
{
    ui->setupUi(this);
    setWindowTitle("Kernel");

    //ui->tableGauss->

    /* Initialize, Minimum */
    ui->tableGauss->setRowCount(3);
    ui->tableGauss->setColumnCount(3);

    for (int i = 0; i < 3; ++i) {
        ui->tableGauss->setRowHeight(i, 40);
        ui->tableGauss->setColumnWidth(i, 40);
    }

}

GaussKernel::~GaussKernel()
{
    delete ui;
}

void GaussKernel::on_sbRadius_valueChanged(int arg1)
{
    /* Clear */
    ui->leSigma->clear();
    ui->tableGauss->clear();

    arg1 = arg1 * 2 + 1;

    ui->tableGauss->setRowCount(arg1);
    ui->tableGauss->setColumnCount(arg1);

    for (int i = 0; i < arg1; ++i) {
        ui->tableGauss->setRowHeight(i, 40);
        ui->tableGauss->setColumnWidth(i, 40);
    }
}

void GaussKernel::on_leSigma_textChanged(const QString &arg1)
{
    /* Clear */
    ui->tableGauss->clear();

    /* Parameters */
    int radius = ui->sbRadius->text().toInt();
    double sigma = arg1.toInt();

    /* Init cell */
    QTableWidgetItem *item;

    for (int x = 0; x < ui->tableGauss->rowCount(); ++x) {
        for (int y = 0; y < ui->tableGauss->colorCount(); ++y) {

            float value = exp(-(pow(x - radius,2) + pow(y - radius,2)) / (2 * pow(sigma,2)));
            item = new QTableWidgetItem;
            ui->tableGauss->setItem(x,y,item);
            item->setText(QString::number(value));
        }
    }
}


















