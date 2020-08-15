#pragma once
#include "Constants.h"
#include "ByteMagic.h"
#include <cstddef>
#include <vector>
#include <stdexcept>



namespace CommandSerializer
{
	struct Message
	{
		DATA_TYPE dataType;
		unsigned char seqNumber;
		//Not really needed for now
		//Buffer messageBytes;
	};

	//escape special chars

	Buffer _escapeSpecials(const Buffer& src);
	Buffer _unescapeSpecials(const Buffer& src);
	unsigned char _sumChecksum(const char* src, size_t size);
	unsigned char _sumChecksum(const Buffer& src);
	//Package a serialized command according to the protocol
	/*
	References:
	* DataType
	* CommandBluetoothSender.sendCommandWithRetries
	* BluetoothSenderWrapper.sendCommandViaBluetooth
	* 
	* Serialized data format: <START_MARKER>ESCAPE_SPECIALS(<DATA_TYPE><SEQ_NUMBER><BIG ENDIAN 4 BYTE SIZE OF UNESCAPED DATA><DATA><1 BYTE CHECKSUM>)<END_MARKER>
	*/
	Buffer packageDataForBt(const Buffer& src, DATA_TYPE dataType, unsigned int seqNumber);

	Message unpackBtMessage(const Buffer& src);
	//Probably set seqNumber to zero
	Buffer serializeNcAndAsmSetting(NC_ASM_EFFECT ncAsmEffect, NC_ASM_SETTING_TYPE ncAsmSettingType, unsigned int unk, ASM_SETTING_TYPE asmSettingType, ASM_ID asmId, unsigned char asmLevel);
}

