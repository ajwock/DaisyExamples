#include "daisy_patch_sm.h"
#include "daisysp.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

DaisyPatchSM patch;

typedef enum {
    EIGHT = 1,
    SIXTEEN = 2,
    MAX = 3,
} bit_depth_type;

int last_bit_depth_type = 1;
int last_divisor = 1;

float set_sample_l = 0.0f;
float set_sample_r = 0.0f;
int j = 0;;

/* Converts the input float to an integer of the given bit depth, up to 32. */
float lofiify(float in, int bit_depth) {
    float mapped = fclamp(in, -1.0f, 1.0f);
    if (bit_depth <= 0) bit_depth = 1;
    if (bit_depth >= 32) bit_depth = 32;
    int32_t maxVal = 1 << (bit_depth - 1);
    mapped *= maxVal;
    int conv = (int) mapped;
    mapped = (float) conv;
    mapped /= (float) maxVal;
    return mapped;
}

/*
 * Lo-fi passthrough:
 *
 * CV_1 controls the bit depth with 4 options selectable from left to right:
 *  4-bit, 8-bit, 16-bit, max
 *
 * CV_2 controls the sample rate divsor.  Turning the knob incrementally increases the
 * sample rate divisor of 96khz.  i.e, all the way left is 96khz, a little right is
 * 96khz / 2, a little more right is / 3, so on until 96khz / 10.
 *
 * CV_3 controls the level of noise mixed in.
 *
 * Depends on what the hardware will give us- I believe 24-bit
 * is the most it will be able to truly resolve, but we will output 32 bit
 * floats regardless.  It's just that these floats will be cast from
 * lower resolution ints.
 */
void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    patch.ProcessAllControls();
    float bit_depth_float = patch.GetAdcValue(CV_1);
    /*bit_depth_float = fmap(bit_depth_float, 1.0f, 3.999f);
    int bit_depth_type = (int) bit_depth_float;
    last_bit_depth_type = bit_depth_type;
    */

    bit_depth_float = fmap(bit_depth_float, 1.0f, 32.999f);
    int bit_depth = (int) bit_depth_float;

    float sample_rate_divisor_float = patch.GetAdcValue(CV_2);
    sample_rate_divisor_float = fmap(sample_rate_divisor_float, 1.0f, 10.999f);
    int sample_rate_divisor = (int) sample_rate_divisor_float;
    last_divisor = sample_rate_divisor;

    for (size_t i = 0; i < size; i++)
    {
        if (j >= sample_rate_divisor) {
            set_sample_l = IN_L[i];
            set_sample_r = IN_R[i];
            /*
            switch (bit_depth_type) {
            case EIGHT:
                set_sample_l = lofiify(set_sample_l, 8);
                set_sample_r = lofiify(set_sample_r, 8);
                break;
            case SIXTEEN:
                set_sample_l = lofiify(set_sample_l, 16);
                set_sample_r = lofiify(set_sample_l, 16);
            default:
                // Full quality passthrough
                break;
            }
            */
            if (bit_depth < 24) {
                set_sample_l = lofiify(set_sample_l, bit_depth);
                set_sample_r = lofiify(set_sample_r, bit_depth);
            }
            j = 0;
        }
        OUT_L[i] = set_sample_l;
        OUT_R[i] = set_sample_r;
        j++;
    }
}

int main(void)
{
    patch.Init();
    patch.StartAudio(AudioCallback);
    while(1){
        patch.Delay(1000 / last_divisor);
        patch.WriteCvOut(CV_OUT_2, 2.5);
        patch.Delay(1000 / last_divisor);
        patch.WriteCvOut(CV_OUT_2, 0.0);
    }
}
