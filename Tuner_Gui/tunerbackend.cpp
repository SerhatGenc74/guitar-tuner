#include "tunerbackend.h"
#include <cstring>
#include <QDebug>

TunerBackend::TunerBackend(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    serial->setPortName("/dev/ttyUSB0"); // Kendi portuna göre değiştir
    serial->setBaudRate(QSerialPort::BaudRate::Baud115200);
    serial->setDataBits(QSerialPort::Data8);

    if(serial->open(QIODevice::ReadOnly)) {
        connect(serial, &QSerialPort::readyRead, this, &TunerBackend::ReadData);
    } else {
        qDebug() << "Seri Port Hatası:" << serial->errorString();
    }
}

void TunerBackend::ProcessFrequency(float rawFreq)
{
    if (rawFreq < 20.0f || rawFreq > 4000.0f) {
        m_smoothedFreq = 0.0f;
        return;
    }


    const float ALPHA = 0.15f;
    if (m_smoothedFreq < 20.0f) {
        m_smoothedFreq = rawFreq;
    } else {
        m_smoothedFreq = (ALPHA * rawFreq) + ((1.0f - ALPHA) * m_smoothedFreq);
    }
    emit freqChanged();


    TunerResult result = Calculation::CalculateNote(m_smoothedFreq);


    if (m_result.cents != result.cents) {
        m_result.cents = result.cents;
        emit centsChanged();
    }


    QString newNoteDisplay = QString("%1%2").arg(result.noteName).arg(result.octave);

    if (m_result.noteName != newNoteDisplay) {
        m_result.noteName = newNoteDisplay;
        emit noteChanged();
        emit midinumChanged();
    }
}

void TunerBackend::ReadData()
{
    static QByteArray buffer;
    buffer.append(serial->readAll());

    while (buffer.size() >= 5) {
        if ((quint8)buffer.at(0) == 0xAA) {
            float gelenHz;
            std::memcpy(&gelenHz, buffer.constData() + 1, 4);
            ProcessFrequency(gelenHz);
            buffer.remove(0, 5);
        } else {
            buffer.remove(0, 1);
        }
    }
}
