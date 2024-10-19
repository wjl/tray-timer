#!/bin/bash
set -e -u

# Install dependencies.
pip install -r requirements.txt

# Build a single binary.
pyinstaller --onefile --noconsole tray-timer.py
