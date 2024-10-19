#!/usr/bin/env python3

# Tray Timer -- simple desktop-independent tray countdown timer
# Copyright (C) 2011-2024 Wesley J. Landaker <wjl@icecavern.net>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import math
import sys

from PyQt6.QtCore import (
	QRect,
	QTime,
	QTimer,
	Qt,
)
from PyQt6.QtGui import (
	QCursor,
	QIcon,
	QPainter,
	QPixmap,
)
from PyQt6.QtWidgets import (
	QApplication,
	QInputDialog,
	QLineEdit,
	QMenu,
	QSystemTrayIcon,
)

def draw_icon_for_completion(completion):
	# Draw on a relatively large pixmap.
	pixmap = QPixmap(128, 128)

	# Make transparent.
	pixmap.fill(Qt.GlobalColor.transparent)

	# Define timer bounding box, leaving some margin.
	margin = 128//16
	bounding_box = QRect(
		margin,                     # x
		margin,                     # y
		pixmap. width() - 2*margin, # width
		pixmap.height() - 2*margin  # height
	)

	# Use a painter to draw on the pixmap.
	p = QPainter(pixmap)

	# Draw the timer background
	p.setPen(Qt.GlobalColor.lightGray)
	p.setBrush(Qt.GlobalColor.lightGray)
	p.drawEllipse(bounding_box)

	# Draw the current timer completion
	p.setPen(Qt.GlobalColor.transparent)
	if completion >= 1.0:
		p.setBrush(Qt.GlobalColor.darkRed)
		p.drawEllipse(bounding_box)
	else:
		p.setBrush(Qt.GlobalColor.darkGreen)
		p.drawPie(bounding_box, round(0.25 * 360 * 16), round(-completion * 360 * 16))

	# Draw the timer outline
	p.setPen(Qt.GlobalColor.darkGray)
	pen = p.pen(); pen.setWidth(margin); p.setPen(pen)
	p.setBrush(Qt.GlobalColor.transparent)
	p.drawEllipse(bounding_box)

	# Finish drawing.
	p.end()

	return QIcon(pixmap)

class Tray_Timer:
	def __init__(self):
		# Tray icon.
		self.tray_icon = QSystemTrayIcon()
		self.tray_icon.activated.connect     (self.on_tray_icon_clicked)
		self.tray_icon.messageClicked.connect(self.  on_message_clicked)

		# Popup menu.
		self.popup_menu = QMenu()
		self.popup_menu.addAction("Tray Timer").setEnabled(False)
		self.popup_menu.addAction("&Start ...", self.on_menu_start)
		self.popup_menu.addAction("Sto&p"     , self.on_menu_stop)
		self.popup_menu.addSeparator()
		self.popup_menu.addAction("&Quit"     , QApplication.quit)
		self.tray_icon.setContextMenu(self.popup_menu)

		# Countdown information.
		self.countdown_duration_ms = 0
		self.countdown_elapsed_ms  = 0
		self.last_completion = None

		# Tick timer.
		self.tick_timer = QTimer()
		self.tick_timer.timeout.connect(self.on_tick)
		self.tick_timer.start(500)

		# Message timeout timer.
		self.message_timer = QTimer()
		self.message_timer.timeout.connect(self.on_message_timeout)

		# Last time text entered.
		self.last_text_entry = "00:25:00"

		# Flag so we don't get multiple dialog boxes.
		self.dialog_running = False

		# Start with an initial tick.
		self.on_tick()

		# Show the tray icon.
		self.tray_icon.show()

	def on_tick(self):
	
		# Update the countdown timer if it's running.
		if self.countdown_duration_ms > 0:
			interval = self.tick_timer.interval()
			self.countdown_elapsed_ms += interval
			completion = self.countdown_elapsed_ms / self.countdown_duration_ms
			if completion >= 1: completion = 1
		else:
			completion = 1

		# No updates are needed if complation hasn't changed.
		if completion == self.last_completion: return
		self.last_completion = completion

		# Update tray icon
		self.tray_icon.setIcon(draw_icon_for_completion(completion))

		# Update tooltip
		if completion < 1:
			remaining_ms = self.countdown_duration_ms - self.countdown_elapsed_ms
			remaining_time = QTime(0, 0).addMSecs(remaining_ms).toString("HH:mm:ss")
			self.tray_icon.setToolTip(f"Tray Timer\n{remaining_time} remaining")
		else:
			self.tray_icon.setToolTip("Tray Timer\nTimer Expired")

		# Display a message if the timer just expired
		if self.countdown_duration_ms > 0 and completion >= 1:
			self.countdown_duration_ms = 0
			self.on_message_timeout()

	def on_tray_icon_clicked(self, reason):
		# Interacting always counts as clicking the message
		self.on_message_clicked()

		if reason == QSystemTrayIcon.ActivationReason.Trigger:
			if self.countdown_duration_ms == 0:
				self.on_menu_start()

	def on_message_clicked(self):
		self.message_timer.stop()

	def on_message_timeout(self):
		self.tray_icon.showMessage("Tray Timer", "Timer Expired", QSystemTrayIcon.MessageIcon.Information, 1000)
		self.message_timer.start(2000)

	def on_menu_start(self):
		# Don't run multiple dialog boxes.
		if self.dialog_running: return

		self.on_menu_stop()

		# Create a dialog box to get the text.
		dialog = QInputDialog()
		dialog.setWindowTitle("Tray Timer")
		dialog.setLabelText("Duration (hh:mm:ss)")
		dialog.setTextValue(self.last_text_entry)
		dialog.setTextEchoMode(QLineEdit.EchoMode.Normal)

		# Move to cursor position.
		dialog.move(QCursor.pos())

		# Get the timer duration from the user.
		while True:
			self.dialog_running = True
			result = dialog.exec()
			self.dialog_running = False
			if not result: return
			text = dialog.textValue()
			duration = QTime.fromString(text, "HH:mm:ss")
			if duration.isValid(): break

		# Save the current text entry to use next time
		self.last_text_entry = text

		# Start the timer
		self.countdown_duration_ms = (
			duration.hour  () * 60 * 60 +
			duration.minute() * 60 +
			duration.second()
		) * 1000
		self.countdown_elapsed_ms = 0
		self.last_completion = None
		self.tick_timer.start()

	def on_menu_stop(self):
		self.on_message_clicked()
		self.countdown_duration_ms = 0

def main():
	app = QApplication(sys.argv)
	app.setQuitOnLastWindowClosed(False)

	timer = Tray_Timer()

	sys.exit(app.exec())

if __name__ == "__main__":
	main()
