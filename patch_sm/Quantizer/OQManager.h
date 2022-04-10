#ifndef OQMANAGER_H
#define OQMANAGER_H

#include "OctaveQuantizer.h"

#define N_QUANTIZERS 9


class OQManager {
    public:
    OctaveQuantizer quantizers[N_QUANTIZERS];
    OctaveQuantizer *q;
    uint8_t old_key;
    uint8_t old_type;

    uint8_t range;
    uint8_t octave;

    OQManager();

    void setParams(float, float);
    float process(float);
};

#endif
