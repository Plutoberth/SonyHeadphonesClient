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

long long MACStringToLong(const std::string& str)
{
    if (str.size() != MAC_ADDR_STR_SIZE)
    {
        throw std::runtime_error("Invalid MAC address size (" + str + "): " + std::to_string(str.size()) + " != " + std::to_string(MAC_ADDR_STR_SIZE));
    }

    unsigned char bytes[6];
    int convertedFields = sscanf_s(str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
        bytes + 0, bytes + 1, bytes + 2, bytes + 3, bytes + 4, bytes + 5);

    if (convertedFields != 6)
        throw std::runtime_error("Invalid MAC address format (" + str + ")");
    return
        uint64_t(bytes[0]) << 40 |
        uint64_t(bytes[1]) << 32 |
        uint64_t(bytes[2]) << 24 |
        uint64_t(bytes[3]) << 16 |
        uint64_t(bytes[4]) << 8 |
        uint64_t(bytes[5]);
}

