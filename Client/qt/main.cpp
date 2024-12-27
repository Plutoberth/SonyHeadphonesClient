#include <QtWidgets/QApplication>
#include <QFile>
#include <QString>
#include <QLatin1String> 

#include <memory>
#include <iostream>

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
#else
#error "Unsupported OS"
#endif

int main(int argc, char *argv[]) {
	try
	{
		QApplication app(argc, argv);

		// Load an application style
		QFile styleFile(QLatin1String("D:\\Kuroi.qss"));
		styleFile.open(QFile::ReadOnly);

		// Apply the loaded stylesheet
		QString style =
			QString::fromStdString(styleFile.readAll().toStdString());
		app.setStyleSheet(style);

		BluetoothWrapper bt(std::make_unique<BluetoothConnector>());
		Dialog dialog(std::move(bt));
		app.setStyleSheet(style);
		
		dialog.show();

		return app.exec();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Fatal error: " << e.what() << '\n';
		return -1337;
	}
}
