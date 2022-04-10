#ifndef OCTAVE_QUANTIZER_H
#define OCTAVE_QUANTIZER_H

#include "Quantizer.h"
#include <cstdint>
#include <cstring>

typedef enum {
    C = 0,
    C_S = 1,
    D = 2,
    D_S = 3,
    E = 4,
    F = 5,
    F_S = 6,
    G = 7,
    G_S = 8,
    A = 9,
    A_S = 10, 
    B = 11, 
} semitone_indices;

const char *semi_str(uint8_t semi);

// These are bitmaps for each type of scale with root note C.
// A key change is sufficient to convert them to any other key.

#define CHROMATIC           0xFFF
#define MAJOR               0xAB5
#define MINOR               0x5AD
#define PENT_MAJ            0x295
#define PENT_MIN            0x4A9
#define ARP_MAJ             0x091
#define ARP_MIN             0x089
#define OCT_FIFTH           0x081
#define OCTAVE              0X001


/*
 * Quantizes to the nearest semitone present in the 12-bit map
 * of semitones.  It is octave agnostic.  Only accepts midi values
 * 0 - 127.  C3 is 60.
 *
 * The key can be efficiently changed with a semitone offset.
 *
 */
class OctaveQuantizer : public Quantizer {
    public:
    uint16_t semi_map;
    uint16_t root;
    float    last_in;
    float    last_out;
    
    OctaveQuantizer();

    float quantize(float input);

    bool is_quantum(uint8_t semi);
    bool is_quantum_midi(uint8_t semi);
    void add_quantum(uint8_t semi);
    void remove_quantum(uint8_t semi);

    bool is_empty();

    // Sets the quanta bitmap to all 0s.
    void empty_quanta();
    // Sets the quanta bitmap to all 1s.
    void fill_quanta();

    // Prints the bitmap and root note.
    char *quantizer_str(char *buf, size_t bufsize);

    /*
     * Sets the root note without changing the bitmap.
     * This is really somewhat arbitrary and is only used for key changes.
     */
    void set_root(uint8_t root);

    /*
     * Changes the key being quantized to while preserving the intervals between quanta.
     * 
     * Root note is changed accordingly.
     *
     * This is based on the current root note.
     */
    void change_key(uint8_t semi);

    /*
     * Sets the bitmap to the given bitmap.
     */
    void set_map(uint16_t map);
};

#endif
