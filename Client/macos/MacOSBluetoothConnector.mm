#include "MacOSBluetoothConnector.h"

// don't know if this is needed
void WSAStartupWrapper()
{
	// int iResult;
	// WSADATA wsaData;
	// iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// if (iResult != 0) {
	// 	throw std::runtime_error("WSAStartup failed: " + std::to_string(iResult));
	// }
}

MacOSBluetoothConnector::MacOSBluetoothConnector()
{
	// static bool startedUp = false;
	// if (!startedUp)
	// {
	// 	::WSAStartupWrapper();
	// 	startedUp = true;
	// }
}

MacOSBluetoothConnector::~MacOSBluetoothConnector()
{
	// if (this->_socket != INVALID_SOCKET)
	// {
	// 	::closesocket(this->_socket);
	// }
}

// doesn't generate error anymore, but doesn't close connection yet, because the channel is not stored globaly 
void MacOSBluetoothConnector::connect(const std::string& addrStr)
{
	// convert mac adress to nsstring
	NSString *addressNSString = [NSString stringWithCString:addrStr.c_str() encoding:[NSString defaultCStringEncoding]];
	// get device based on mac adress
	IOBluetoothDevice *device = [IOBluetoothDevice deviceWithAddressString:addressNSString];
	// create new channel
	IOBluetoothRFCOMMChannel *channel = [[IOBluetoothRFCOMMChannel alloc] init];
	// create sppServiceid
	IOBluetoothSDPUUID *sppServiceUUID = [IOBluetoothSDPUUID uuid16: kBluetoothSDPUUID16RFCOMM];
	// get sppServiceRecord
    IOBluetoothSDPServiceRecord *sppServiceRecord = [device getServiceRecordForUUID:sppServiceUUID];
    // get rfcommChannelID from sppServiceRecord
	UInt8 rfcommChannelID;
    [sppServiceRecord getRFCOMMChannelID:&rfcommChannelID];

	if ([device openRFCOMMChannelSync: &channel withChannelID: rfcommChannelID delegate: nil] == kIOReturnSuccess) {
		const int connectResult = kIOReturnSuccess;
		printf("%d", connectResult);
		this->_connected = true;
	}
}

int MacOSBluetoothConnector::send(char* buf, size_t length)
{
	// auto bytesSent = ::send(this->_socket, buf, length, 0);
	// if (bytesSent == SOCKET_ERROR)
	// {
	// 	throw RecoverableException("Couldn't send (" + std::to_string(WSAGetLastError()) + ")", true);
	// }
	// return bytesSent;
}

int MacOSBluetoothConnector::recv(char* buf, size_t length)
{
	// auto bytesReceived = ::recv(this->_socket, buf, length, 0);
	// if (bytesReceived == SOCKET_ERROR)
	// {
	// 	throw RecoverableException("Couldn't recv (" + std::to_string(WSAGetLastError()) + ")", true);
	// }
	// return bytesReceived;
}

// currently working
std::vector<BluetoothDevice> MacOSBluetoothConnector::getConnectedDevices()
{
	// create the output vector
	std::vector<BluetoothDevice> res;
	// loop through the paired devices (also includes non paired devices for some reason)
	for (IOBluetoothDevice* device in [IOBluetoothDevice pairedDevices]) {
		// check if device is connected
		if ([device isConnected]) {
			BluetoothDevice dev;
			// save the mac address and name
			dev.mac =  [[device addressString]UTF8String];
			dev.name = [[device name] UTF8String];
			// add device to the connected devices vector
			res.push_back(dev);
		}
	}

	return res;
}

void MacOSBluetoothConnector::disconnect() noexcept
{
	// if (this->_socket != INVALID_SOCKET)
	// {
	// 	this->_connected = false;
	// 	shutdown(this->_socket, SD_BOTH);
	// 	closesocket(this->_socket);
	// 	this->_socket = INVALID_SOCKET;
	// }
	// [this->channel closeChannel];
}

bool MacOSBluetoothConnector::isConnected() noexcept
{
	return this->_connected;
}

// probably don't need this function (right?)
void MacOSBluetoothConnector::_initSocket()
{
	// SOCKET sock = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	// if (sock == INVALID_SOCKET)
	// {
	// 	throw std::runtime_error("Couldn't create socket: " + std::to_string(WSAGetLastError()));
	// }

	// ULONG enable = TRUE;
	// if (::setsockopt(sock, SOL_RFCOMM, SO_BTH_AUTHENTICATE, reinterpret_cast<char*>(&enable), sizeof(enable)))
	// {
	// 	throw std::runtime_error("Couldn't set SO_BTH_AUTHENTICATE: " + std::to_string(WSAGetLastError()));
	// }

	// if (::setsockopt(sock, SOL_RFCOMM, SO_BTH_ENCRYPT, reinterpret_cast<char*>(&enable), sizeof(enable)))
	// {
	// 	throw std::runtime_error("Couldn't set SO_BTH_ENCRYPT: " + std::to_string(WSAGetLastError()));
	// }

	// this->_socket = sock;
}

// probably don't need this function (right?)
std::string MacOSBluetoothConnector::_wstringToUtf8(const std::wstring& wstr)
{
	// std::string strTo;
	// const int num_chars = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);

	// if (num_chars > 0)
	// {
	// 	strTo.resize(num_chars);
	// 	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &strTo[0], num_chars, NULL, NULL);
	// }
	// return strTo;
}


// probably don't need this function (right?)
std::vector<BluetoothDevice> MacOSBluetoothConnector::_findDevicesInRadio()
{
	// std::vector<BluetoothDevice> res;

	// for (IOBluetoothDevice* device in [IOBluetoothDevice pairedDevices]) {
	// 	BluetoothDevice dev;
	// 	dev.mac =  [[device addressString]UTF8String]
	// 	dev.name = [[device name] UTF8String]
	// 	printf("%s - ",[[device addressString]UTF8String]);
	// 	printf("%s - ",[[device name] UTF8String]);
	// 	if ([device isConnected]) {
	// 		printf("connected device");
	// 		res.append(dev)
	// 	}else{
	// 		printf("disconnected device");
	// 	}
	// 	printf("\n");
	// }

	// return res;
}
