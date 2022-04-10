#ifndef SEMI_SET_TEST_OBJ_H
#define SEMI_SET_TEST_OBJ_H

#include <gtest/gtest.h>
#include <set>

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

#endif
