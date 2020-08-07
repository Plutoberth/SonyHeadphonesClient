#include "WindowsBluetoothConnector.h"

void WSAStartupWrapper()
{
    int iResult;
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup failed: " + std::to_string(iResult));
    }
}

WindowsBluetoothConnector::WindowsBluetoothConnector(BTH_ADDR addr)
{
	static bool startedUp = false;
	if (!startedUp)
	{
        ::WSAStartupWrapper();
        startedUp = true;
	}
    SOCKET sock = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (sock == INVALID_SOCKET)
    {
        throw std::runtime_error("Couldn't create socket: " + std::to_string(WSAGetLastError()));
    }
    this->_socket = sock;

    SOCKADDR_BTH sab = { 0 };
    sab.addressFamily = AF_BTH;
    RPC_STATUS errCode = ::UuidFromStringA((RPC_CSTR)XM3_UUID, &sab.serviceClassId);
    if (errCode != RPC_S_OK)
    {
        throw std::runtime_error("Couldn't create GUID: " + std::to_string(errCode));
    }
    sab.btAddr = addr;

    if (::connect(this->_socket, (sockaddr*)&sab, sizeof(sab)))
    {
        throw std::runtime_error("Couldn't connect: " + std::to_string(WSAGetLastError()));
    }
}

WindowsBluetoothConnector::~WindowsBluetoothConnector()
{
    ::closesocket(this->_socket);
}

int WindowsBluetoothConnector::send(char* buf, size_t length)
{
    auto bytesSent = ::send(this->_socket, buf, length, 0);
    if (bytesSent == SOCKET_ERROR)
    {
        throw std::runtime_error("Couldn't send: " + std::to_string(WSAGetLastError()));
    }
    return bytesSent;
}
