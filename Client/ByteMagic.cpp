#include "ByteMagic.h"

unsigned int byteOrderSwap(unsigned int num)
{
    return (num >> 24) |
        ((num << 8) & 0x00FF0000) |
        ((num >> 8) & 0x0000FF00) |
        (num << 24);
}

std::vector<unsigned char> intToBytesBE(unsigned int num)
{
    std::vector<unsigned char> ret(sizeof(num));
    for (size_t i = 0; i < sizeof(num); i++)
    {
        ret[i] = (num >> (CHAR_BIT * (3-i))) & 0xff;
    }
    return ret;
}
