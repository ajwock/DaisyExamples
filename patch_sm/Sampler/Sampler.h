#ifndef SAMPLER_H
#define SAMPLER_H

typedef enum record_state {
    RECORDING,
    NOT_RECORDING,
} record_state;

typedef enum play_state {
    PLAYING,
    NOT_PLAYING,
} play_state;

typedef enum idx_crossings {
    NONE,
    END_TO_BEGINNING,
    BEGINNING_TO_END,
    CURRENT_END,
    PREVIOUS_END,
    START_IDX,
} idx_crossings;


class SimpleSampler {
    public:
    float *buffer; // Buffer to record to
    size_t buffer_length; // Length of buffer to record to
    size_t length_in_use; // Length of current sample
    size_t length_recorded;
    size_t play_idx; // Float index representing a position that may be between samples for interpolation.
    float  play_idx_offset; // Float offset above current index for interpolating faster or slower playbacks.
    size_t record_idx; // Current position being recorded to in buffer
    size_t start_idx; // Where to start playing from

    float feedback; // How much of old sample is mixed with new sample
    float velocity; // Speed of sample playing

    int rec_state;
    int prev_rec_state;
    int play_state;

    void clearBuffer();

    SimpleSampler() {}
    SimpleSampler(float *buf, size_t buf_len);

    void setFeedback(float fb);

    void setVelocity(float vel);

    void startRecording();

    void endRecording();

    void startPlaying();

    // When buffer is full, record from beginning.  Indicate a boolean value in case
    // the consumer wants to stop recording when this condition is met.
    int increment_rec_idx(int *idx);

    int record_sample(const float input);

    int increment_play_idx(size_t *idx, float *idx_offset);

    // Computes a weighted average between two floats.
    //
    // rightBias is a value between 0.0 and 1.0.  It is the weight
    // of the right hand side, and 1 - rightBias is the weight of the
    // left hand side.
    float weighted_avg(float left, float right, float rightBias);

    int u_mod(int input, int modulo);

    int get_next_sample(float *send);

    float Process(const float input);
};

#endif
