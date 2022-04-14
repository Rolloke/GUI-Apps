#pragma once
struct DataBit
{
    unsigned char bit1              : 1; /// pin 2
    unsigned char bit2              : 1; /// pin 3
    unsigned char bit3              : 1; /// pin 4
    unsigned char bit4              : 1; /// pin 5
    unsigned char bit5              : 1; /// pin 6
    unsigned char bit6              : 1; /// pin 7
    unsigned char bit7              : 1; /// pin 8
    unsigned char bit8              : 1; /// pin 9
};

struct StatusBit
{
    unsigned char notused1          : 1; 
    unsigned char notused2          : 1;
    unsigned char notused3          : 1;
    unsigned char error             : 1; /// pin 15
    unsigned char select            : 1; /// pin 13
    unsigned char paper_end         : 1; /// pin 12
    unsigned char acknowledge       : 1; /// pin 10
    unsigned char busy              : 1; /// pin 11
};

struct ControlBit
{
    unsigned char data_strobe       : 1; /// pin 1
    unsigned char line_feed         : 1; /// pin 14
    unsigned char init_printer      : 1; /// pin 16
    unsigned char select_input      : 1; /// pin 17
    unsigned char enable_interrupt  : 1;
    unsigned char notused1          : 1;
    unsigned char notused2          : 1;
    unsigned char notused3          : 1;
};

union Bits
{
    unsigned char   bits; 
    DataBit         d;
    ControlBit      c;
    StatusBit       s;
};

class LPTPort
{
public:
    enum ePort {data, control, status};
    LPTPort();
    ~LPTPort();
    // getter / setter
    BOOL setPortBaseAddress(unsigned short aAdr);
    Bits getBits(ePort aPort);
    void setBits(ePort aPort, Bits aB);

private:
    unsigned short mPortBaseAddress;
};
