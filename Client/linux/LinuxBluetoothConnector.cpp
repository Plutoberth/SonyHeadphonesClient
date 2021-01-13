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


// https://stackoverflow.com/questions/41130180/how-to-connect-from-linux-to-android-bluetooth-socket
uint8_t
getServiceChannel(
    uint8_t *uuid) // uuid of service as 16 bytes
{
  sdp_session_t *s;
  uuid_t svc_uuid;
  sdp_list_t *response_list, *search_list, *attrid_list, *r;
  int range;
  int n;
  uint8_t addr[6]; // define my own addr type
  uint8_t chan = 0;

  // CONNECT TO SDP SERVER
  // (Note: device must be ON but server need not be running)
  str2ba(DEV_ADDR, (bdaddr_t *)&addr);
  const bdaddr_t bdaddr_any = {{0, 0, 0, 0, 0, 0}};
  s = sdp_connect(&bdaddr_any, (bdaddr_t *)&addr, SDP_RETRY_IF_BUSY);
  if (!s)
  {
    fprintf(stderr, "can't connect to sdp server\n");
    return (0);
  }

  // CREATE QUERY LISTS
  sdp_uuid128_create(&svc_uuid, uuid);
  search_list = sdp_list_append(NULL, &svc_uuid);

  range = 0x0000ffff; // start at 0000, end at ffff
  attrid_list = sdp_list_append(NULL, &range);

  // SEARCH FOR RECORDS
  // (Note: Server must be running)
  n = sdp_service_search_attr_req(
      s, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &response_list);
  if (n)
  {
    fprintf(stderr, "search failed.\n");
    return (0);
    ;
  }

  // CHECK IF ANY RESPONSES
  n = sdp_list_len(response_list);
  if (n <= 0)
  {
    fprintf(stderr, "no responses.\n");
    return (0);
    ;
  }

  // PROCESS RESPONSES
  r = response_list;
  while (r)
  { // loop thru all responses
    sdp_record_t *rec;
    sdp_list_t *proto_list, *p;
    rec = (sdp_record_t *)r->data;
    n = sdp_get_access_protos(rec, &proto_list);
    if (n)
    {
      fprintf(stderr, "can't get access protocols.\n");
      return (0);
    }
    p = proto_list;
    while (p)
    { // loop thru all protocols
      sdp_list_t *pds;
      int proto = 0;
      pds = (sdp_list_t *)p->data;
      while (pds)
      { // loop thru all pds
        sdp_data_t *d;
        int dtd;
        d = (sdp_data_t *)pds->data; // get data ptr of pds
        while (d)
        {               // loop over all data
          dtd = d->dtd; // get dtd of data
          switch (dtd)
          { // which dtd?
          case SDP_UUID16:
          case SDP_UUID32:
          case SDP_UUID128:
            proto = sdp_uuid_to_proto(&d->val.uuid); // get proto #
            break;
          case SDP_UINT8:
            if (proto == RFCOMM_UUID)
            {                      // proto is rfcomm?
              chan = d->val.uint8; // save chan num
            }
            break;
          }
          d = d->next; // advance to next data unit
        }

        pds = pds->next; // advance to next pds
      }
      sdp_list_free((sdp_list_t *)p->data, 0);

      p = p->next; // advance to next protocol
    }
    sdp_list_free(proto_list, 0);

    r = r->next; // advance to next response
  }

  return chan;
  // Return chan number [1-30] or 0 if not found
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
  uint8_t channel = getServiceChannel(SERVICE_UUID_IN_BYTES);
  printf("channel: %d\n", channel);
  addr.rc_channel = channel;
  str2ba(dest, &addr.rc_bdaddr);

  // connect to server
  status = ::connect(this->_socket, (struct sockaddr *)&addr, sizeof(addr));


  if (status < 0){
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
