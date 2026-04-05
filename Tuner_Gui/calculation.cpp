#include "calculation.h"

Calculation::Calculation() {}

TunerResult Calculation::CalculateNote(float freq)
{
    TunerResult result;
    float midi_num = 12.0f * log2(freq/440.0f) + 69.0f;
    int base_freq = std::round(midi_num);
    float octave = std::floor(midi_num/12) - 1;
    int index = base_freq % 12;
    float cent = (midi_num - base_freq) * 100.0f;
    result.noteName =  Tuning::noteNames[index];
    result.cents = cent;
    result.midi_num = midi_num;
    result.octave = octave;

    return result;
}
