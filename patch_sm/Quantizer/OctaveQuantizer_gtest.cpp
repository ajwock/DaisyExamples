#include <gtest/gtest.h>

#include "OctaveQuantizer.h"
#include "OctaveQuantizer.cpp"

#include "SemiSet.h"

class SemiSet {
    public:
    std::set<uint8_t> semis;

    SemiSet() {}

    void add_semi(uint8_t semi) {
        semis.insert(semi);
    }

    void remove_semi(uint8_t semi) {
        semis.erase(semi);
    }

    void clear() {
        semis.clear();
    }

    ::testing::AssertionResult quantizer_semis_equal(OctaveQuantizer *q) {
        char buf[1024];
        size_t bufsize = sizeof(buf);
        for (uint8_t semi = C; semi <= B; semi++) {
            if (q->is_quantum(semi)) {
                if (semis.find(semi) == semis.end())
                    return ::testing::AssertionFailure() << "Expected " << semi_str(semi) <<
                        " not to be a quantum, but it was present.\nQuantizer:\n" <<
                        q->quantizer_str(buf, bufsize); 
            } else {
                if (semis.find(semi) != semis.end())
                    return ::testing::AssertionFailure() << "Expected " << semi_str(semi) <<
                        " to be a quantum, but it was not present.\nQuantizer:\n" <<
                        q->quantizer_str(buf, bufsize);
            }
        }
        return ::testing::AssertionSuccess();
    }

};

TEST(OctaveQuantizerTest, OctaveQuantizer_Constructor) {
    OctaveQuantizer q = OctaveQuantizer();
    EXPECT_EQ(C, q.root);
    EXPECT_TRUE(q.is_empty());
    EXPECT_EQ(INFINITY, q.last_out);
    EXPECT_EQ(INFINITY, q.last_in);
}

TEST(OctaveQuantizerTest, OctaveQuantizer_add_or_remove_quanta) {
    OctaveQuantizer q = OctaveQuantizer();
    SemiSet s = SemiSet();

    q.add_quantum(C);
    s.add_semi(C);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));

    q.add_quantum(D);
    s.add_semi(D);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));

    q.add_quantum(B);
    s.add_semi(B);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));

    q.remove_quantum(D);
    s.remove_semi(D);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
}

TEST(OctaveQuantizerTest, OctaveQuantizer_quantize) {
    float out;
    OctaveQuantizer q = OctaveQuantizer();
    q.add_quantum(C);
    q.add_quantum(E);
    q.add_quantum(G);

    out = q.quantize(60.5f);
    EXPECT_EQ(60.f, out);

    out = q.quantize(59.5f);
    EXPECT_EQ(60.f, out);

    out = q.quantize(62.001f);
    EXPECT_EQ(64.f, out);

    out = q.quantize(62.f);
    EXPECT_EQ(60.f, out);

    out = q.quantize(68.f);
    EXPECT_EQ(67.f, out);

    out = q.quantize(70.f);
    EXPECT_EQ(72.f, out);

    out = q.quantize(5.f);
    EXPECT_EQ(4.f, out);

    out = q.quantize(6.f);
    EXPECT_EQ(7.F, out);

    out = q.quantize(1.f);
    EXPECT_EQ(0.f, out);

    out = q.quantize(126.f);
    EXPECT_EQ(127.f, out);
}

TEST(OctaveQuantizerTest, OctaveQuantizer_change_key) {
    OctaveQuantizer q = OctaveQuantizer();
    SemiSet s = SemiSet();
    q.add_quantum(C);
    q.add_quantum(E);
    q.add_quantum(G);

    s.add_semi(C_S);
    s.add_semi(F);
    s.add_semi(G_S);
    q.change_key(C_S);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C_S, q.root);

    s.clear();
    s.add_semi(G);
    s.add_semi(B);
    s.add_semi(D);
    q.change_key(G);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(G, q.root);

    s.clear();
    s.add_semi(D);
    s.add_semi(F_S);
    s.add_semi(A);
    q.change_key(D);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(D, q.root);

    q.add_quantum(C_S);

    s.clear();
    s.add_semi(B);
    s.add_semi(C);
    s.add_semi(E);
    s.add_semi(G);
    q.change_key(C);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);
}

TEST(OctaveQuantizerTest, OctaveQuantizer_set_map) {
    OctaveQuantizer q = OctaveQuantizer();
    SemiSet s = SemiSet();
    for (int8_t i = 0; i < 12; i++)
        s.add_semi(i);

    q.set_map(CHROMATIC);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);

    q.empty_quanta();
    s.remove_semi(C_S);
    s.remove_semi(D_S);
    s.remove_semi(F_S);
    s.remove_semi(G_S);
    s.remove_semi(A_S);
    q.set_map(MAJOR);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);

    q.empty_quanta();
    s.clear();
    s.add_semi(C);
    s.add_semi(D);
    s.add_semi(D_S);
    s.add_semi(F);
    s.add_semi(G);
    s.add_semi(G_S);
    s.add_semi(A_S);
    q.set_map(MINOR);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);

    q.empty_quanta();
    s.clear();
    s.add_semi(C);
    s.add_semi(D);
    s.add_semi(E);
    s.add_semi(G);
    s.add_semi(A);
    q.set_map(PENT_MAJ);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);

    q.empty_quanta();
    s.clear();
    s.add_semi(C);
    s.add_semi(D_S);
    s.add_semi(F);
    s.add_semi(G);
    s.add_semi(A_S);
    q.set_map(PENT_MIN);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);

    q.empty_quanta();
    s.clear();
    s.add_semi(C);
    s.add_semi(E);
    s.add_semi(G);
    q.set_map(ARP_MAJ);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);

    q.empty_quanta();
    s.clear();
    s.add_semi(C);
    s.add_semi(D_S);
    s.add_semi(G);
    q.set_map(ARP_MIN);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);

    q.empty_quanta();
    s.clear();
    s.add_semi(C);
    s.add_semi(G);
    q.set_map(OCT_FIFTH);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);

    q.empty_quanta();
    s.clear();
    s.add_semi(C);
    q.set_map(OCTAVE);
    EXPECT_TRUE(s.quantizer_semis_equal(&q));
    EXPECT_EQ(C, q.root);
}
