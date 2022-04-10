#include <gtest/gtest.h>

#include "Sampler.h"
#include "Sampler.cpp"

// Ensure that the constructor clears the passed buffer
TEST(SamplerTest, SimpleSampler_Constructor) {
    size_t buflen = 1024;
    float buf[buflen];

    // Fill with some nonzero data to make sure that the
    // data is zeroed.
    for (int i = 0; i < 1024; i++)
        buf[i] = (float) i;
    SimpleSampler s = SimpleSampler(buf, buflen);

    EXPECT_EQ(s.buffer_length, 1024);
    for (int i = 0; i < 1024; i++)
        EXPECT_EQ(0.f, buf[i]);
}

TEST(SamplerTest, SimpleSampler_record) {
    size_t buflen = 1024;
    float data_buf[buflen];
    float sampler_buf[buflen];
    float sent;

    for (int i = 0; i < 1024; i++)
        data_buf[i] = (float) i * 3;
    SimpleSampler s = SimpleSampler(sampler_buf, buflen);

    EXPECT_EQ(NOT_RECORDING, s.rec_state);
    EXPECT_EQ(1, s.length_in_use);
    EXPECT_EQ(0, s.length_recorded);
    EXPECT_EQ(0, s.record_idx);

    // Not recording yet, so nothing should change.
    sent = s.Process(5.f);
    EXPECT_EQ(NOT_RECORDING, s.rec_state);
    EXPECT_EQ(1, s.length_in_use);
    EXPECT_EQ(0, s.length_recorded);
    EXPECT_EQ(0, s.record_idx);
    EXPECT_EQ(0.f, sent);

    s.startRecording();
    sent = s.Process(5.f);
    EXPECT_EQ(RECORDING, s.rec_state);
    EXPECT_EQ(1, s.length_in_use);
    EXPECT_EQ(1, s.length_recorded);
    EXPECT_EQ(1, s.record_idx);
    EXPECT_EQ(0.f, sent);
    EXPECT_EQ(5.f, sampler_buf[0]);

    sent = s.Process(4.f);
    EXPECT_EQ(RECORDING, s.rec_state);
    EXPECT_EQ(1, s.length_in_use);
    EXPECT_EQ(2, s.length_recorded);
    EXPECT_EQ(2, s.record_idx);
    EXPECT_EQ(0.f, sent);
    EXPECT_EQ(5.f, sampler_buf[0]);
    EXPECT_EQ(4.f, sampler_buf[1]);

    s.endRecording();
    sent = s.Process(7.f);
    EXPECT_EQ(NOT_RECORDING, s.rec_state);
    EXPECT_EQ(2, s.length_in_use);
    EXPECT_EQ(0, s.length_recorded);
    EXPECT_EQ(0, s.record_idx);
    EXPECT_EQ(0.f, sent);
    EXPECT_EQ(5.f, sampler_buf[0]);
    EXPECT_EQ(4.f, sampler_buf[1]);
    EXPECT_EQ(0.f, sampler_buf[2]);

    s.startRecording();
    sent = s.Process(-2.f);
    EXPECT_EQ(RECORDING, s.rec_state);
    EXPECT_EQ(2, s.length_in_use);
    EXPECT_EQ(1, s.length_recorded);
    EXPECT_EQ(1, s.record_idx);
    EXPECT_EQ(0.f, sent);
    EXPECT_EQ(-2.f, sampler_buf[0]);
    EXPECT_EQ(4.f, sampler_buf[1]);
    EXPECT_EQ(0.f, sampler_buf[2]);

    s.endRecording();
    s.startRecording();

    for (int i = 0; i < 1024; i++)
        s.Process((float) i * 3);
    EXPECT_EQ(RECORDING, s.rec_state);
    EXPECT_EQ(1, s.length_in_use);
    EXPECT_EQ(1024, s.length_recorded);
    // Should have bounded back to the beginning
    EXPECT_EQ(0, s.record_idx);
    EXPECT_EQ(0.f, sent);
    for (int i = 0; i < 1024; i++)
        EXPECT_EQ(data_buf[i], sampler_buf[i]);
    s.Process(2.5f);
    EXPECT_EQ(1, s.length_in_use);
    EXPECT_EQ(1024, s.length_recorded);
    EXPECT_EQ(1, s.record_idx);
    EXPECT_EQ(2.5f, sampler_buf[0]);

    s.endRecording();
    EXPECT_EQ(NOT_RECORDING, s.rec_state);
    EXPECT_EQ(1024, s.length_in_use);
    EXPECT_EQ(0, s.length_recorded);
    EXPECT_EQ(0, s.record_idx);
    EXPECT_EQ(0.f, sent);
}


TEST(SamplerTest, SimpleSampler_play) {
    size_t buflen = 1024;
    float data_buf[buflen];
    float sampler_buf[buflen];
    float sent;

    for (int i = 0; i < 1024; i++)
        data_buf[i] = (float) i * 3;
    SimpleSampler s = SimpleSampler(sampler_buf, buflen);

    EXPECT_EQ(NOT_PLAYING, s.play_state);
    EXPECT_EQ(0, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(1.f, s.velocity);

    s.length_in_use = 10;
    for (int i = 0; i < 10; i++)
        sampler_buf[i] = (float) i * 3;

    s.startPlaying();
    sent = s.Process(4.f);
    EXPECT_EQ(PLAYING, s.play_state);
    EXPECT_EQ(1, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(1.f, s.velocity);
    EXPECT_EQ(0.f, sent);

    sent = s.Process(1.f);
    EXPECT_EQ(PLAYING, s.play_state);
    EXPECT_EQ(2, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(1.f, s.velocity);
    EXPECT_EQ(3.f, sent);

    for (int i = s.play_idx; i < 10; i++)
        sent = s.Process(1.f);
    EXPECT_EQ(NOT_PLAYING, s.play_state);
    EXPECT_EQ(0, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(1.f, s.velocity);
    EXPECT_EQ(27.f, sent);

    s.setVelocity(.5f);
    s.startPlaying();
    sent = s.Process(4.f);
    EXPECT_EQ(PLAYING, s.play_state);
    EXPECT_EQ(0, s.play_idx);
    EXPECT_EQ(.5f, s.play_idx_offset);
    EXPECT_EQ(.5f, s.velocity);
    EXPECT_EQ(0.f, sent);

    sent = s.Process(4.f);
    EXPECT_EQ(PLAYING, s.play_state);
    EXPECT_EQ(1, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);;
    EXPECT_EQ(.5f, s.velocity);
    EXPECT_EQ(1.5f, sent);
    for (float i = s.play_idx; i < 9.5f; i += .5f) {
        sent = s.Process(1.f);
        EXPECT_EQ(i * 3.f, sent);
    }
    EXPECT_EQ(PLAYING, s.play_state);
    EXPECT_EQ(9, s.play_idx);
    EXPECT_EQ(.5f, s.play_idx_offset);
    EXPECT_EQ(.5f, s.velocity);
    EXPECT_EQ(27.f, sent);

    sent = s.Process(1.f);
    EXPECT_EQ(NOT_PLAYING, s.play_state);
    EXPECT_EQ(0, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(.5F, s.velocity);
    EXPECT_EQ(13.5f, sent);

    s.setVelocity(-1.5f);
    s.startPlaying();
    EXPECT_EQ(9, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(-1.5f, s.velocity);
    sent = s.Process(1.f);
    EXPECT_EQ(8, s.play_idx);
    EXPECT_EQ(-.5f, s.play_idx_offset);
    EXPECT_EQ(27.f, sent);

    sent = s.Process(1.f);
    EXPECT_EQ(6, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(22.5f, sent);

    for (float i = s.play_idx; i > 0; i -= 1.5f) {
        sent = s.Process(1.5);
        EXPECT_EQ(i * 3.f, sent);
    }
    EXPECT_EQ(PLAYING, s.play_state);
    EXPECT_EQ(0, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(4.5f, sent);

    sent = s.Process(1.f);
    EXPECT_EQ(NOT_PLAYING, s.play_state);
    EXPECT_EQ(9, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(0.f, sent);

    s.setVelocity(-.5f);
    s.play_state = PLAYING;
    s.play_idx = 0;
    s.play_idx_offset = 0.f;
    sent = s.Process(1.f);
    EXPECT_EQ(PLAYING, s.play_state);
    EXPECT_EQ(0, s.play_idx);
    EXPECT_EQ(-.5f, s.play_idx_offset);
    EXPECT_EQ(0.f, sent);

    sent = s.Process(1.f);
    EXPECT_EQ(NOT_PLAYING, s.play_state);
    EXPECT_EQ(9, s.play_idx);
    EXPECT_EQ(0.f, s.play_idx_offset);
    EXPECT_EQ(13.5f, sent);
}
