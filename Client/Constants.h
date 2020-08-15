#pragma once

#include <vector>

inline constexpr auto MAX_BLUETOOTH_MESSAGE_SIZE = 2048;
inline constexpr char START_MARKER{ 62 };
inline constexpr char END_MARKER{ 60 };

inline constexpr auto MAC_ADDR_STR_SIZE = 17;

inline constexpr auto SERVICE_UUID = "96CC203E-5068-46ad-B32D-E316F5E069BA";

inline constexpr auto APP_NAME_W = L"Sony Headphones App";

using Buffer = std::vector<char>;

enum class DATA_TYPE : char
{
	ACK = 1,
	DATA_MDR = 12
};

enum class NC_ASM_INQUIRED_TYPE : char
{
	NO_USE = 0,
	NOISE_CANCELLING = 1,
	NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE = 2,
	AMBIENT_SOUND_MODE = 3
};

enum class NC_ASM_EFFECT : char
{
	OFF = 0,
	ON = 1,
	ADJUSTMENT_IN_PROGRESS = 16,
	ADJUSTMENT_COMPLETION = 17
};

enum class NC_ASM_SETTING_TYPE : char
{
	ON_OFF = 0,
	LEVEL_ADJUSTMENT = 1,
	DUAL_SINGLE_OFF = 2
};

enum class ASM_SETTING_TYPE : char
{
	ON_OFF = 0,
	LEVEL_ADJUSTMENT = 1
};

enum class ASM_ID : char
{
	NORMAL = 0,
	VOICE = 1
};

enum class COMMAND_TYPE : char
{
	NCASM_SET_PARAM = 104
};