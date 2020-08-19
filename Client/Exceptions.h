#pragma once

#include <stdexcept>
#include <string>

//An exception that should be shown in the UI, and that doesn't make any state invalid.
class RecoverableException : public std::runtime_error {
public:
	//I: what - This string WILL BE SHOWN TO THE USER.
	RecoverableException(const std::string& what, bool shouldDisconnect = false) : std::runtime_error(what), shouldDisconnect(shouldDisconnect) {}
	bool shouldDisconnect = false;
};