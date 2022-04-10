#include "OQManager.h"
#include "OctaveQuantizer.h"
#include "daisysp.h"

using namespace daisysp;

OQManager::OQManager() {

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

void OQManager::setParams(float adckey, float adctype) {
    float key_f = fmap(adckey, 0.f, 11.999f);
    uint8_t key = (uint8_t) key_f;

    float type_f = fmap(adctype, 0.f, 8.999f);
    uint8_t type = (uint8_t) type_f;

    if (key != old_key || type != old_type) {
        q = &quantizers[type];
        if (key != q->root) {
            q->change_key(key);
        }
        old_key = key;
        old_type = type;
    }
}

/*
 * By default, comply with v/oct standards.
 *
 * We can only output 0V-5V which is C3-C8. 
 * 
 * Of course, the oscillator can be tuned 1-2 octaves lower.
 */
float OQManager::process(float cvin) {
    // Map the input reading (5V represented as 0f-1f) to 5 octaves (60 midi semitones).
    float midi = fmap(cvin, 0.f, 60.f);
    // Make an input of 0V equal to middle C.
    midi += 60.f;

    float out = q->quantize(midi);

    // Reverse offset and scale for conversion to 0-5V.
    out -= 60.f;
    return out * (5.f / 60.f);
}
