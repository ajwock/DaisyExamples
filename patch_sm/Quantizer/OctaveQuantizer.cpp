#include "OctaveQuantizer.h"
#include <math.h>

#include <cstdio>

const char *semi_str(uint8_t semi) {
    switch (semi) {
    case C:
        return "C";
    case C_S:
        return "C#";
    case D:
        return "D";
    case D_S:
        return "D#";
    case E:
        return "E";
    case F:
        return "F";
    case F_S:
        return "F#";
    case G:
        return "G";
    case G_S:
        return "G#";
    case A:
        return "A";
    case A_S:
        return "A#";
    case B:
        return "B";
    default:
        break;
    }
    return "(Invalid Semitone)";
}
OctaveQuantizer::OctaveQuantizer() {
    empty_quanta();
    root = C;
    last_in = INFINITY;
    last_out = INFINITY;
}

float OctaveQuantizer::quantize(float input) {
    float  output = input;
    int    conv;
    float  left;
    float  right;
    float  dist_l = INFINITY;
    float  dist_r = INFINITY;

    /* Memoization to reduce repeated effort */
    if (input == last_in)
        return last_out;

    if (is_empty())
        return input;

    conv = (int) input;
    for (int16_t r = conv + 1; r <= 127; r++)
        if (is_quantum_midi((int8_t) r)) {
            right = (float) r;
            dist_r = right - input;
            break;
        }

    for (int16_t l = conv; l >= 0; l--)
        if (is_quantum_midi((int8_t) l)) {
            left = (float) l;
            dist_l = input - left;
            break;
        }

    if (dist_r < dist_l)
        output = right;
    else
        output = left;

    last_in = input;
    last_out = output;
    return output;
}

bool OctaveQuantizer::is_quantum(uint8_t semi) {
    return semi_map & (uint16_t) (0x1 << semi);
}

bool OctaveQuantizer::is_quantum_midi(uint8_t midi) {
    int16_t m = midi % 12;
    return is_quantum(m);
}

void OctaveQuantizer::add_quantum(uint8_t semi) {
    semi_map |= (uint16_t) (0x1 << semi);
}

void OctaveQuantizer::remove_quantum(uint8_t semi) {
    semi_map &= (uint16_t) (~(0x1 << semi));
}

void OctaveQuantizer::empty_quanta() {
    semi_map = 0;
}

void OctaveQuantizer::fill_quanta() {
    semi_map |= (uint16_t) 0x0FFF;
}

bool OctaveQuantizer::is_empty() {
    return ! (semi_map & 0x0FFF);
}

char *OctaveQuantizer::quantizer_str(char *buf, size_t bufsize) {
    snprintf(buf, bufsize,
        "Root note: %s\n"
        "| C  | C# | D  | D# | E  | F  | F# | G  | G# | A  | A# | B  |\n"
        "-------------------------------------------------------------\n"
        "|  %d |  %d |  %d |  %d |  %d |  %d |  %d |  %d |  %d |  %d |  %d |  %d |\n",
        semi_str(root),
        is_quantum(C),
        is_quantum(C_S),
        is_quantum(D),
        is_quantum(D_S),
        is_quantum(E),
        is_quantum(F),
        is_quantum(F_S),
        is_quantum(G),
        is_quantum(G_S),
        is_quantum(A),
        is_quantum(A_S),
        is_quantum(B));
    return buf;
}

/*
 * Input must be a semitone from 0-11
 */
void OctaveQuantizer::change_key(uint8_t semi) {
    int16_t delta = ((int16_t) semi) - root;
    int16_t shiftr;
    uint32_t buf;
    uint32_t shifted;
    uint32_t shifted_mask;
    while (delta < 0)
        delta += 12;
    delta %= 12;
    //shiftr = 12 - delta;
    shiftr = delta;

    // Rotate the bitmap.
    buf = 0x0 | (semi_map << shiftr);
    shifted_mask = ((0x1 << (shiftr + 1)) - 1) << 12;
    shifted = (0x0 | (shifted_mask & buf)) >> 12;
    buf |= shifted;
    semi_map = (uint16_t) buf & 0xFFF;
    root = semi;
}

void OctaveQuantizer::set_map(uint16_t map) {
    semi_map = map;
}
