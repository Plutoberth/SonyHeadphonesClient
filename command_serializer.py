from constants import *


def escape_specials(data: bytes) -> bytes:
    arr = bytearray()
    for byte in data:
        # byte if not default
        arr.extend(SPECIAL_CHARS.get(byte, (byte)))
    return bytes(arr)


def sum_checksum(data: bytes) -> bytes:
    return int.to_bytes(sum(data) & 255, 1, "little")


def package_data_for_bt(data_type: int, command_data: bytes, unk: int) -> bytes:
    """

    # 
    """
    data_type_byte = ntb(data_type)
    unk_byte = ntb(unk)

    data_size_big_endian = int.to_bytes(len(command_data), 4, "big")
    data_to_check = data_type_byte + unk_byte + data_size_big_endian + command_data
    checksum = sum_checksum(data_to_check)
    data_to_escape = data_to_check + checksum
    final_command = START_MARKER + escape_specials(data_to_escape) + END_MARKER

    # Message will be chunked if it's larger than MAX_BLUETOOTH_MESSAGE_SIZE, just crash to deal with it for now
    if len(final_command) > MAX_BLUETOOTH_MESSAGE_SIZE:
        raise Exception(
            f"Exceeded {MAX_BLUETOOTH_MESSAGE_SIZE}, can't handle chunked messages"
        )

    return final_command


class NcAndAsmSetting:
    def __init__(
        self,
        nc_asm_effect: NC_ASM_EFFECT,
        nc_asm_setting_type: NC_ASM_SETTING_TYPE,
        unk_nc_value,
        asm_setting_type: ASM_SETTING_TYPE,
        asm_id: ASM_ID,
        asm_level: int,
    ):
        """
        Fixed values within AscSender:

        unk_nc_value: Set to 0
        """
        self.nc_asm_effect = nc_asm_effect
        self.nc_asm_setting_type = nc_asm_setting_type
        self.unk_nc_value = unk_nc_value
        self.asm_setting_type = asm_setting_type
        self.asm_id = asm_id
        self.asm_level = ntb(asm_level)

    def to_bytes(self) -> bytes:
        return (
            NC_ASM_INQUIRED_TYPE.NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE.value
            + self.nc_asm_effect.value
            + self.nc_asm_setting_type.value
            + self.unk_nc_value
            + self.asm_setting_type.value
            + self.asm_id.value
            + self.asm_level
        )

    def to_command_bytes(self) -> bytes:
        return Command.NCASM_SET_PARAM.value + self.to_bytes()