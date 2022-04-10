#include "daisy_patch_sm.h"
#include "daisysp.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

DaisyPatchSM patch;
MoogLadder   filt;


float fold(float in, float amp) {
    float out = in;
    float diff;
    float namp = -amp;
    // i limits iterations to prevent infinite loops
    int i = 0;
    while (i < 10) {
        if (out > amp) {
            diff = out - amp;
            out = amp - diff;
        } else if (out < namp) {
            diff = out - namp;
            out = amp - diff;
        } else {
            return out;
        }
        i++;
    }
    return out;
}

float lastOut = 0.0f;

/*
 * Simple wavfolder has just this:
 * gain: How much input signal is multiplied before folding.
 *
 */
void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    patch.ProcessAllControls();
    float gain_knob = patch.GetAdcValue(CV_1);
    float gain = fmap(gain_knob, 0.1f, 5.0f);

    float out_level_knob = patch.GetAdcValue(CV_2);
    float out_level = fmap(out_level_knob, 0.1f, 1.0f);

    float cutoff_knob = patch.GetAdcValue(CV_3);
    float k_cutoff = fmap(cutoff_knob, 24.f, 108.f);

    float voct_cv = patch.GetAdcValue(CV_5);
    float voct = fmap(voct_cv, 0.f, 60.f);

    float midi_note = fclamp(voct + k_cutoff, 0.f, 127.f);
    float filter_cutoff = mtof(midi_note);

    float filter_res = patch.GetAdcValue(CV_4);

    filt.SetFreq(filter_cutoff);
    filt.SetRes(filter_res);

    for (size_t i = 0; i < size; i++)
    {
        float dryl = IN_L[i] * gain;
        float dryr = IN_L[i] * gain;
        float sendl = fold(dryl, out_level);
        float sendr = fold(dryr, out_level);
        sendl = filt.Process(sendl);
        sendr = filt.Process(sendr);
        OUT_L[i] = sendl;
        OUT_R[i] = sendr;
        lastOut = filter_cutoff * 2.5f / 128.f;
    }
}

int main(void)
{
    patch.Init();
    filt.Init(patch.AudioSampleRate());
    patch.StartAudio(AudioCallback);
    while(1){
        patch.Delay(1);
        patch.WriteCvOut(CV_OUT_2, lastOut);
    }
}
