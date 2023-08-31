#include "SingleInstanceFuture.h"
#include "BluetoothWrapper.h"
#include "Constants.h"

#include <mutex>

template <class T>
struct Property {
	T current;
	T desired;

	void fulfill();
	bool isFulfilled();
};

class Headphones {
public:
	Headphones(BluetoothWrapper& conn);

	void setAmbientSoundControl(bool val);
	bool getAmbientSoundControl();

	bool isFocusOnVoiceAvailable();
	void setFocusOnVoice(bool val);
	bool getFocusOnVoice();

	bool isSetAsmLevelAvailable();
	void setAsmLevel(int val);
	int getAsmLevel();

	void setOptimizerState(OPTIMIZER_STATE val);
	OPTIMIZER_STATE getOptimizerState();

	void setSurroundPosition(SOUND_POSITION_PRESET val);
	SOUND_POSITION_PRESET getSurroundPosition();

	void setVptType(int val);
	int getVptType();
	
	bool isChanged();
	void setChanges();
private:
	Property<bool> _ambientSoundControl = { 0 };
	Property<bool> _focusOnVoice = { 0 };
	Property<int> _asmLevel = { 0 };

	Property<SOUND_POSITION_PRESET> _surroundPosition = { SOUND_POSITION_PRESET::OUT_OF_RANGE, SOUND_POSITION_PRESET::OFF };
	Property<int> _vptType = { 0 };

	Property<OPTIMIZER_STATE> _optimizerState = { OPTIMIZER_STATE::IDLE };
	
	std::mutex _propertyMtx;

	BluetoothWrapper& _conn;
};

template<class T>
inline void Property<T>::fulfill()
{
	this->current = this->desired;
}

template<class T>
inline bool Property<T>::isFulfilled()
{
	return this->desired == this->current;
}
