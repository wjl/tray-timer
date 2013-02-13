#include "tray-timer.h++"

int main (int argc, char ** argv) {
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);
	Tray_Timer tray_timer;
	return app.exec();
}
