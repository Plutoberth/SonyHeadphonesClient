#include "Dialog.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"

Dialog::Dialog(BluetoothWrapper bt, QDialog *parent) : btWrap(std::move(bt)) {
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

void Dialog::on_deviceDisconnected() {
	statusLabel->setText(QStringLiteral(""));
	deviceListWidget->setEnabled(true);
	refreshButton->setEnabled(true);
	ambientSoundModeGroupBox->setEnabled(false);
	connectButton->setText(tr("&Connect"));
	this->isConnected = false;
}

void Dialog::on_deviceConnected() {
	statusLabel->setText(QStringLiteral(""));
	this->isConnected = true;
	connectButton->setText(tr("&Disconnect"));
	connectButton->setEnabled(true);
	refreshButton->setEnabled(false);
	deviceListWidget->setEnabled(false);
	ambientSoundModeGroupBox->setEnabled(true);
	ambientSoundControlCheckBox->setEnabled(true);

	// Send the initial state
	this->on_ambientSoundSlider_valueChanged(ambientSoundSlider->value());
}

void Dialog::on_connectButton_clicked() {
	if (connectionFuture.ready()) {
		connectionFuture.get();
	} else if (connectionFuture.valid()) {
		// TODO: if something is in flight and this button was clicked, we
		// should indicate we failed and terminate
	}

	// TODO: Add a timeout and fail connections
	// TODO: Handle connections failing and report.
	if (isConnected) {
		statusLabel->setText(tr("Disconnecting..."));

		connectButton->setEnabled(false);

		connectionFuture.setFromAsync([this]() {
			btWrap.disconnect();
			QMetaObject::invokeMethod(
				this, "on_deviceDisconnected", Qt::BlockingQueuedConnection);
		});
	} else {
		statusLabel->setText(tr("Connecting"));
		connectButton->setEnabled(false);
		refreshButton->setEnabled(false);
		deviceListWidget->setEnabled(false);
		deviceListWidget->clearSelection();
		label->setText(
			tr("Control ambient sound for your %1s").arg(selectedDevice));

		auto selectedDevice = this->selectedDevice.toStdString();
		connectionFuture.setFromAsync([this]() {
			btWrap.connect(deviceMap[this->selectedDevice.toStdString()]);

			QMetaObject::invokeMethod(
				this, "on_deviceConnected", Qt::BlockingQueuedConnection);
		});
	}
}

void Dialog::on_commandSent() {
	statusLabel->setText(QStringLiteral(""));
}

void Dialog::on_ambientSoundSlider_valueChanged(int asmSliderValue) {
	focusOnVoiceCheckBox->setEnabled(value >= MINIMUM_VOICE_FOCUS_STEP);

	bool ambientSoundControl = ambientSoundControlCheckBox->isChecked();

	bool focusOnVoice = focusOnVoiceCheckBox->isChecked();

	// If we finished sending a command
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
	}

	// If a command is in-flight
	if (sendCommandFuture.valid()) {
		// TODO: Maybe we can do something more interesting in this state
		statusLabel->setText(tr("Waiting for prior command to send..."));
	} else {
		const NC_ASM_EFFECT ncAsmEffect = [this, ambientSoundControl] {
			if (ambientSoundSlider->isSliderDown()) {
				return NC_ASM_EFFECT::ADJUSTMENT_IN_PROGRESS;
			} else if (ambientSoundControl) {
				return NC_ASM_EFFECT::ADJUSTMENT_COMPLETION;
			} else {
				return NC_ASM_EFFECT::OFF;
			}
		}();

		const auto asmId = focusOnVoice ? ASM_ID::VOICE : ASM_ID::NORMAL;

		const auto asmLevel = ambientSoundControl ? asmSliderValue : -1;

		statusLabel->setText(tr("Sending command"));

		// TODO: Ensure this is called when ambient sound control is turned
		// off

		sendCommandFuture.setFromAsync([this, asmId, ncAsmEffect, asmLevel]() {
			btWrap.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
				ncAsmEffect,
				NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
				ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
				asmId,
				asmLevel));
			QMetaObject::invokeMethod(
				this, "on_commandSent", Qt::BlockingQueuedConnection);
		});
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
