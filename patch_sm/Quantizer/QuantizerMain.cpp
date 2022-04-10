#include "daisy_patch_sm.h"
#include "daisysp.h"
#include "OctaveQuantizer.h"
#include "OQManager.h"

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


OQManager m;

void process_cvs();

int main(void)
{
    m = OQManager();
    m.setParams(0.f, 0.f);
    patch.Init();
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

    float knob_key = patch.GetAdcValue(CV_1);
    float knob_type = patch.GetAdcValue(CV_2);

    m.setParams(knob_key, knob_type);

    float note_in = patch.GetAdcValue(CV_5);

    float out = m.process(note_in);

    patch.WriteCvOut(CV_OUT_1, out);
    patch.WriteCvOut(CV_OUT_2, out);
}
