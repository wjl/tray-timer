#include "tray-timer.h++"

struct Tray_Timer::Detail {
  // GUI elements
  QSystemTrayIcon icon;
  QMenu menu;

  // Main timer
  double duration;
  bool running;    // gcc 4.7 = false;
  QTime timer;

  // Message
  QTimer message_timer;

  // Start dialog
  QString last_text;
};

static QIcon icon_for_completion(double completion) {
  // Draw on a relatively large pixmap
  QPixmap pixmap(128, 128);

  // Make transparent
  pixmap.fill(Qt::transparent);

  // Define timer bounding box (with a little margin)
  QRectF bounding_box(8, 8, pixmap.width()-16, pixmap.height()-16);

  // Use a painter to draw on the pixmap
  QPainter p;
  p.begin(&pixmap);

  // Draw the timer background
  p.setPen(Qt::lightGray);
  p.setBrush(Qt::lightGray);
  p.drawEllipse(bounding_box);

  // Draw the current timer completion
  p.setPen(Qt::transparent);
  if (completion >= 1.0) {
    completion = 1.0;
    p.setBrush(Qt::darkRed);
    p.drawEllipse(bounding_box);
  } else {
    p.setBrush(Qt::darkGreen);
    p.drawPie(bounding_box, 0.25 * 360 * 16, -completion * 360 * 16);
  }

  // Draw the timer outline
  p.setPen(Qt::darkGray);
  { auto pen = p.pen(); pen.setWidth(8); p.setPen(pen); }
  p.setBrush(Qt::transparent);
  p.drawEllipse(bounding_box);

  // Finish drawing
  p.end();

  return pixmap;
}

Tray_Timer::Tray_Timer() :
  detail(new Detail)
{
  // Setup tray icon
  auto & icon = detail->icon;
  connect (
    &icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    this, SLOT(on_icon_clicked(QSystemTrayIcon::ActivationReason))
  );
  connect(&icon, SIGNAL(messageClicked()), this, SLOT(on_message_clicked()));

  // Setup menu
  auto & menu = detail->menu;

  // Create menu actions
  QAction * action;
  action = menu.addAction("Tray Timer");
  action->setEnabled(false);
  menu.addAction("&Start ...", this, SLOT(on_menu_start()));
  menu.addAction("Sto&p", this, SLOT(on_menu_stop()));
  menu.addSeparator();
  menu.addAction("&Quit", qApp, SLOT(quit()));
  icon.setContextMenu(&menu);

  // Setup internal tick timer
  auto tick_timer = new QTimer(this);
  connect(tick_timer, SIGNAL(timeout()), this, SLOT(on_tick()));
  tick_timer->start(1000);

  // Create message timer
  auto & message_timer = detail->message_timer;
  connect(&message_timer, SIGNAL(timeout()), this, SLOT(on_message_timeout()));

  // Final setup
  detail->running = false; // gcc 4.7 remove
  detail->timer.start();
  on_tick();
  icon.show();
}

Tray_Timer::~Tray_Timer() {}

void Tray_Timer::on_tick() {

  // Compute completion
  const double duration   = detail->duration;
  const double elapsed    = detail->timer.elapsed() * 1e-3;
  const double remaining  = duration - elapsed;
  const double completion = detail->running ? elapsed / duration : 1.0;

  // Update tray icon
  detail->icon.setIcon(icon_for_completion(completion));

  // Update tooltip
  if (detail->running && remaining > 0) {
    auto remaining_time = QTime().addMSecs(remaining * 1e3);
    detail->icon.setToolTip (
      QString("Tray Timer\n%1:%2:%3 remaining")
      .arg(remaining_time.hour()  , 2, 10, QChar('0'))
      .arg(remaining_time.minute(), 2, 10, QChar('0'))
      .arg(remaining_time.second(), 2, 10, QChar('0'))
    );
  } else {
    detail->icon.setToolTip("Tray Timer\nTimer Expired");
  }

  // Display a message if the timer just expired
  if (detail->running && completion >= 1.0) {
    detail->running = false;
    on_message_timeout();
  }

}

void Tray_Timer::on_icon_clicked(QSystemTrayIcon::ActivationReason reason) {

  // Interacting always counts as clicking the message
  on_message_clicked();

  switch (reason) {
    case QSystemTrayIcon::DoubleClick:
      if (!detail->running) on_menu_start();
      break;

    // We don't do anything special with the rest
    default:
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::Unknown: 
    case QSystemTrayIcon::Context:
    case QSystemTrayIcon::MiddleClick:
      break;
  }
}

void Tray_Timer::on_message_clicked() {
  detail->message_timer.stop();
}

void Tray_Timer::on_message_timeout() {
  detail->icon.showMessage (
    "Tray Timer", "Timer Expired",
    QSystemTrayIcon::Information, 5000);
  detail->message_timer.start(10000);
}

void Tray_Timer::on_menu_start() {
  on_menu_stop();

  // Get the timer duration from the user
  QTime duration;
  QString text;
  do {

    bool accepted;
    text = QInputDialog::getText (
      detail->icon.contextMenu(),
      "Tray Timer",
      "Duration (hh:mm:ss)",
      QLineEdit::Normal,
      detail->last_text,
      &accepted,
      0 // flags
    );
    if (!accepted) return;

    // Convert the text entry to a time value
    duration = QTime::fromString(text);

  } while (!duration.isValid());

  // Save the current text entry to use next time
  detail->last_text = text;

  // Start the timer
  detail->duration =
    duration.hour()   * 60 * 60 +
    duration.minute() * 60      +
    duration.second()           +
    duration.msec()   * 1e-3;
  detail->running = true;
  detail->timer.start();

}

void Tray_Timer::on_menu_stop() {
  on_message_clicked();
  detail->running = false;
}

