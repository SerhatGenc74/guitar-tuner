#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calculation.h"

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ReadData();
    void FindString(float freq);
    QString getDynamicColor(float cents);


private:
    Ui::MainWindow *ui;
    QSerialPort *serial;

};
#endif // MAINWINDOW_H
