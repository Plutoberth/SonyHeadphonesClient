#pragma once

#include <cstddef>

inline constexpr auto MAX_BLUETOOTH_MESSAGE_SIZE = 2048;
inline constexpr std::byte START_MARKER{ 0x62 }; 
inline constexpr std::byte END_MARKER{ 0x60 };

inline constexpr auto SERVICE_UUID = "96CC203E-5068-46ad-B32D-E316F5E069BA";

enum class DATA_TYPE : unsigned char
{
	DATA_MDR = 12
};

enum class NC_ASM_INQUIRED_TYPE : unsigned char
{
	NO_USE = 0,
	NOISE_CANCELLING = 1,
	NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE = 2,
	AMBIENT_SOUND_MODE = 3
};

enum class NC_ASM_EFFECT : unsigned char
{
	OFF = 0,
	ON = 1,
	ADJUSTMENT_IN_PROGRESS = 16,
	ADJUSTMENT_COMPLETION = 17
};

enum class NC_ASM_SETTING_TYPE : unsigned char
{
	ON_OFF = 0,
	LEVEL_ADJUSTMENT = 1,
	DUAL_SINGLE_OFF = 2
};

enum class ASM_SETTING_TYPE : unsigned char
{
	ON_OFF = 0,
	LEVEL_ADJUSTMENT = 1
};

enum class ASM_ID : unsigned char
{
	NORMAL = 0,
	voice = 1
};

enum class COMMAND_TYPE : unsigned char
{
	NCASM_SET_PARAM = 104
};