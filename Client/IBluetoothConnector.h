#pragma once
#include <string>
#include <vector>
#include "RecoverableException.h"

inline constexpr auto SONY_UUID = "96CC203E-5068-46ad-B32D-E316F5E069BA";

inline constexpr auto NO_BLUETOOTH_DEVICES_ERROR = "No Bluetooth radios were found - is your adapter connected?";

struct BluetoothDevice
{
	//UTF-8
	std::string name;
	std::string mac;
};

class IBluetoothConnector
{
public:
	IBluetoothConnector() = default;
	virtual ~IBluetoothConnector() = default;

	IBluetoothConnector(const IBluetoothConnector&) = delete;
	IBluetoothConnector& operator=(const IBluetoothConnector&) = delete;

	//O: The number of bytes sent.
	virtual int send(char* buf, size_t length) noexcept(false) = 0;
	virtual void connect(const std::string& addrStr) noexcept(false) = 0;
	virtual void disconnect() noexcept(false) = 0;

	virtual std::vector<BluetoothDevice> getConnectedDevices() = 0;
};
