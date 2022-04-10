#include "Sampler.h"

void SimpleSampler::clearBuffer() {
    for (int i = 0; i < buffer_length; i++)
        buffer[i] = 0.f;
}

SimpleSampler::SimpleSampler(float *buf, size_t buf_len) {
    buffer = buf;
    buffer_length = buf_len;
    length_in_use = 1;
    length_recorded = 0;
    play_idx = 0;
    play_idx_offset = 0.0f;
    record_idx = 0;
    start_idx = 0;
    feedback = 0.f;
    velocity = 1.f;
    rec_state = NOT_RECORDING;
    prev_rec_state = NOT_RECORDING;
    play_state = NOT_PLAYING;
    clearBuffer();
}

void SimpleSampler::setFeedback(float fb) {
    if (fb > 1.f) fb = 1.f;
    if (fb < 0.f) fb = 0.f;
    feedback = fb;
}

void SimpleSampler::setVelocity(float vel) {
    velocity = vel;
}

void SimpleSampler::startRecording() {
    // Save last recording
    if (length_recorded > 0)
        endRecording();
    rec_state = RECORDING;
}

void SimpleSampler::endRecording() {
    rec_state = NOT_RECORDING;
    if (length_recorded > 0)
        length_in_use = length_recorded;
    else
        length_in_use = 1;
    length_recorded = 0;
    record_idx = 0;
}

void SimpleSampler::startPlaying() {
    if (velocity < 0)
        play_idx = length_in_use - 1;
    else
        play_idx = 0;
    play_idx_offset = 0.0f;
    play_state = PLAYING;
}

// When buffer is full, record from beginning.  Indicate a boolean value in case
// the consumer wants to stop recording when this condition is met.
int SimpleSampler::increment_rec_idx(int *idx) {
    *idx += 1;
    if (*idx >= buffer_length) {
        *idx = 0;
        return 1;
    }
    return 0;
}

int SimpleSampler::record_sample(const float input) {
    float old_sample = buffer[record_idx % length_in_use];
    buffer[record_idx] = input + old_sample * feedback;
    if (length_recorded < buffer_length)
        length_recorded++;
    return increment_rec_idx((int *) &record_idx);
}

int SimpleSampler::increment_play_idx(size_t *idx, float *idx_offset) {
    int prev_idx = *idx;
    int vel_int = (int) velocity;
    int rc = 0;
    float vel_float_delta = (float) vel_int;
    vel_float_delta = velocity - vel_float_delta;
    int int_idx = *idx + vel_int;
    float total_offset = *idx_offset + vel_float_delta;
    if (total_offset >= 1.f) {
        int_idx++;
        total_offset -= 1.f;
    } else if (total_offset <= -1.f) {
        int_idx--;
        total_offset += 1.f;
    }
    *idx_offset = total_offset;
    while (int_idx >= (int) length_in_use) {
        int_idx -= length_in_use;
        rc = 1;
    }
    while (int_idx < 0) {
        int_idx += length_in_use;
        rc = 1;
    }
    *idx = int_idx;
    // TODO: Handle variable start position
    return rc;
}

// Computes a weighted average between two floats.
//
// rightBias is a value between 0.0 and 1.0.  It is the weight
// of the right hand side, and 1 - rightBias is the weight of the
// left hand side.
float SimpleSampler::weighted_avg(float left, float right, float rightBias) {
    return left * (1.f - rightBias) + right * rightBias;
}

int SimpleSampler::u_mod(int input, int modulo) {
    while (input < 0)
        input += modulo;
    return input % modulo;
}

int SimpleSampler::get_next_sample(float *send) {
    float left;
    float right;
    if (play_idx_offset >= 0) {
        left = buffer[play_idx];
        right = buffer[play_idx, u_mod(play_idx + 1, length_in_use)];
        *send = weighted_avg(left, right, play_idx_offset);
    } else {
        left = buffer[u_mod(play_idx - 1, length_in_use)];
        right = buffer[play_idx];
        *send = weighted_avg(left, right, play_idx_offset + 1.f);
    }
    int rc = increment_play_idx(&play_idx, &play_idx_offset);
    return rc;
}

float SimpleSampler::Process(const float input)
{
    float send;
    int rc;
    switch (rec_state) {
    case RECORDING:
        record_sample(input);
        break;
    case NOT_RECORDING:
        break;
    default:
        break;
    }

    switch (play_state) {
    case PLAYING:
        rc = get_next_sample(&send);
        /*if (rc) {
            play_state = NOT_PLAYING;
            play_idx_offset = 0.f;
        }*/
        break;
    default:
        send = 0.0f;
        break;
    }
    return send;
}
