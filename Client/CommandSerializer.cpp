#include "CommandSerializer.h"

namespace CommandSerializer
{
    Buffer escapeSpecials(const Buffer& src)
    {
        Buffer ret;
        ret.reserve(src.size());

        for (auto&& b : src)
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

    unsigned char sumChecksum(const Buffer& src)
    {
        unsigned char accumulator = 0;
        for (auto&& b : src)
        {
            accumulator += b;
        }
        return accumulator;
    }

    Buffer packageDataForBt(const Buffer& src, DATA_TYPE dataType, unsigned int unk)
    {
        //Reserve at least the size for the size, start&end markers, and the source
        Buffer toEscape;
        toEscape.reserve(src.size() + 2 + sizeof(int));
        Buffer ret;        
        ret.reserve(toEscape.capacity());
        toEscape.push_back(static_cast<unsigned char>(dataType));
        toEscape.push_back(unk);
        auto retSize = intToBytesBE(src.size());
        //Insert data size
        toEscape.insert(toEscape.end(), retSize.begin(), retSize.end());
        //Insert command data
        toEscape.insert(toEscape.end(), src.begin(), src.end());

        auto checksum = sumChecksum(toEscape);
        toEscape.push_back(checksum);
        toEscape = escapeSpecials(toEscape);

        //
        ret.push_back(START_MARKER);
        ret.insert(ret.end(), toEscape.begin(), toEscape.end());
        ret.push_back(END_MARKER);


        // Message will be chunked if it's larger than MAX_BLUETOOTH_MESSAGE_SIZE, just crash to deal with it for now
        if (ret.size() > MAX_BLUETOOTH_MESSAGE_SIZE)
        {
            throw std::runtime_error("Exceeded the max bluetooth message size, and I can't handle chunked messages");
        }

        return toEscape;
    }

    Buffer serializeNcAndAsmSetting(NC_ASM_EFFECT ncAsmEffect, NC_ASM_SETTING_TYPE ncAsmSettingType, unsigned int unk, ASM_SETTING_TYPE asmSettingType, ASM_ID asmId, unsigned char asmLevel)
    {
        Buffer ret;
        ret.push_back(static_cast<unsigned char>(NC_ASM_INQUIRED_TYPE::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE));
        ret.push_back(static_cast<unsigned char>(ncAsmEffect));
        ret.push_back(static_cast<unsigned char>(ncAsmSettingType));
        ret.push_back(static_cast<unsigned char>(unk));
        ret.push_back(static_cast<unsigned char>(asmSettingType));
        ret.push_back(static_cast<unsigned char>(asmId));
        ret.push_back(asmLevel);
        return ret;
    }

}

