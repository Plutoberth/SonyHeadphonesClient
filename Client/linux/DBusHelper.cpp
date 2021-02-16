#include "DBusHelper.h"
#include "../Exceptions.h"
#include <cstring>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

DBusConnection *dbus_open_system_bus(void)
{
    DBusError error;
    DBusConnection *connection = NULL;

    dbus_error_init(&error);

    connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);

    if (dbus_error_is_set(&error))
    {
        printf("Could not get system bus\nError: %s\n", error.message);
        dbus_error_free(&error);
        return NULL;
    }

    return connection;
}

dbus_bool_t open_dict_read(DBusMessageIter *const iter, DBusMessageIter *const iter_dict)
{
    if (NULL == iter || NULL == iter_dict)
    {
        return FALSE;
    }

    if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_ARRAY)
    {
        return FALSE;
    }

    if (dbus_message_iter_get_element_type(iter) != DBUS_TYPE_DICT_ENTRY)
    {
        return FALSE;
    }

    dbus_message_iter_recurse(iter, iter_dict);
    return TRUE;
}

dbus_bool_t read_next_object_path_entry(DBusMessageIter *const iter_object_paths, char **const object_path, DBusMessageIter *const iter_interfaces)
{
    DBusMessageIter iter_dict_entry;

    if (!iter_object_paths || !iter_interfaces)
    {
        goto error;
    }

    if (dbus_message_iter_get_arg_type(iter_object_paths) != DBUS_TYPE_DICT_ENTRY)
    {
        goto error;
    }

    dbus_message_iter_recurse(iter_object_paths, &iter_dict_entry);

    if (dbus_message_iter_get_arg_type(&iter_dict_entry) != DBUS_TYPE_OBJECT_PATH)
    {
        printf("Error: Unexpected arg type for object path\n");
        goto error;
    }

    dbus_message_iter_get_basic(&iter_dict_entry, object_path);

    dbus_message_iter_next(&iter_dict_entry);

    if (FALSE == open_dict_read(&iter_dict_entry, iter_interfaces))
    {
        goto error;
    }

    dbus_message_iter_next(iter_object_paths);

    return TRUE;

error:
    return FALSE;
}

dbus_bool_t read_next_interface_entry(DBusMessageIter *const interface_dict, char **const interface)
{
    DBusMessageIter iter_dict_entry;

    if (!interface_dict)
    {
        goto error;
    }

    if (dbus_message_iter_get_arg_type(interface_dict) != DBUS_TYPE_DICT_ENTRY)
    {
        goto error;
    }

    dbus_message_iter_recurse(interface_dict, &iter_dict_entry);

    if (dbus_message_iter_get_arg_type(&iter_dict_entry) != DBUS_TYPE_STRING)
    {
        printf("Error: Unexpected arg type for interface\n");
        goto error;
    }

    dbus_message_iter_get_basic(&iter_dict_entry, interface);

    dbus_message_iter_next(interface_dict);

    return TRUE;

error:
    return FALSE;
}

std::vector<std::string> dbus_list_adapters(DBusConnection *const connection)
{

    std::vector<std::string> ret;

    DBusError error;
    DBusMessage *msg = NULL;
    DBusMessage *rsp = NULL;
    DBusMessageIter args;
    DBusMessageIter subargs;
    DBusMessageIter interface_iter;
    char *object_path = NULL;
    char *interface = NULL;
    char const *const device_interface = "org.bluez.Device1";

    dbus_error_init(&error);

    msg = dbus_message_new_method_call("org.bluez",
                                       "/",
                                       "org.freedesktop.DBus.ObjectManager",
                                       "GetManagedObjects");
    if (NULL == msg)
    {
        printf("Error: Could not obtain method call\n");
        goto clean_up;
    }

    /* No arguments */

    /* Invoke the method */
    rsp = dbus_connection_send_with_reply_and_block(connection,
                                                    msg,
                                                    DBUS_TIMEOUT_USE_DEFAULT,
                                                    &error);
    if (dbus_error_is_set(&error))
    {
        printf("Could not send dbus message\nError: %s\n", error.message);
        dbus_error_free(&error);
        goto clean_up;
    }

    if (NULL == rsp)
    {
        printf("Error: Response was NULL\n");
        goto clean_up;
    }

    if (!dbus_message_iter_init(rsp, &args))
    {
        printf("Error: RCould not start message iterator on response\n");
        goto clean_up;
    }

    if (!open_dict_read(&args, &subargs))
    {
        printf("Error: Could not start dict read\n");
        goto clean_up;
    }

    while (FALSE != read_next_object_path_entry(&subargs, &object_path, &interface_iter))
    {
        /* dbus_bool_t found = FALSE; */
        while (FALSE != read_next_interface_entry(&interface_iter, &interface))
        {
            if (strcmp(device_interface, interface) == 0)
            {
                ret.push_back(object_path);
            }
        }
    }

clean_up:
    if (NULL != rsp)
    {
        dbus_message_unref(rsp);
    }

    if (NULL != msg)
    {
        dbus_message_unref(msg);
    }
    return ret;
}

std::string dbus_get_property(DBusConnection *const connection, const char *device_path, const char *property)
{

    std::string ret;

    DBusError error;
    DBusMessage *msg = NULL;
    DBusMessage *rsp = NULL;
    DBusMessageIter args;
    DBusMessageIter subargs;
    char *interface = NULL;
    char const *const device_interface = "org.bluez.Device1";

    dbus_error_init(&error);

    msg = dbus_message_new_method_call("org.bluez",
                                       device_path,
                                       "org.freedesktop.DBus.Properties",
                                       "Get");
    if (NULL == msg)
    {
        printf("Error: Could not obtain method call\n");
        goto clean_up;
    }

    /* arguments */
    dbus_message_append_args(msg,
                             DBUS_TYPE_STRING, &device_interface,
                             DBUS_TYPE_INVALID);

    dbus_message_append_args(msg,
                             DBUS_TYPE_STRING, &property,
                             DBUS_TYPE_INVALID);

    /* Invoke the method */
    rsp = dbus_connection_send_with_reply_and_block(connection,
                                                    msg,
                                                    DBUS_TIMEOUT_USE_DEFAULT,
                                                    &error);
    if (dbus_error_is_set(&error))
    {
        printf("Could not send dbus message\nError: %s\n", error.message);
        dbus_error_free(&error);
        goto clean_up;
    }

    if (NULL == rsp)
    {
        printf("Error: Response was NULL\n");
        goto clean_up;
    }

    if (!dbus_message_iter_init(rsp, &args))
    {
        printf("Error: Could not start message iterator on response\n");
        goto clean_up;
    }

    dbus_message_iter_recurse(&args, &subargs);

    dbus_message_iter_get_basic(&subargs, &interface);
    ret = std::string(interface);

clean_up:
    if (NULL != rsp)
    {
        dbus_message_unref(rsp);
    }

    if (NULL != msg)
    {
        dbus_message_unref(msg);
    }

    return ret;
}

uint8_t sdp_getServiceChannel(const char *dev_addr, uint8_t *uuid128)
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
  if (!session) {
    throw RecoverableException("Error: could not connect to bluetooth spd server", true);
}

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