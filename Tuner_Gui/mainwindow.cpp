#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    serial->setPortName("/dev/ttyUSB1");
    serial->setBaudRate(QSerialPort::BaudRate::Baud115200);
    serial->setDataBits(QSerialPort::Data8);

    if(serial->open(QIODevice::ReadOnly))
    {
        connect(serial,&QSerialPort::readyRead,this,&MainWindow::ReadData);
    }
    else
    {
        qDebug() << "Hata var " << serial->errorString();
    }
    ui->slider_cent->setRange(-50,50);
    ui->slider_cent->setValue(0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FindString(float freq)
{
    TunerResult res;
    res = Calculation::CalculateNote(freq);
    QString currentColor = getDynamicColor(res.cents);
    QString style = QString("color: %1; font-weight: bold;").arg(currentColor);

    ui->slider_cent->setValue(res.cents);
    ui->lbl_cent->setText(QString::number(res.cents, 'f', 1));

    // E string (6. Tel)
    if(res.midi_num == 40) {
        ui->lbl_E_string->setStyleSheet(style);
    }
    // A string (5. Tel)
    else if(res.midi_num == 45) {
        ui->lbl_A_string->setStyleSheet(style);
    }
    // D string (4. Tel)
    else if(res.midi_num == 50) {
        ui->lbl_D_string->setStyleSheet(style);
    }
    // G string (3. Tel)
    else if(res.midi_num == 55) {
        ui->lbl_G_string->setStyleSheet(style);
    }
    // B string (2. Tel)
    else if(res.midi_num == 59) {
        ui->lbl_B_string->setStyleSheet(style);
    }
    // lower e string (1. Tel)
    else if(res.midi_num == 64) {
        ui->lbl_e_string->setStyleSheet(style);
    }
    else {

    }

    ui->slider_cent->setStyleSheet(QString(
    "QSlider::handle:horizontal { background-color: %1; border: 1px solid white; width: 10px; }"
    ).arg(currentColor));
}

QString MainWindow::getDynamicColor(float cents) {
    float absCents = std::abs(cents);
    int r = 0, g = 0, b = 0;

    if (absCents <= 25.0f) {
        r = static_cast<int>((absCents / 25.0f) * 255);
        g = 255;
    }
    else {
        r = 255;
        float factor = (absCents - 25.0f) / 25.0f;
        if (factor > 1.0f) factor = 1.0f;
        g = static_cast<int>(255 * (1.0f - factor));
    }

    return QColor(r, g, b).name();
}
void MainWindow::ReadData()
{
    static QByteArray buffer;
    buffer.append(serial->readAll());

    qDebug() << "Gelen Ham Veri (HEX):" << buffer.toHex();

    while (buffer.size() >= 5) {
        if ((quint8)buffer.at(0) == 0xAA) {
            float gelenHz;
            std::memcpy(&gelenHz, buffer.constData() + 1, 4);

            qDebug() << "Okunan Frekans:" << QString::number(gelenHz, 'f', 1) << "Hz";
            ui->lblHz->setText(QString::number(gelenHz, 'f', 1));

            FindString(gelenHz);
            buffer.remove(0, 5);
        }
        else {
            buffer.remove(0, 1);
        }
    }
}
