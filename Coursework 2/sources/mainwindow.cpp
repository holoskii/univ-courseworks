#include "headers/mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->computeButton->setEnabled(false);
    ui->disconnectButton->setEnabled(false);

    // create graph
    ui->myPlot->addGraph();
    ui->myPlot->xAxis->setLabel("x");
    ui->myPlot->yAxis->setLabel("y");
}

MainWindow::~MainWindow(){
    p.stop();
    delete ui;
}


void MainWindow::on_connectButton_clicked(){
    if (p.connect() != 0) {
        writeDebug("Failed to connected");
        return;
    }

    writeDebug("Connected");
    ui->connectButton->setEnabled(false);
    ui->disconnectButton->setEnabled(true);
    ui->computeButton->setEnabled(true);
}

void MainWindow::on_exitButton_clicked(){
    this->~MainWindow();
}

void MainWindow::on_disconnectButton_clicked(){
    p.stop();
    ui->connectButton->setEnabled(true);
    ui->computeButton->setEnabled(false);
    ui->disconnectButton->setEnabled(false);
    writeDebug("Disonnected");
}

void MainWindow::writeDebug(QString qstr) {
    ui->textDebug->setText(ui->textDebug->toPlainText() + qstr + "\n");
}

void MainWindow::on_computeButton_clicked(){
    bool ok = false;

    int count, order;
    double xMax, xMin;
    xMin = ui->xBeg->text().toDouble(&ok);
    if (ok) {
        xMax = ui->xEnd->text().toDouble(&ok);
        if (xMax <= xMin + 0.1) {
            xMax = xMin + 0.1;
            ui->xEnd->setText(QString::number(xMax));
        }
    }
    if (ok) {
        count = ui->nPoints->text().toInt(&ok);
        if (count < 2) {
            count = 2;
            ui->nPoints->setText(QString::number(count));
        }
        if (count > 1000) {
            count = 1000;
            ui->nPoints->setText(QString::number(count));
        }
    }
    if (ok) order = ui->order->text().toInt(&ok);

    if (!ok) {
       writeDebug("Non-numberal arguments");
       return;
    }

    QString name = ui->functionBox->currentText();
    wchar_t* wname = (wchar_t*) calloc(name.length() + 1, sizeof(wchar_t));
    name.toWCharArray(wname);
    wname[name.length()] = L'\0';

    int size;
    double* arr = p.getData(&size, wname, xMin, xMax, count, order);

    if (arr == NULL) {
        writeDebug("Failed to compute");
        return;
    }

    writeDebug("Computed");
    double yMin = arr[1], yMax = arr[1];
    QVector<double> x(size / 2), y(size / 2);
    for (int i = 0; i < size / 2; i++) {
      x[i] = arr[i * 2];
      y[i] = arr[i * 2 + 1];
      if (y[i] < yMin) yMin = y[i];
      if (y[i] > yMax) yMax = y[i];
    }
    free(arr);
    yMin *= yMin > 0 ? 0.9 : 1.1;
    yMax *= yMax > 0 ? 1.1 : 0.9;

    ui->myPlot->graph(0)->setData(x, y);

    ui->myPlot->xAxis->setRange(x[0], x[size / 2 - 1]);
    ui->myPlot->yAxis->setRange(yMin, yMax);

    ui->myPlot->replot();
}
