#ifndef QUANTIZER_MODULE_H
#define QUANTIZER_MODULE_H

#include "OctaveQuantizer.h"

#define N_QUANTIZERS 9

/*
 * Class defining how OctaveQuantizers will be used as a module.
 *
 * Also helps write unit tests that can take in cv values without being coupled to
 * the daisy libraries.
 */
class QuantizerModule {
    public:
    OctaveQuantizer quantizers[N_QUANTIZERS];
    OctaveQuantizer *q;

    uint8_t old_key;
    uint8_t old_type;

    QuantizerModule();

    void ProcessCVs(
        float cv1,
        float cv2,
        float cv3,
        float cv4,
        float cv5,
        float *cv_out_1,
        float *cv_out_2
    );

}

#endif
