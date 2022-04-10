#include <gtest/gtest.h>

#include "OctaveQuantizer.h"
#include "OQManager.h"
#include "OQManager.cpp"

#include "SemiSet.h"

TEST(OQManagerTest, OQManager_Constructor) {
    OQManager m = OQManager();
    EXPECT_EQ(C, m.old_key);
    EXPECT_EQ(0, m.old_type);
}

TEST(OQManagerTest, OQManager_setParams) {
    OQManager m = OQManager();

    m.setParams(0.f, 0.f);
    EXPECT_EQ(0, m.old_key);
    EXPECT_EQ(0, m.old_type);
    EXPECT_EQ(&m.quantizers[0], m.q);

    m.setParams(1.f / 11.99f, 1.f / 8.99f);
    EXPECT_EQ(1, m.old_key);
    EXPECT_EQ(1, m.old_type);
    EXPECT_EQ(&m.quantizers[1], m.q);
    EXPECT_EQ(1, m.q->root);

    m.setParams(2.f / 11.99f, 2.f / 8.99f);
    EXPECT_EQ(2, m.old_key);
    EXPECT_EQ(2, m.old_type);
    EXPECT_EQ(&m.quantizers[2], m.q);
    EXPECT_EQ(2, m.q->root);

    m.setParams(11.5f / 11.99f, 8.5f / 8.99f);
    EXPECT_EQ(11, m.old_key);
    EXPECT_EQ(8, m.old_type);
    EXPECT_EQ(&m.quantizers[8], m.q);
    EXPECT_EQ(11, m.q->root);

    m.setParams(11.5f / 11.99f, 0.f);
    EXPECT_EQ(11, m.old_key);
    EXPECT_EQ(0, m.old_type);
    EXPECT_EQ(&m.quantizers[0], m.q);
    EXPECT_EQ(11, m.q->root);

    m.setParams(6.0f / 11.99f, 0.f);
    EXPECT_EQ(6, m.old_key);
    EXPECT_EQ(0, m.old_type);
    EXPECT_EQ(&m.quantizers[0], m.q);
    EXPECT_EQ(6, m.q->root);
}

TEST(OQManagerTest, OQManager_process) {
    float out;
    OQManager m = OQManager();

    m.setParams(0.f, 0.f);

    out = m.process(0.f);
    EXPECT_FLOAT_EQ(0.f, out);

    out = m.process(1.f / 5.f);
    EXPECT_FLOAT_EQ(1.f, out);

    out = m.process(2.f / 5.f);
    EXPECT_FLOAT_EQ(2.f, out);

    out = m.process(3.f / 5.f);
    EXPECT_FLOAT_EQ(3.f, out);

    out = m.process(4.f / 5.f);
    EXPECT_FLOAT_EQ(4.f, out);

    out = m.process(1.f);
    EXPECT_FLOAT_EQ(5.f, out);

    // Key of C octave-fifth quantization
    m.setParams(0.f, 7.f / 8.99f);
    out = m.process(4.f / 60.f);
    EXPECT_FLOAT_EQ(7.f * (5.f / 60.f), out);

    // Key of D octave-fifth quantization
    m.setParams(1.f / 11.99f, 7.f / 8.99f);
    out = m.process(5.f / 60.f);
    EXPECT_FLOAT_EQ(8.f * (5.f / 60.f), out);
}
