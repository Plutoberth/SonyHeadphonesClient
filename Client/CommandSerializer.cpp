#include "CommandSerializer.h"

/* 
 * Because 
 * 	0x3E represents beginning of packet
 * 	0x3C represents end of packet
 * we need to escape these in the packet payload
*/
constexpr unsigned char ESCAPED_BYTE_SENTRY = 61;   // 0x3D
constexpr unsigned char ESCAPED_60 = 44;	    // 0x2C
constexpr unsigned char ESCAPED_61 = 45;	    // 0x2D
constexpr unsigned char ESCAPED_62 = 46;   	    // 0x2E


constexpr int MAX_STEPS_WH_1000_XM3 = 19;

namespace CommandSerializer
{
	Buffer _escapeSpecials(const Buffer& src)
	{
		Buffer ret;
		ret.reserve(src.size());

		for (auto&& b : src)
		{
			switch (b)
			{
			case 60:
				ret.push_back(ESCAPED_BYTE_SENTRY);
				ret.push_back(ESCAPED_60);
				break;

			case 61:
				ret.push_back(ESCAPED_BYTE_SENTRY);
				ret.push_back(ESCAPED_61);
				break;

			case 62:
				ret.push_back(ESCAPED_BYTE_SENTRY);
				ret.push_back(ESCAPED_62);
				break;

			default:
				ret.push_back(b);
				break;
			}
		}

		return ret;
	}

	Buffer _unescapeSpecials(const Buffer& src)
	{
		Buffer ret;
		ret.reserve(src.size());

		for (size_t i = 0; i < src.size(); i++)
		{
			auto currByte = src[i];
			if (currByte == ESCAPED_BYTE_SENTRY)
			{
				if (i == src.size() - 1)
				{
					throw std::runtime_error("No data left for escaped byte data");
				}
				i = i + 1;
				switch (src[i])
				{
				case ESCAPED_60:
					ret.push_back(60);
					break;

				case ESCAPED_61:
					ret.push_back(61);
					break;

				case ESCAPED_62:
					ret.push_back(62);
					break;

				default:
					throw std::runtime_error("Unexpected escaped byte");
					break;
				}
			}
			else
			{
				ret.push_back(currByte);
			}
		}

		return ret;
	}

	unsigned char _sumChecksum(const char* src, size_t size)
	{
		unsigned char accumulator = 0;
		for (size_t i = 0; i < size; i++)
		{
			accumulator += src[i];
		}
		return accumulator;
	}

	unsigned char _sumChecksum(const Buffer& src)
	{
		return _sumChecksum(src.data(), src.size());
	}

	Buffer packageDataForBt(const Buffer& src, DATA_TYPE dataType, unsigned int seqNumber)
	{
		//Reserve at least the size for the size, start&end markers, and the source
		Buffer toEscape;
		toEscape.reserve(src.size() + 2 + sizeof(int));
		Buffer ret;
		ret.reserve(toEscape.capacity());
		toEscape.push_back(static_cast<unsigned char>(dataType));
		toEscape.push_back(seqNumber);
		auto retSize = intToBytesBE(static_cast<unsigned int>(src.size()));
		//Insert data size
		toEscape.insert(toEscape.end(), retSize.begin(), retSize.end());
		//Insert command data
		toEscape.insert(toEscape.end(), src.begin(), src.end());

		auto checksum = _sumChecksum(toEscape);
		toEscape.push_back(checksum);
		toEscape = _escapeSpecials(toEscape);

		
		ret.push_back(START_MARKER);
		ret.insert(ret.end(), toEscape.begin(), toEscape.end());
		ret.push_back(END_MARKER);


		// Message will be chunked if it's larger than MAX_BLUETOOTH_MESSAGE_SIZE, just crash to deal with it for now
		if (ret.size() > MAX_BLUETOOTH_MESSAGE_SIZE)
		{
			throw std::runtime_error("Exceeded the max bluetooth message size, and I can't handle chunked messages");
		}

		return ret;
	}

	BtMessage unpackBtMessage(const Buffer& src)
	{
		//Message data format: ESCAPE_SPECIALS(<DATA_TYPE><SEQ_NUMBER><BIG ENDIAN 4 BYTE SIZE OF UNESCAPED DATA><DATA><1 BYTE CHECKSUM>)
		auto unescaped = _unescapeSpecials(src);

		if (unescaped.size() < 7)
		{
			throw std::runtime_error("Invalid message: Smaller than the minimum message size");
		}

		BtMessage ret;
		ret.dataType = static_cast<DATA_TYPE>(unescaped[0]);
		ret.seqNumber = unescaped[1];
		if ((unsigned char)unescaped[unescaped.size() - 1] != _sumChecksum(unescaped.data(), unescaped.size() - 1))
		{
			throw RecoverableException("Invalid checksum!", true);
		}
		unsigned char numMsgBytes = static_cast<unsigned char>(unescaped[5]);
		ret.messageBytes.insert(ret.messageBytes.end(), unescaped.begin() + 6, unescaped.begin() + 6 + numMsgBytes); 
		return ret;
	}

	NC_DUAL_SINGLE_VALUE getDualSingleForAsmLevel(char asmLevel)
	{
		NC_DUAL_SINGLE_VALUE val = NC_DUAL_SINGLE_VALUE::OFF;
		if (asmLevel > MAX_STEPS_WH_1000_XM3)
		{
			throw std::runtime_error("Exceeded max steps");
		}
		else if (asmLevel == 1)
		{
			val = NC_DUAL_SINGLE_VALUE::SINGLE;
		}
		else if (asmLevel == 0)
		{
			val = NC_DUAL_SINGLE_VALUE::DUAL;
		}
		return val;
	}

	Buffer serializeXM4OptimizeCommand(OPTIMIZER_STATE state)
	{
	    Buffer ret;
	    ret.push_back(static_cast<unsigned char>(COMMAND_TYPE::XM4_OPTIMIZER_PARAM));
	    ret.push_back(static_cast<unsigned char>(0x01));
	    ret.push_back(static_cast<unsigned char>(0x00));
	    ret.push_back(static_cast<unsigned char>(state));
	    return ret;
	}

	Buffer serializeNcAndAsmSetting(NC_ASM_EFFECT ncAsmEffect, NC_ASM_SETTING_TYPE ncAsmSettingType, ASM_SETTING_TYPE asmSettingType, ASM_ID asmId, char asmLevel)
	{
		Buffer ret;
		ret.push_back(static_cast<unsigned char>(COMMAND_TYPE::NCASM_SET_PARAM));
		ret.push_back(static_cast<unsigned char>(NC_ASM_INQUIRED_TYPE::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE));
		ret.push_back(static_cast<unsigned char>(ncAsmEffect));
		ret.push_back(static_cast<unsigned char>(ncAsmSettingType));
		ret.push_back(static_cast<unsigned char>(getDualSingleForAsmLevel(asmLevel)));
		ret.push_back(static_cast<unsigned char>(asmSettingType));
		ret.push_back(static_cast<unsigned char>(asmId));
		ret.push_back(asmLevel);
		return ret;
	}

	Buffer serializeVPTSetting(VPT_INQUIRED_TYPE type, unsigned char preset)
	{
		Buffer ret;
		ret.push_back(static_cast<unsigned char>(COMMAND_TYPE::VPT_SET_PARAM));
		ret.push_back(static_cast<unsigned char>(type));
		ret.push_back(preset);

		return ret;
	}

	Buffer serializeXM4SpeakToChat(S2C_TOGGLE s2cState)
	{
		Buffer ret;
		ret.push_back(static_cast<unsigned char>(COMMAND_TYPE::XM4_S2C_TOGGLE_PARAM));
		ret.push_back(static_cast<unsigned char>(0x05));
		ret.push_back(static_cast<unsigned char>(0x01));
		ret.push_back(static_cast<unsigned char>(s2cState));
		return ret;
	}

	Buffer serializeXM4_S2C_Options(unsigned char sensitivity, unsigned char voice, unsigned char offTime)
	{
		Buffer ret;
		ret.push_back(static_cast<unsigned char>(COMMAND_TYPE::XM4_S2C_OPTIONS_PARAM));
		ret.push_back(static_cast<unsigned char>(0x05));
		ret.push_back(static_cast<unsigned char>(0x00));
		ret.push_back(static_cast<unsigned char>(sensitivity));
		ret.push_back(static_cast<unsigned char>(voice));
		ret.push_back(static_cast<unsigned char>(offTime));
		return ret;
	}

	Buffer serializeMultiPointCommand(MULTI_POINT_COMMANDS cmd, std::string macAddr)
	{
		Buffer ret;
		ret.push_back(static_cast<unsigned char>(COMMAND_TYPE::MULTI_POINT_PARAM));
		ret.push_back(static_cast<unsigned char>(0x01));
		ret.push_back(static_cast<unsigned char>(0x00));
		ret.push_back(static_cast<unsigned char>(cmd));
		for (unsigned char c: macAddr)
		{
			ret.push_back(c);
		}
		return ret;
	}
}

