#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

inline constexpr auto MAC_ADDR_STR_SIZE = 17;

unsigned int byteOrderSwap(unsigned int num);

//Convert an int to its big endian bytes
std::vector<unsigned char> intToBytesBE(unsigned int num);

long long MACStringToLong(const std::string& str);