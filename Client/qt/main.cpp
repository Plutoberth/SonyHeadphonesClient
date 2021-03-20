#include <QtWidgets/QApplication>
#include <memory>

#include "Dialog.h"

#ifdef Q_OS_LINUX
#include "linux/LinuxBluetoothConnector.h"
#define BluetoothConnector LinuxBluetoothConnector
#elif defined(Q_OS_WIN32)
#include "windows/WindowsBluetoothConnector.h"
#define BluetoothConnector WindowsBluetoothConnector
#elif defined(Q_OS_MACOS)
#include "macos/MacOSBluetoothConnector.h"
#define BluetoothConnector MacOSBluetoothConnector
#endif

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	Dialog *dia = new Dialog(std::make_unique<BluetoothConnector>());
	dia->show();
	return app.exec();
}
