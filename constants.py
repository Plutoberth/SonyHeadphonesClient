MAX_BLUETOOTH_MESSAGE_SIZE = 2048

SPECIAL_CHARS = {60: (61, 44), 61: (61, 45), 62: (61, 46)}
START_MARKER = int.to_bytes(62, 1, "little")
END_MARKER = int.to_bytes(60, 1, "little")

class DATA_TYPE:
    DATA_MDR = 12