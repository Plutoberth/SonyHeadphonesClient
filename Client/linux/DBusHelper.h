#pragma once

#include <dbus/dbus.h>
#include <vector>
#include <string>
#include <cstdint>

DBusConnection *dbus_open_system_bus();
dbus_bool_t open_dict_read(DBusMessageIter *const iter, DBusMessageIter *const iter_dict);
dbus_bool_t read_next_object_path_entry(DBusMessageIter *const iter_object_paths, char **const object_path, DBusMessageIter *const iter_interfaces);
dbus_bool_t read_next_interface_entry(DBusMessageIter *const interface_dict, char **const interface);
std::vector<std::string> dbus_list_adapters(DBusConnection *const connection);
std::string dbus_get_property(DBusConnection *const connection, const char *device_path, const char *property);

uint8_t sdp_getServiceChannel(const char *dev_addr, uint8_t *uuid128);

