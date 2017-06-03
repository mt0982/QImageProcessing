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
        ui->tableGauss->setRowHeight(i, 20);
        ui->tableGauss->setColumnWidth(i, 20);
    }

    //QTableWidgetItem *item = ui->tableGauss->item(0,0);
    //item->setText("C");
    //ui->tableGauss->setItem(0,0,item);
}

GaussKernel::~GaussKernel()
{
    delete ui;
}

void GaussKernel::on_sbRadius_valueChanged(int arg1)
{
    arg1 = arg1 * 2 + 1;

    ui->tableGauss->setRowCount(arg1);
    ui->tableGauss->setColumnCount(arg1);

    for (int i = 0; i < arg1; ++i) {
        ui->tableGauss->setRowHeight(i, 20);
        ui->tableGauss->setColumnWidth(i, 20);
    }
}

void GaussKernel::on_leSigma_textChanged(const QString &arg1)
{
    int radius = ui->sbRadius->text().toInt();
    double sigma = arg1.toInt();

    for (int x = 0; x < ui->tableGauss->rowCount(); ++x) {
        for (int y = 0; y < ui->tableGauss->colorCount(); ++y) {
            float value = exp(-(pow(x - radius,2) + pow(y - radius,2)) / (2 * pow(sigma,2)));
        }
    }
}


















