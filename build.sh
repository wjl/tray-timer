#!/bin/bash
set -e -u

# Create and activate our venv.
pyenv exec python -m venv venv
source ./venv/bin/activate

# Install dependencies.
pip install --upgrade pip
pip install \
	PyQt6 \
	pyinstaller \
	;

# Build a single binary.
pyinstaller --onefile --noconsole tray-timer.py
