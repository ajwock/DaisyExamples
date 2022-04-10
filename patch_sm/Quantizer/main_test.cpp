#include "OctaveQuantizer.h"

#include <cstdio>
#include <cstdint>

float process_cvs();

#define N_QUANTIZERS 9
OctaveQuantizer quantizers[N_QUANTIZERS];

OctaveQuantizer *q;

uint8_t old_key;
uint8_t old_type;

int main(void)
{
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

    //patch.Init();
    int i = 0;
    int j = 0;
    
    //patch.WriteCvOut(CV_OUT_2, 3.3f);
    while (1) {
        //patch.Delay(1);
        i++;
        /*if (i == 500) {
            if (j == 0)
                patch.WriteCvOut(CV_OUT_2, 3.3f);
            else
                patch.WriteCvOut(CV_OUT_2, 0.f);
            i = 0;
            j = !j;
        } */
        printf("Out: %f\n", process_cvs());
    }
}

inline float fclamp(float in, float min, float max)
{
    if (in > max)
        return max;
    if (in < min)
        return min;
    return in;
}

inline float
fmap(float in, float min, float max)
{
    return fclamp(min + in * (max - min), min, max);
}

float process_cvs()
{
    //patch.ProcessAnalogControls();

    float knob_base = .5;
    float knob_type = .5;
    float knob_octave = .5;
    float knob_range = .5;

    float key_f = fmap(knob_base, 0.f, 11.999f);
    uint8_t key = (uint8_t) key_f;

    float type_f = fmap(knob_type, 0.f, 8.999f);
    uint8_t type = (uint8_t) type_f;

    if (key != old_key || type != old_type) {
        OctaveQuantizer *q = &quantizers[type];
        if (key != q->root) {
            q->change_key(key);
        }
        old_key = key;
        old_type = type;
    }

    float octave_f = fmap(knob_octave, 0.f, 9.999f);
    uint8_t octave = (uint8_t) octave_f;

    float range_f = fmap(knob_range, 1.f, 6.999f);
    uint8_t range = (uint8_t) range_f;

    float midi_in = .5;
    float midi_conv = fmap(midi_in, 0.f, range * 12.f);
    float midi = fclamp(midi_conv + octave * 12.f, 0.f, 127.f);

    float out = q->quantize(midi);

    out *= (5.f / 128.f);

    //uint16_t out_i = (uint16_t) out;
    return out;
    //patch.WriteCvOut(CV_OUT_2, out);
}
