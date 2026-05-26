#ifndef TUNERBACKEND_H
#define TUNERBACKEND_H

#include <QObject>
#include <QSerialPort>
#include "calculation.h"

class TunerBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float cents READ getCents NOTIFY centsChanged)
    Q_PROPERTY(QString note READ getNote NOTIFY noteChanged)
    Q_PROPERTY(float freq READ getFreq NOTIFY freqChanged)
    Q_PROPERTY(int midinum READ getMidiNum NOTIFY midinumChanged)

public:
    explicit TunerBackend(QObject *parent = nullptr);

    float getCents() const { return m_result.cents; }
    QString getNote() const { return m_result.noteName; }
    float getFreq() const { return m_smoothedFreq; }
    int getMidiNum() const { return m_result.midi_num; }

signals:

    void centsChanged();
    void noteChanged();
    void freqChanged();
    void midinumChanged();

private slots:
    void ReadData();

private:
    QSerialPort *serial;
    TunerResult m_result;
    float m_smoothedFreq = 0.0f;

    void ProcessFrequency(float rawFreq);
};

#endif // TUNERBACKEND_H
