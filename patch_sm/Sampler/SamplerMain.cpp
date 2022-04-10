#include "daisy_patch_sm.h"
#include "daisysp.h"
#include "Sampler.h"
#include "Quantizer.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

DaisyPatchSM patch;
Switch button;
Switch toggle;

SimpleSampler s;
ArbitraryQuantizer q;
//Crossfade c;

#define AUDIO_BUF_SIZE 96000 * 60
float DSY_SDRAM_BSS buffer[AUDIO_BUF_SIZE];
bool last_button_state = 0;
bool last_toggle_state = 0;
bool last_rec_gate = 0;
bool last_play_gate = 0;
float last_velocity_exp = 1.f;

void toggleRecording(SimpleSampler *samp) {
    if (samp->rec_state == RECORDING)
        samp->endRecording();
    else
        samp->startRecording();
}

void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    patch.ProcessAllControls();
    button.Debounce();
    toggle.Debounce();

    float feedback = patch.GetAdcValue(CV_2);
    float fb_cv = patch.GetAdcValue(CV_6);
    feedback = fclamp(feedback + fb_cv, 0.f, 1.f);

    s.setFeedback(feedback);

    bool rec_gate = patch.gate_in_1.State();
    if (rec_gate && !last_rec_gate)
        toggleRecording(&s);

    bool play_gate = patch.gate_in_2.State();
    if (play_gate && !last_play_gate)
        s.startPlaying();

    bool button_state = button.Pressed();
    if (button_state && !last_button_state)
        s.startPlaying();

/*
    bool toggle_state = toggle.Pressed();
    if (toggle_state && !last_toggle_state)
        s.startRecording();
    if (!toggle_state && last_toggle_state)
        s.endRecording();
*/

    float velocity_exp = patch.GetAdcValue(CV_1);
    float vel_cv = patch.GetAdcValue(CV_5);
    velocity_exp = fclamp(vel_cv + velocity_exp, 0.f, 1.f);
    velocity_exp = fmap(velocity_exp, -7.f, 7.f);

    // A good bit of math is done here, memoize so we only do it when changing value.
    if (velocity_exp != last_velocity_exp) {
        // Toggle switch decides whether the velocity is quantized.
        bool toggle_state = toggle.Pressed();
        if (toggle_state)
            velocity_exp = q.quantize(velocity_exp);

        bool negative = 0;
        if (velocity_exp <= 0) {
            negative = 1;
            velocity_exp = -velocity_exp;
        }
        // Our range of speeds is from 1/8x - 8x as fast, curving exponentially, for 
        // positive or negative speeds.
        // Represented as powers of 2 from -3 to +3 on the input.
        velocity_exp -= 4.f;

        float velocity = powf(2.f, velocity_exp);
        if (negative)
            velocity = -velocity;

        s.setVelocity(velocity);

        last_velocity_exp = velocity_exp;
    }

    for (size_t i = 0; i < size; i++)
    {
        float dryl = IN_L[i];
        float sendl = s.Process(dryl);       
        OUT_L[i] = sendl;
        OUT_R[i] = sendl;
    }

    last_button_state = button_state;
  //  last_toggle_state = toggle_state;
    last_rec_gate = rec_gate;
  //  last_play_gate = play_gate;
}


int main(void)
{
    float quanta[] = {-7.f, -6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f};
    patch.Init();
    button.Init(patch.B7);
    toggle.Init(patch.B8);
    s = SimpleSampler(buffer, (size_t) AUDIO_BUF_SIZE);
    size_t n_quanta = sizeof(quanta) / sizeof(float);
    q = ArbitraryQuantizer(quanta, n_quanta, n_quanta);
    patch.StartAudio(AudioCallback);
    while(1){
        if (s.rec_state == RECORDING)
            patch.WriteCvOut(CV_OUT_2, 2.5);
        else
            patch.WriteCvOut(CV_OUT_2, 0.0);
    }
}

