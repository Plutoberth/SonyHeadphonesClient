#include "Dialog.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"

Dialog::Dialog(std::unique_ptr<IBluetoothConnector> connector, QDialog *parent)
	: btWrap(BluetoothWrapper(std::move(connector))) {
	setupUi(this);
	setupConnectedDevices();
}

void Dialog::setupConnectedDevices() {
	deviceListWidget->clear();
	deviceMap.clear();
	for (BluetoothDevice dev : btWrap.getConnectedDevices()) {
		deviceMap[dev.name] = dev.mac;
		deviceListWidget->addItem(QString::fromStdString(dev.name));
	}
}

void Dialog::on_refreshButton_clicked() {
	setupConnectedDevices();
	connectButton->setEnabled(false);
	deviceListWidget->clearSelection();
}

void Dialog::on_connectButton_clicked() {
	if (isConnected) {
		statusLabel->setText(tr("Disconnecting"));
		if (disconnectFuture.ready()) {
			disconnectFuture.get();
		}
		disconnectFuture.setFromAsync([this]() {
			btWrap.disconnect();
			statusLabel->setText(QStringLiteral(""));
			deviceListWidget->setEnabled(true);
			refreshButton->setEnabled(true);
			ambientSoundModeGroupBox->setEnabled(false);
			connectButton->setText(tr("&Connect"));
			connectButton->setEnabled(false);
			isConnected = false;
		});
	} else {
		statusLabel->setText(tr("Connecting"));
		connectButton->setEnabled(false);
		refreshButton->setEnabled(false);
		deviceListWidget->setEnabled(false);
		deviceListWidget->clearSelection();
		label->setText(
			tr("Control ambient sound for your %1s").arg(selectedDevice));
		if (connectFuture.ready()) {
			connectFuture.get();
		}
		connectFuture.setFromAsync([this]() {
			btWrap.connect(deviceMap[selectedDevice.toStdString()]);
			statusLabel->setText(QStringLiteral(""));
			isConnected = true;
			connectButton->setText(tr("&Disconnect"));
			connectButton->setEnabled(true);
			refreshButton->setEnabled(false);
			deviceListWidget->setEnabled(false);
			ambientSoundModeGroupBox->setEnabled(true);
			ambientSoundControlCheckBox->setEnabled(true);
			// Send initial state
			btWrap.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
				NC_ASM_EFFECT::ADJUSTMENT_COMPLETION,
				NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
				ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
				ASM_ID::NORMAL,
				0));
		});
	}
}

void Dialog::on_ambientSoundSlider_valueChanged(int value) {
	focusOnVoiceCheckBox->setEnabled(value >= MINIMUM_VOICE_FOCUS_STEP);
	bool ambientSoundControl = ambientSoundControlCheckBox->isChecked();
	static bool sentAmbientSoundControl = ambientSoundControl;
	bool focusOnVoice = focusOnVoiceCheckBox->isChecked();
	static bool sentFocusOnVoice = focusOnVoice;
	int asmLevel = ambientSoundSlider->value();
	static int lastAsmLevel = asmLevel;
	static int sentAsmLevel = asmLevel;
	if (sendCommandFuture.ready()) {
		try {
			sendCommandFuture.get();
		} catch (const RecoverableException &exc) {
			if (exc.shouldDisconnect) {
				btWrap.disconnect();
				statusLabel->setText(tr("Disconnected due to %1")
										 .arg(QString::fromUtf8(exc.what())));
			}
		}
	} else if (sendCommandFuture.valid()) {
		statusLabel->setText(tr("Waiting for prior command to send"));
	} else if (sentAsmLevel != asmLevel || sentFocusOnVoice != focusOnVoice ||
			   sentAmbientSoundControl != ambientSoundControl) {
		statusLabel->setText(tr("Sending command"));
		auto ncAsmEffect = ambientSoundSlider->isSliderDown() ?
								 NC_ASM_EFFECT::ADJUSTMENT_IN_PROGRESS :
						   ambientSoundControl ?
								 NC_ASM_EFFECT::ADJUSTMENT_COMPLETION :
								 NC_ASM_EFFECT::OFF;
		auto asmId = focusOnVoice ? ASM_ID::VOICE : ASM_ID::NORMAL;
		lastAsmLevel = asmLevel == -1 ? lastAsmLevel : asmLevel;
		asmLevel = ambientSoundControl ? lastAsmLevel : -1;
		sendCommandFuture.setFromAsync([this, asmId, ncAsmEffect, asmLevel]() {
			auto ret =
				btWrap.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
					ncAsmEffect,
					NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
					ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
					asmId,
					asmLevel));
			statusLabel->setText(QStringLiteral(""));
			return ret;
		});
		sentAsmLevel = asmLevel;
		sentFocusOnVoice = focusOnVoice;
		sentAmbientSoundControl = ambientSoundControl;
	}
}

void Dialog::on_deviceListWidget_itemSelectionChanged() {
	if (!deviceListWidget->selectedItems().isEmpty()) {
		selectedDevice = deviceListWidget->selectedItems().first()->text();
		connectButton->setEnabled(true);
	}
}

void Dialog::on_ambientSoundControlCheckBox_stateChanged(int state) {
	ambientSoundSlider->setEnabled(state == Qt::CheckState::Checked);
	focusOnVoiceCheckBox->setEnabled(state == Qt::CheckState::Checked &&
									 ambientSoundSlider->value() >=
										 MINIMUM_VOICE_FOCUS_STEP);
}

void Dialog::on_focusOnVoiceCheckBox_stateChanged(int state) {
	on_ambientSoundSlider_valueChanged(ambientSoundSlider->value());
}
