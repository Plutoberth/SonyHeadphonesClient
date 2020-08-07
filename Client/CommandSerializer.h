#pragma once

#include "Constants.h"
#include "ByteMagic.h"
#include <cstddef>
#include <vector>
#include <stdexcept>

namespace CommandSerializer
{
	using Buffer = std::vector<unsigned char>;

	//escape special chars
	Buffer escapeSpecials(const Buffer& src);
	unsigned char sumChecksum(const Buffer& src);
	//Package a serialized command according to the protocol
	/*
	unk: Seems to be some type of ack value? Based on current understanding (1|0)
    References:
    * DataType
    * CommandBluetoothSender:sendCommandWithRetries
    * BluetoothSenderWrapper.sendCommandViaBluetooth
	*/
	Buffer packageDataForBt(const Buffer& src, DATA_TYPE dataType, unsigned int unk);
}

