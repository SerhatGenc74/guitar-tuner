#ifndef TUNING_H
#define TUNING_H
#include <QString>

class Tuning
{
public:
    Tuning();
    static inline const QString noteNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    void WhichString();

};

#endif // TUNING_H
