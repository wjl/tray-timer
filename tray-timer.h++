#ifndef __tray_timer__header__
#define __tray_timer__header__

#include <memory>
#include <QtWidgets>

struct Tray_Timer : public QObject {
	Tray_Timer();
	~Tray_Timer();

	private slots:
	void on_tick();
	void on_icon_clicked(QSystemTrayIcon::ActivationReason);
	void on_message_clicked();
	void on_message_timeout();
	void on_menu_start();
	void on_menu_stop();

	private:
	struct Detail;
	std::unique_ptr<Detail> detail;
	Q_OBJECT
};

#endif
