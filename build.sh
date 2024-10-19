#!/bin/bash
set -e -u

# Install dependencies.
pip install --upgrade pip
pip install \
	PyQt6 \
	pyinstaller \
	;

# Build a single binary.
pyinstaller --onefile --noconsole tray-timer.py
