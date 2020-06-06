#pragma once
#include <QMainWindow>
#include <headers/piper.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Piper p;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();

    void on_exitButton_clicked();

    void on_disconnectButton_clicked();

    void on_computeButton_clicked();

    void writeDebug(QString qstr);
private:
    Ui::MainWindow *ui;
};

