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

WindowsBluetoothConnector::WindowsBluetoothConnector()
{
	static bool startedUp = false;
	if (!startedUp)
	{
        ::WSAStartupWrapper();
        startedUp = true;
	}
    WindowsBluetoothConnector::_initSocket();
}

void WindowsBluetoothConnector::connect(const std::string& addrStr)
{
    SOCKADDR_BTH sab = { 0 };
    sab.addressFamily = AF_BTH;
    RPC_STATUS errCode = ::UuidFromStringA((RPC_CSTR)XM3_UUID, &sab.serviceClassId);
    if (errCode != RPC_S_OK)
    {
        throw std::runtime_error("Couldn't create GUID: " + std::to_string(errCode));
    }
    sab.btAddr = MACStringToLong(addrStr);

    if (::connect(this->_socket, (sockaddr*)&sab, sizeof(sab)))
    {
        throw std::runtime_error("Couldn't connect: " + std::to_string(WSAGetLastError()));
    }
}

WindowsBluetoothConnector::~WindowsBluetoothConnector()
{
    if (this->_socket != INVALID_SOCKET)
    {
        ::closesocket(this->_socket);
    }
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

void WindowsBluetoothConnector::disconnect() noexcept(false)
{
    if (this->_socket != INVALID_SOCKET)
    {
        
        if (::shutdown(this->_socket, SD_BOTH))
        {
            throw std::runtime_error("Couldn't shutdown connection: " + std::to_string(WSAGetLastError()));
        }

        if (::closesocket(this->_socket))
        {
            throw std::runtime_error("Couldn't disconnect: " + std::to_string(WSAGetLastError()));
        }
        
        this->_socket = INVALID_SOCKET;
        WindowsBluetoothConnector::_initSocket();
    }
    else
    {
        throw std::runtime_error("The socket was already closed, or it was never open");
    }
}

void WindowsBluetoothConnector::_initSocket()
{
    SOCKET sock = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (sock == INVALID_SOCKET)
    {
        throw std::runtime_error("Couldn't create socket: " + std::to_string(WSAGetLastError()));
    }

    ULONG enable = TRUE;
    if (::setsockopt(sock, SOL_RFCOMM, SO_BTH_AUTHENTICATE, reinterpret_cast<char*>(&enable), sizeof(enable)))
    {
        throw std::runtime_error("Couldn't set SO_BTH_AUTHENTICATE: " + std::to_string(WSAGetLastError()));
    }

    if (::setsockopt(sock, SOL_RFCOMM, SO_BTH_ENCRYPT, reinterpret_cast<char*>(&enable), sizeof(enable)))
    {
        throw std::runtime_error("Couldn't set SO_BTH_ENCRYPT: " + std::to_string(WSAGetLastError()));
    }

    this->_socket = sock;
}
