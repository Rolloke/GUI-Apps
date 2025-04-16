#ifndef PSEUDORANDOMGENERATOR_H
#define PSEUDORANDOMGENERATOR_H

class PseudoRandomGenerator
{
    enum
    {
        generatorfieldsize = 256,
        bitfieldsize = 32
    };
public:
    PseudoRandomGenerator();
    long getRandomValue();

private:
    void init_PSBMG();
    unsigned char getNext_PSBMG();
    void fillGenerator();

    unsigned long mPSBMG;
    unsigned char mGeneratorIndex;
    unsigned long mGeneratorField[generatorfieldsize];

};

#endif // PSEUDORANDOMGENERATOR_H
