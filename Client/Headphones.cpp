#include "Headphones.h"
#include "CommandSerializer.h"

#include <stdexcept>

Headphones::Headphones(BluetoothWrapper& conn) : 
_conn(conn)
{
	std::vector<BluetoothDevice> devices({BluetoothDevice({"",""})});
	this->_savedDevices = devices;
}

void Headphones::setAmbientSoundControl(bool val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_ambientSoundControl.desired = val;
}

bool Headphones::getAmbientSoundControl()
{
	return this->_ambientSoundControl.current;
}

bool Headphones::isFocusOnVoiceAvailable()
{
	return this->_ambientSoundControl.current && this->_asmLevel.current > MINIMUM_VOICE_FOCUS_STEP;
}

void Headphones::setFocusOnVoice(bool val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_focusOnVoice.desired = val;
}

bool Headphones::getFocusOnVoice()
{
	return this->_focusOnVoice.current;
}

bool Headphones::isSetAsmLevelAvailable()
{
	return this->_ambientSoundControl.current;
}

void Headphones::setAsmLevel(int val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_asmLevel.desired = val;
}

int Headphones::getAsmLevel()
{
	return this->_asmLevel.current;
}

void Headphones::setOptimizerState(OPTIMIZER_STATE val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_optimizerState.desired = val;
}

OPTIMIZER_STATE Headphones::getOptimizerState()
{
	return this->_optimizerState.current;
}

void Headphones::setS2CToggle(S2C_TOGGLE val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_speakToChat.desired = val;
}

void Headphones::setS2COptions(int sensitivity, bool voice, int offTime)
{
	unsigned int sens = (unsigned int) sensitivity;
	unsigned int time = (unsigned int) offTime;
	std::lock_guard guard(this->_propertyMtx);
	this->_s2cOptions.desired = { (sens << 16) | (voice << 8) | (time) };
}

S2C_TOGGLE Headphones::getS2CToggle()
{
	return this->_speakToChat.current;
}

unsigned int Headphones::getS2COptions()
{
	return this->_s2cOptions.current;
}

void Headphones::setSurroundPosition(SOUND_POSITION_PRESET val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_surroundPosition.desired = val;
}

SOUND_POSITION_PRESET Headphones::getSurroundPosition()
{
	return this->_surroundPosition.current;
}

void Headphones::setVptType(int val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_vptType.desired = val;
}

int Headphones::getVptType()
{
	return this->_vptType.current;
}

bool Headphones::getMultiPointSetting()
{
	return this->_multiPointSetting;
}

const std::vector<BluetoothDevice>& Headphones::getDevices()
{
	return this->_savedDevices;
}

std::pair<int, int> Headphones::getConnectedDevices()
{
	return {this->_dev1.current, this->_dev2.current};
}

void Headphones::setMultiPointConnection(int connectionId, int newDevice, int oldDevice)
{
	Property<int>& dev = this->_dev1;

	if (connectionId==1)
		dev = this->_dev1;
	else
		dev = this->_dev2;

	std::lock_guard guard(this->_propertyMtx);
	dev.desired = newDevice;
}

inline void Headphones::disconnect(int deviceIdx)
{
	std::string deviceMac = this->_savedDevices[deviceIdx].mac;
	this->_conn.sendCommand(CommandSerializer::serializeMultiPointCommand(
			MULTI_POINT_COMMANDS::DISCONNECT,
			deviceMac
		),
		DATA_TYPE::DATA_MDR_NO2
	);
}

inline void Headphones::connect(int deviceIdx)
{
	std::string deviceMac = this->_savedDevices[deviceIdx].mac;
	this->_conn.sendCommand(CommandSerializer::serializeMultiPointCommand(
			MULTI_POINT_COMMANDS::CONNECT,
			deviceMac
		),
		DATA_TYPE::DATA_MDR_NO2
	);
}

bool Headphones::isChanged()
{
	return !(this->_ambientSoundControl.isFulfilled() && 
		this->_asmLevel.isFulfilled() &&
		this->_focusOnVoice.isFulfilled() &&
		this->_surroundPosition.isFulfilled() &&
		this->_vptType.isFulfilled() &&
		this->_optimizerState.isFulfilled() &&
		this->_speakToChat.isFulfilled() &&
		this->_s2cOptions.isFulfilled() && 
		this->_dev1.isFulfilled() &&
		this->_dev2.isFulfilled()
		);
}

// At most one instance of this function is invoked at any time
// Synchronization not required
void Headphones::setChanges()
{
	if (!(this->_optimizerState.isFulfilled()))
	{
		auto state = this->_optimizerState.desired;

		this->_conn.sendCommand(CommandSerializer::serializeXM4OptimizeCommand(
			state
		));

		std::lock_guard guard(this->_propertyMtx);
		this->_optimizerState.fulfill();
	}

	if (!(this->_speakToChat.isFulfilled()))
	{
		auto s2cState = this->_speakToChat.desired;

		this->_conn.sendCommand(CommandSerializer::serializeXM4SpeakToChat(
			s2cState
		));

		std::lock_guard guard(this->_propertyMtx);
		this->_speakToChat.fulfill();
	}

	if (!(this->_s2cOptions.isFulfilled()))
	{
		auto s2cOptions = this->_s2cOptions.desired;
		unsigned char sensitivity = (unsigned char) (s2cOptions >> 16) & 0xff;
		unsigned char voice = (unsigned char) (s2cOptions >> 8) & 0xff;
		unsigned char offTime = (unsigned char) (s2cOptions) & 0xff;

		this->_conn.sendCommand(CommandSerializer::serializeXM4_S2C_Options(
			sensitivity,
			voice,
			offTime
		));

		std::lock_guard guard(this->_propertyMtx);
		this->_s2cOptions.fulfill();
	}

	if (!(this->_ambientSoundControl.isFulfilled() && this->_focusOnVoice.isFulfilled() && this->_asmLevel.isFulfilled()))
	{
		auto ncAsmEffect = this->_ambientSoundControl.desired ? NC_ASM_EFFECT::ADJUSTMENT_COMPLETION : NC_ASM_EFFECT::OFF;
		auto asmId = this->_focusOnVoice.desired ? ASM_ID::VOICE : ASM_ID::NORMAL;
		auto asmLevel = this->_ambientSoundControl.desired ? this->_asmLevel.desired : ASM_LEVEL_DISABLED;

		this->_conn.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
			ncAsmEffect,
			NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
			ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
			asmId,
			asmLevel
		));
		
		std::lock_guard guard(this->_propertyMtx);
		this->_ambientSoundControl.fulfill();
		this->_asmLevel.fulfill();
		this->_focusOnVoice.fulfill();
	}

	if (!(this->_vptType.isFulfilled() && this->_surroundPosition.isFulfilled())) {
		VPT_INQUIRED_TYPE command;
		unsigned char preset;

		if (this->_vptType.desired != 0) {
			command = VPT_INQUIRED_TYPE::VPT;
			preset = static_cast<unsigned char>(this->_vptType.desired);
		}
		else if (this->_surroundPosition.desired != SOUND_POSITION_PRESET::OFF) {
			command = VPT_INQUIRED_TYPE::SOUND_POSITION;
			preset = static_cast<unsigned char>(this->_surroundPosition.desired);
		}
		else {
			// Just used that one because it seems like it disables both
			if (this->_surroundPosition.current != SOUND_POSITION_PRESET::OFF) {
				command = VPT_INQUIRED_TYPE::SOUND_POSITION;
				preset = static_cast<unsigned char>(SOUND_POSITION_PRESET::OFF);
			}
			else if (this->_vptType.current != 0) {
				command = VPT_INQUIRED_TYPE::VPT;
				preset = 0;
			}
			else {
				throw std::logic_error("it's impossible that both values were changed to zero and were also previously zero");
			}
		}

		this->_conn.sendCommand(CommandSerializer::serializeVPTSetting(command, preset));

		std::lock_guard guard(this->_propertyMtx);
		this->_vptType.fulfill();
		this->_surroundPosition.fulfill();
	}
	
	if (!(this->_dev1.isFulfilled()))
	{
		if (this->_dev1.current!=0)
			this->disconnect(this->_dev1.current);
		if (this->_dev1.desired!=0)
			this->connect(this->_dev1.desired);

		std::lock_guard guard(this->_propertyMtx);
		this->_dev1.fulfill();
	}

	if (!(this->_dev2.isFulfilled()))
	{
		if (this->_dev2.current!=0)
			this->disconnect(this->_dev2.current);
		if (this->_dev2.desired!=0)
			this->connect(this->_dev2.desired);

		std::lock_guard guard(this->_propertyMtx);
		this->_dev2.fulfill();
	}

	// THIS IS A WORKAROUND
	// My XM4 do not respond when 2 commands of the same function are sent back to back
	// This command breaks the chain and makes it respond every time
	// And I can't seem to be able to fix it
	{
		// this->_conn.sendCommand({0x36, 0x01}, DATA_TYPE::DATA_MDR_NO2);
		this->_conn.sendCommand({0x30, 0x01}, DATA_TYPE::DATA_MDR_NO2);
	}
}

void Headphones::setStateFromReply(BtMessage replyMessage)
{
	Buffer bytes = replyMessage.messageBytes;
	COMMAND_TYPE cmd = static_cast<COMMAND_TYPE>(bytes[0]);

	switch (cmd)
	{
	case COMMAND_TYPE::DEVICES_QUERY_RESPONSE:
	case COMMAND_TYPE::DEVICES_STATE_RESPONSE:
	{
		if (bytes[1] != 0x01)
			// Wrong query type, break
			break;

		std::vector<BluetoothDevice> savedDevices = std::vector<BluetoothDevice>({BluetoothDevice("","")});
		int dev1 = 0, dev2 = 0;
		int idx = 3;
		int numDevices = static_cast<unsigned char>(bytes[2]);
		for (; numDevices > 0; numDevices--)
		{
			std::string mac_addr = "";
			for (int i = idx; i<idx+MAC_ADDR_STR_SIZE; i++)
			{
				mac_addr += bytes[i];
			}

			idx += MAC_ADDR_STR_SIZE;

			unsigned char number = static_cast<unsigned char>(bytes[idx]);

			idx++;
			unsigned char name_length = static_cast<unsigned char>(bytes[idx]);
			idx++;
			std::string name = "";
			for (int i = idx; i<idx+name_length; i++)
			{
				name += bytes[i];
			}

			idx += name_length;

			BluetoothDevice dev = {name, mac_addr};
			savedDevices.push_back(dev);
			if (number == 0x01)
			{
				// dev1 = (this->_savedDevices.end() - this->_savedDevices.begin() - 1);
				dev1 = savedDevices.size()-1;
				std::cout<< "device 1: "<< dev1 << std::endl;
			}
			if (number == 0x02)
			{
				// dev2 = (this->_savedDevices.end() - this->_savedDevices.begin() - 1);
				dev2 = savedDevices.size()-1;
				std::cout<< "device 2: "<< dev2 << std::endl;
			}
		}
		std::cout<<"Connected devices received:\n";
		for (auto &dev: savedDevices)
		{
			std::cout<< dev.name<<": "<<dev.mac<<std::endl;
		}

		{
			std::lock_guard guard(this->_propertyMtx);
			this->_savedDevices = std::move(savedDevices);
			this->_dev1.setState(dev1);
			this->_dev2.setState(dev2);
		}
		break;
	}
	
	case COMMAND_TYPE::XM4_OPTIMIZER_RESPONSE:
	{
		if (bytes[2] != 0x00)
			this->_optimizerState.setState(OPTIMIZER_STATE::OPTIMIZING);
		else
			this->_optimizerState.setState(OPTIMIZER_STATE::IDLE);
		break;
	}

	default:
		break;
	}
}
