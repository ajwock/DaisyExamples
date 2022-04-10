#include "Quantizer.h"
#include <math.h>

ArbitraryQuantizer::ArbitraryQuantizer(float *quant_buf, size_t buf_cap, size_t filled_quanta) {
    quanta = quant_buf;
    capacity = buf_cap;
    n_quanta = filled_quanta;
    last_in = INFINITY;
    last_out = INFINITY;
}

int ArbitraryQuantizer::add_quantum(float quantum) {
    if (n_quanta + 1 >= capacity)
        return 1;
    quanta[n_quanta] = quantum;
    n_quanta += 1;
    return 1;
}

int ArbitraryQuantizer::add_quanta(float *in_quanta, size_t n) {
    int rc;
    for (size_t i = 0; i < n; i++) {
        rc = add_quantum(in_quanta[i]);
        if (rc)
            return 1;
    }
    return 0;
}

int ArbitraryQuantizer::remove_quantum_at(size_t index) {
    if (index < 0 || index >= n_quanta)
        return 1;
    n_quanta -= 1;
    for (size_t i = index; i < n_quanta; i++)
        quanta[i] = quanta[i + 1];
    return 0;
}

int ArbitraryQuantizer::remove_quantum(float quantum) {
    size_t i;
    bool   found = 0;
    for (i = 0; i < n_quanta; i++)
        if (quanta[i] == quantum) {
            // No break, in case we must remove duplicates.
            remove_quantum_at(i);
            found = 1;
        }
    return found;
}

float ArbitraryQuantizer::quantize(float input) {
    // Memoization for quantizing continually static CV values
    float quantum = input;
    float min_distance = INFINITY;
    if (last_in == input)
        return last_out;

    size_t i;
    for (i = 0; i < n_quanta; i++) {
        float distance = fabs(input - quanta[i]);
        if (distance < min_distance) {
            quantum = quanta[i];
            min_distance = distance;
        }
    }        
    last_in = input;
    last_out = quantum;
    return quantum;
}
