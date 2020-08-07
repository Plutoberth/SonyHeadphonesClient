#include "CommandSerializer.h"

std::vector<unsigned char> CommandSerializer::EscapeSpecials(const Buffer& src)
{
    Buffer ret;
    ret.reserve(src.size());

    for (auto&& b: src)
    {
        switch (b)
        {
        case 60:
            ret.push_back(61);
            ret.push_back(44);
            break;

        case 61:
            ret.push_back(61);
            ret.push_back(45);
            break;

        case 62:
            ret.push_back(61);
            ret.push_back(46);
            break;
    
        default:
            ret.push_back(b);
            break;
        }
    }

    return ret;
}

unsigned char CommandSerializer::sumChecksum(const Buffer& src)
{
    unsigned char accumulator = 0;
    for (auto&& b: src)
    {
        accumulator += b;
    }
    return accumulator;
}
