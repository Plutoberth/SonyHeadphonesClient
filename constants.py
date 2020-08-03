from enum import Enum

MAX_BLUETOOTH_MESSAGE_SIZE = 2048

SPECIAL_CHARS = {60: (61, 44), 61: (61, 45), 62: (61, 46)}
START_MARKER = int.to_bytes(62, 1, "little")
END_MARKER = int.to_bytes(60, 1, "little")


def ntb(num: int) -> bytes:
    if num > 255 or num < 0:
        raise Exception("num must be between 0 and 255")
    return int.to_bytes(num, 1, "little")


class DATA_TYPE(Enum):
    DATA_MDR = ntb(12)


class NC_ASM_INQUIRED_TYPE(Enum):
    NO_USE = ntb(0)
    NOISE_CANCELLING = ntb(1)
    NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE = ntb(2)
    AMBIENT_SOUND_MODE = ntb(3)


class NC_ASM_EFFECT(Enum):
    OFF = ntb(0)
    ON = ntb(1)
    ADJUSTMENT_IN_PROGRESS = ntb(16)
    ADJUSTMENT_COMPLETION = ntb(17)


class NC_ASM_SETTING_TYPE(Enum):
    ON_OFF = ntb(0)
    LEVEL_ADJUSTMENT = ntb(1)
    DUAL_SINGLE_OFF = ntb(2)


class ASM_SETTING_TYPE(Enum):
    ON_OFF = ntb(0)
    LEVEL_ADJUSTMENT = ntb(1)


class ASM_ID(Enum):
    NORMAL = ntb(0)
    VOICE = ntb(1)

class Command(Enum):
    NCASM_SET_PARAM = ntb(104)