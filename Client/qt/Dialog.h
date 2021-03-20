#pragma once

#include "BluetoothWrapper.h"
#include "SingleInstanceFuture.h"

#include "ui_dialog.h"

class IBluetoothConnector;
class QString;
class QTimer;

class Dialog : public QDialog, private Ui::Dialog {
	Q_OBJECT

public:
	explicit Dialog(std::unique_ptr<IBluetoothConnector> connector,
					QDialog *parent = nullptr);
	~Dialog();

private:
	void setupConnectedDevices();

	bool isConnected = false;
	BluetoothWrapper btWrap;
	QMap<std::string, std::string> deviceMap;
	QString selectedDevice;
	QTimer *timer;
	SingleInstanceFuture<int> sendCommandFuture;
	SingleInstanceFuture<void> connectFuture;
	SingleInstanceFuture<void> disconnectFuture;

private Q_SLOTS:
	void on_ambientSoundControlCheckBox_stateChanged(int);
	void on_ambientSoundSlider_valueChanged(int);
	void on_connectButton_clicked();
	void on_deviceListWidget_itemSelectionChanged();
	void on_refreshButton_clicked();
	void on_timeout();
};
