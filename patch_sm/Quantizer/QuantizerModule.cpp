#include "QuantizerModule.h"
#include "OctaveQuantizer.h"

#include "daisysp.h"

using namespace daisysp;

QuantizerModule::QuantizerModule() {
    for (int i = 0; i < N_QUANTIZERS; i++)
        quantizers[i] = OctaveQuantizer();

    quantizers[0].set_map(CHROMATIC);
    quantizers[1].set_map(MAJOR);
    quantizers[2].set_map(MINOR);
    quantizers[3].set_map(PENT_MAJ);
    quantizers[4].set_map(PENT_MIN);
    quantizers[5].set_map(ARP_MAJ);
    quantizers[6].set_map(ARP_MIN);
    quantizers[7].set_map(OCT_FIFTH);
    quantizers[8].set_map(OCTAVE);

    old_key = C;
    old_type = 0;

    q = &quantizers[0];
}

void QuantizerModule::ProcessCVs(
    float cv1,
    float cv2,
    float cv3,
    float cv4,
    float *cv_out_1,
    float *cv_out_2
) {
    float key_f = fmap(cv1, 0.f, 11.999f);
    uint8_t key = (uint8_t) key_f;

    float type_f = fmap(cv2, 0.f, 8.999f);
    uint8_t type = (uint8_t) type_f;

    if (key != old_key || type != old_type) {
        OctaveQuantizer *q = &quantizers[type];
        if (key != q->root) {
            q->change_key(key);
        }
        old_key = key;
        old_type = type;
    }

    float octave_f = fmap(cv3, 0.f, 9.999f);
    uint8_t octave = (uint8_t) octave_f;

    float range_f = fmap(cv4, 1.f, 6.999f);
    uint8_t range = (uint8_t) range_f;

    float midi_in = cv5;
    float midi_conv = fmap(midi_in, 0.f, range * 12.f);
    float midi = fclamp(midi_conv + octave * 12.f, 0.f, 127.f);

    float out = q->quantize(midi);

    out *= (5.f / 128.f);

    *cv_out_1 = out;
    *cv_out_2 = out;
}
