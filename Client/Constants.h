#pragma once

#include <vector>

inline constexpr auto MAX_BLUETOOTH_MESSAGE_SIZE = 2048;
inline constexpr char START_MARKER{ 62 };
inline constexpr char END_MARKER{ 60 };

inline constexpr auto MAC_ADDR_STR_SIZE = 17;

inline constexpr auto SERVICE_UUID = "96CC203E-5068-46ad-B32D-E316F5E069BA";
inline unsigned char SERVICE_UUID_IN_BYTES[] = { // this is the SERVICE_UUID but in bytes
	0x96, 0xcc, 0x20, 0x3e, 0x50, 0x68, 0x46, 0xad,
	0xb3, 0x2d, 0xe3, 0x16, 0xf5, 0xe0, 0x69, 0xba
};

inline constexpr auto APP_NAME_W = L"Sony Headphones App";

using Buffer = std::vector<char>;

enum class DATA_TYPE : char
{
	DATA = 0,
	ACK = 1,
    DATA_MC_NO1 = 2,
    DATA_ICD = 9,
    DATA_EV = 10,
	DATA_MDR = 12,
    DATA_COMMON = 13,
    DATA_MDR_NO2 = 14,
    SHOT =  16,
    SHOT_MC_NO1 =  18,
    SHOT_ICD =  25,
    SHOT_EV =  26,
    SHOT_MDR =  28,
    SHOT_COMMON =  29,
    SHOT_MDR_NO2 = 30,
    LARGE_DATA_COMMON =  45,
    UNKNOWN = -1
};

enum class EQ_EBB_INQUIRED_TYPE: char
{
	NO_USE = 0,
	PRESET_EQ = 1,
	EBB = 2,
	PRESET_EQ_NONCUSTOMIZABLE = 3,
	OUT_OF_RANGE = -1,
};

enum class EqPresetId: char {
  OFF = 0,
  ROCK = 1,
  POP = 2,
  JAZZ = 3,
  DANCE = 4,
  EDM = 5,
  R_AND_B_HIP_HOP = 6,
  ACOUSTIC = 7,
  /*RESERVED_FOR_FUTURE_NO8 = 8,… */
  /*RESERVED_FOR_FUTURE_NO15 = 15,*/
  BRIGHT = 16,
  EXCITED = 17,
  MELLOW = 18,
  RELAXED = 19,
  VOCAL = 20,
  TREBLE = 21,
  BASS = 22,
  SPEECH = 23,
  /*RESERVED_FOR_FUTURE_NO24 = 24,… */
  /*RESERVED_FOR_FUTURE_NO31 = 31,*/
  CUSTOM = -96,
  USER_SETTING1 = -95,
  USER_SETTING2 = -94,
  USER_SETTING3 = -93,
  USER_SETTING4 = -92,
  USER_SETTING5 = -91,
  UNSPECIFIED = -1,
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

enum class NC_DUAL_SINGLE_VALUE : char
{
	OFF = 0,
	SINGLE = 1,
	DUAL = 2
};

enum class COMMAND_TYPE : char
{
	COMMON_NTFY_UPSCALING_EFFECT = 23,
	EQEBB_SET_PARAM = 88,
	EQEBB_NTFY_PARAM = 89,
	EQEBB_GET_EXTENDED_INFO = 90,
	EQEBB_RET_EXTENDED_INFO = 91,
	NCASM_SET_PARAM = 104
};