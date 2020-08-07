#pragma once
#include <string>

inline constexpr auto XM3_UUID = "96CC203E-5068-46ad-B32D-E316F5E069BA";

class IBluetoothConnector
{
public:
	virtual ~IBluetoothConnector() = default;

	virtual int send(char* buf, size_t length) = 0;
};
