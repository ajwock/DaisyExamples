#ifndef ARBITRARY_QUANTIZER_H
#define ARBITRARY_QUANTIZER_H

#include "Quantizer.h"

/*
 * Quantizes floating point value to the nearest quantum (in linear distance)
 * in the given set of quanta provided.  This quantizer is unspecialized and will simply
 * take in any quanta provided.
 *
 * A linear search is performed, making this quantizer most suitable for small sets of values.
 */
class ArbitraryQuantizer : public Quantizer {
    public:
    float *quanta; // Array of quanta.
    size_t capacity; // Max number of quanta that can be added.  Size of quanta array.
    size_t n_quanta; // Number of quanta currently present.
    float last_in;
    float last_out;

    ArbitraryQuantizer() {}
    ArbitraryQuantizer(float *quant_buf, size_t buf_cap, size_t filled_quanta = 0);

    // Adds the quantum to the internal quanta array.
    // Returns 0 on success or 1 on failure due to going over capacity.
    int add_quantum(float quantum);

    int add_quanta(float *in_quanta, size_t n);

    // ArrayList style removal at the given index.
    // Returns 0 on success or 1 due to going out of bounds.
    int remove_quantum_at(size_t index);

    // Removes the quantum by value.
    // Returns 0 if removed or 1 if the quantum was not found.
    int remove_quantum(float quantum);

    float quantize(float input);
};

#endif
