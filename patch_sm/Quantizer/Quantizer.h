#ifndef QUANTIZER_H
#define QUANTIZER_H

class Quantizer {
    public:
    virtual float quantize(float input) = 0;
};

#endif
