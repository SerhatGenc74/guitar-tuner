#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cmath> // std::abs, std::round, std::log2 için gerekli

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    serial->setPortName("/dev/ttyUSB0"); // Senin portun
    serial->setBaudRate(QSerialPort::BaudRate::Baud115200);
    serial->setDataBits(QSerialPort::Data8);

    if(serial->open(QIODevice::ReadOnly))
    {
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::ReadData);
    }
    else
    {
        qDebug() << "Hata var " << serial->errorString();
    }

    ui->slider_cent->setRange(-50, 50);
    ui->slider_cent->setValue(0);

    // Başlangıçta arayüzü sıfırla
    resetUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// === EKLENEN YARDIMCI FONKSİYONLAR ===
void MainWindow::resetUI()
{
    ui->lbl_E_string->setStyleSheet("");
    ui->lbl_A_string->setStyleSheet("");
    ui->lbl_D_string->setStyleSheet("");
    ui->lbl_G_string->setStyleSheet("");
    ui->lbl_B_string->setStyleSheet("");
    ui->lbl_e_string->setStyleSheet("");

    ui->slider_cent->setValue(0);
    ui->lbl_cent->setText("0.0");
    ui->lblHz->setText("—");

    updateSliderColor("gray");
}

void MainWindow::updateSliderColor(const QString& color)
{
    // Aynı renk arka arkaya gelirse baştan UI'ı yormamak için kontrol:
    if (color == m_lastSliderColor) return;
    m_lastSliderColor = color;

    // Daha şık, modern ve akıcı bir slider stili
    ui->slider_cent->setStyleSheet(QString(
                                       "QSlider::handle:horizontal { background-color: %1; border: 1px solid white; width: 14px; height: 14px; border-radius: 7px; margin: -5px 0; }"
                                       "QSlider::groove:horizontal { height: 4px; background: palette(mid); border-radius: 2px; }"
                                       "QSlider::sub-page:horizontal { background: %1; border-radius: 2px; }"
                                       ).arg(color));
}

// === GÜNCELLENEN FINDSTRING FONKSİYONU ===
void MainWindow::FindString(float rawFreq)
{
    // Geçersiz veya sessizlik durumu varsa her şeyi sıfırla
    if (rawFreq < 20.0f || rawFreq > 1500.0f) {
        resetUI();
        m_smoothedFreq = 0.0f;
        return;
    }

    // 1. ADIM: FREKANS YUMUŞATMA (Gürültü ve Pırpır Önleyici Filtre)
    const float ALPHA = 0.15f;
    if (m_smoothedFreq < 20.0f) {
        m_smoothedFreq = rawFreq; // İlk okumayı direkt al
    } else {
        m_smoothedFreq = (ALPHA * rawFreq) + ((1.0f - ALPHA) * m_smoothedFreq);
    }

    // 2. ADIM: EN YAKIN GİTAR TELİNE KİLİTLENME
    struct GuitarString {
        int midi;
        float targetFreq;
        QLabel* label;
    };

    GuitarString strings[] = {
        {40, 82.41f,  ui->lbl_E_string},
        {45, 110.00f, ui->lbl_A_string},
        {50, 146.83f, ui->lbl_D_string},
        {55, 196.00f, ui->lbl_G_string},
        {59, 246.94f, ui->lbl_B_string},
        {64, 329.63f, ui->lbl_e_string}
    };

    GuitarString closestString = strings[0];
    float minDiff = std::abs(m_smoothedFreq - strings[0].targetFreq);

    for (int i = 1; i < 6; ++i) {
        float diff = std::abs(m_smoothedFreq - strings[i].targetFreq);
        if (diff < minDiff) {
            minDiff = diff;
            closestString = strings[i];
        }
    }

    // 3. ADIM: CENT HESABI VE EKRANA BASTIRMA
    float cents = 1200.0f * std::log2(m_smoothedFreq / closestString.targetFreq);

    float displayCents = cents;
    if (displayCents > 50.0f) displayCents = 50.0f;
    if (displayCents < -50.0f) displayCents = -50.0f;

    QString currentColor = getDynamicColor(displayCents);

    // Bütün tellerin renklerini temizle, sadece hedeftekini boya
    for (int i = 0; i < 6; ++i) {
        strings[i].label->setStyleSheet("");
    }
    QString style = QString("color: %1; font-weight: bold;").arg(currentColor);
    closestString.label->setStyleSheet(style);

    ui->slider_cent->setValue(static_cast<int>(std::round(displayCents)));

    QString centText = (displayCents >= 0)
                           ? QString("+%1").arg(displayCents, 0, 'f', 1)
                           : QString::number(displayCents, 'f', 1);
    ui->lbl_cent->setText(centText);

    updateSliderColor(currentColor);
}

// === GÜNCELLENEN RENK HESAPLAYICI (Sarı-Yeşil-Kırmızı geçişleri düzeltildi) ===
QString MainWindow::getDynamicColor(float cents) {
    float absCents = std::abs(cents);
    int r = 0, g = 0, b = 0;

    if (absCents <= 5.0f) {
        // Tam akortlu (Yeşil)
        g = 255;
    } else if (absCents <= 25.0f) {
        // Yeşilden Sarıya geçiş
        float t = (absCents - 5.0f) / 20.0f;
        r = static_cast<int>(t * 255);
        g = 255;
    }
    else {
        // Sarıdan Kırmızıya geçiş
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

    // Debug print'i serial akışını engellememesi için kapattım, istersen geri açabilirsin
    // qDebug() << "Gelen Ham Veri (HEX):" << buffer.toHex();

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
