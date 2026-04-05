#ifndef CALCULATION_H
#define CALCULATION_H
#include "tuning.h"
#include <cmath>
#include <QString>
#include "tuning.h"

typedef struct {
    QString noteName;
    int octave;
    float cents;
    int midi_num;
} TunerResult ;

class Calculation
{
public:
    Calculation();
    static TunerResult CalculateNote(float freq);

};

#endif // CALCULATION_H
