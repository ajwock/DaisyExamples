#include "daisy_patch_sm.h"
#include "daisysp.h"
#include "OctaveQuantizer.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

DaisyPatchSM patch;
Switch button;
Switch toggle;


/*
OctaveQuantizer chromatic;
OctaveQuantizer major;
OctaveQuantizer minor;
OctaveQuantizer pent_maj;
OctaveQuantizer pent_min;
OctaveQuantizer arp_maj;
OctaveQuantizer arp_min;
OctaveQuantizer octave_and_fifth;
OctaveQuantizer octave;
*/

#define N_QUANTIZERS 9
OctaveQuantizer quantizers[N_QUANTIZERS];

OctaveQuantizer *q;

uint8_t old_key;
uint8_t old_type;


void process_cvs();

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

    patch.Init();
    int i = 0;
    int j = 0;
    
    patch.WriteCvOut(CV_OUT_2, 3.3f);
    while (1) {
        patch.Delay(1);
        process_cvs();
    }
}

/*
 * CV1: Root note
 * CV2: Scale type
 * CV3: Octave
 * CV4: Range
 *
 * The base note in the given octave value is the 'base' tone, and input voltage
 * is added to this.
 *
 * Range is the number of semitones available above the base tone, 0-59.
 *
 * Scale type has values chromatic, major, minor, major pent, minor pent, major arp,
 * minor arp, octave and fifth, and octave.  Octave and fifth is just the base note and
 * the perfect fifth, for example in C major C and G.  Octave is just the base note.
 *
 * Through the use of bit rotation, key changes are an extremely efficient operation.
 */
void process_cvs()
{
    patch.ProcessAnalogControls();

    float knob_base = patch.GetAdcValue(CV_1);
    float knob_type = patch.GetAdcValue(CV_2);
    float knob_octave = patch.GetAdcValue(CV_3);
    float knob_range = patch.GetAdcValue(CV_4);

    float key_f = fmap(knob_base, 0.f, 11.999f);
    uint8_t key = (uint8_t) key_f;

    float type_f = fmap(knob_type, 0.f, 8.999f);
    uint8_t type = (uint8_t) type_f;

    if (key != old_key || type != old_type) {
        q = &quantizers[type];
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

    float midi_in = patch.GetAdcValue(CV_5);
    float midi_conv = fmap(midi_in, 0.f, range * 12.f);
    float midi = fclamp(midi_conv + octave * 12.f, 0.f, 127.f);

    float out = q->quantize(midi);

    out *= (5.f / 128.f);

    //uint16_t out_i = (uint16_t) out;
    patch.WriteCvOut(CV_OUT_1, out);
    patch.WriteCvOut(CV_OUT_2, out);
}
