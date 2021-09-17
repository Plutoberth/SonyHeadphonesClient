#pragma once

#include "BluetoothWrapper.h"
#include "SingleInstanceFuture.h"

#include "ui_dialog.h"

class Dialog : public QDialog, private Ui::Dialog {
	Q_OBJECT

public:
	explicit Dialog(BluetoothWrapper bt,
					QDialog *parent = nullptr);

private:
	void setupConnectedDevices();

	bool isConnected = false;
	BluetoothWrapper btWrap;
	QMap<std::string, std::string> deviceMap;
	QString selectedDevice;
	SingleInstanceFuture<void> sendCommandFuture;
	SingleInstanceFuture<void> connectionFuture;

	void updateNcAsmState();

private Q_SLOTS:
	void on_ambientSoundControlCheckBox_stateChanged(int);
	void on_ambientSoundSlider_valueChanged(int);
	void on_connectButton_clicked();
	void on_deviceListWidget_itemSelectionChanged();
	void on_focusOnVoiceCheckBox_stateChanged();
	void on_refreshButton_clicked();

	void on_deviceConnected();
	void on_deviceDisconnected();
	void on_commandSent();
};
