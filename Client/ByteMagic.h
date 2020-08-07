#pragma once

#include <vector>

unsigned int byteOrderSwap(unsigned int num);

//Convert an int to its big endian bytes
std::vector<unsigned char> intToBytesBE(unsigned int num);