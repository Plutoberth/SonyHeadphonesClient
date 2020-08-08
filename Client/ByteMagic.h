#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include "Constants.h"

unsigned int byteOrderSwap(unsigned int num);

//Convert an int to its big endian bytes
std::vector<unsigned char> intToBytesBE(unsigned int num);

long long MACStringToLong(const std::string& str);
std::string MACBytesToString(unsigned char addr[]);
