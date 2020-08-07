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
    std::vector<unsigned char> ret;
    ret.reserve(4);
    for (size_t i = 0; i < sizeof(num); i++)
    {
        ret.push_back(num & 0xff);
        num = num >> CHAR_BIT;
    }
    return ret;
}
