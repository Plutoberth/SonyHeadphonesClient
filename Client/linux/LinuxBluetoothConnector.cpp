#include "LinuxBluetoothConnector.h"

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#define DEV_ADDR "38:18:4C:DA:A0:C4" // Hardcoded MAC address of service device

uint8_t getServiceChannel(const char *dev_addr, uint8_t *uuid128)
{
  int status;
  bdaddr_t target;
  uuid_t svc_uuid;
  sdp_list_t *response_list, *search_list, *attrid_list;
  sdp_session_t *session = 0;
  uint32_t range = 0x0000ffff;
  uint8_t port = 0;

  str2ba(dev_addr, &target);

  /* connect to the SDP server running on the remote machine */
  const bdaddr_t bdaddr_any = {{0, 0, 0, 0, 0, 0}};
  session = sdp_connect(&bdaddr_any, &target, SDP_RETRY_IF_BUSY);

  sdp_uuid128_create(&svc_uuid, uuid128);
  search_list = sdp_list_append(0, &svc_uuid);
  attrid_list = sdp_list_append(0, &range);

  // get a list of service records that have UUID 0xabcd
  response_list = NULL;
  status = sdp_service_search_attr_req(session, search_list,
                                       SDP_ATTR_REQ_RANGE, attrid_list, &response_list);

  if (status == 0)
  {
    sdp_list_t *proto_list = NULL;
    sdp_list_t *r = response_list;

    // go through each of the service records
    for (; r; r = r->next)
    {
      sdp_record_t *rec = (sdp_record_t *)r->data;

      // get a list of the protocol sequences
      if (sdp_get_access_protos(rec, &proto_list) == 0)
      {
        // get the RFCOMM port number
        port = sdp_get_proto_port(proto_list, RFCOMM_UUID);

        sdp_list_free(proto_list, 0);
      }
      sdp_record_free(rec);
    }
  }
  sdp_list_free(response_list, 0);
  sdp_list_free(search_list, 0);
  sdp_list_free(attrid_list, 0);
  sdp_close(session);

  if (port != 0)
  {
    printf("found service running on RFCOMM port %d\n", port);
  }

  return port;
}

LinuxBluetoothConnector::LinuxBluetoothConnector()
{
}
LinuxBluetoothConnector::~LinuxBluetoothConnector()
{
  //onclose event
  if (isConnected())
  {
    disconnect();
  }
}

int LinuxBluetoothConnector::recv(char *buf, size_t length)
{
  size_t read = ::read(this->_socket, buf, length);
  // printf("length: %ld, read: %ld\n", length, read);
  // for (int i = 0; i < read; i++)
  // {
  //   std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)buf[i]) << " ";
  // }
  // std::cout << '\n';
  return read;
}

int LinuxBluetoothConnector::send(char *buf, size_t length)
{

  size_t written = ::write(this->_socket, buf, length);
  // printf("length: %ld, written: %ld\n", length, written);
  // for (int i = 0; i < written; i++)
  // {
  //   std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)buf[i]) << " ";
  // }
  // std::cout << '\n';
  return written;
}

void LinuxBluetoothConnector::connect(const std::string &addrStr)
{

  printf("connecting to %s\n", addrStr.c_str());
  struct sockaddr_rc addr = {0};
  int status;
  const char *dest = addrStr.c_str();

  // allocate a socket
  this->_socket = ::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  uint32_t linkmode = RFCOMM_LM_AUTH | RFCOMM_LM_ENCRYPT;
  int sl = sizeof(linkmode);
  int res = setsockopt(this->_socket, SOL_RFCOMM, RFCOMM_LM, &linkmode, sl);

  // set the connection parameters (who to connect to)
  addr.rc_family = AF_BLUETOOTH;
  uint8_t channel = getServiceChannel(DEV_ADDR, SERVICE_UUID_IN_BYTES);
  printf("channel: %d\n", channel);
  addr.rc_channel = channel;
  str2ba(dest, &addr.rc_bdaddr);

  // connect to server
  status = ::connect(this->_socket, (struct sockaddr *)&addr, sizeof(addr));

  if (status < 0)
  {
    // TODO: add error/exception
    return;
  }
  this->_connected = true;
}

std::vector<BluetoothDevice> LinuxBluetoothConnector::getConnectedDevices()
{

  // hardcoded for now - need to use dbus and getting it to work is a PIA
  BluetoothDevice device = {.name = "Sony", .mac = DEV_ADDR};
  std::vector<BluetoothDevice> res;
  res.push_back(device);
  return res;
}

void LinuxBluetoothConnector::disconnect() noexcept
{
  // close connection
  if (this->_socket != -1)
  {
    ::close(this->_socket);
  }
  this->_connected = false;
  printf("closed\n");
}

bool LinuxBluetoothConnector::isConnected() noexcept
{
  return this->_connected;
}
