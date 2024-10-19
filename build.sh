#!/bin/bash
set -e -u

python_version=3.12.7

# Check for pyenv.
if ! command -v pyenv >& /dev/null; then
	echo "pyenv not installed."
	exit 1
fi

# Install our desired Python version.
pyenv install --skip-existing "$python_version"
pyenv local "$python_version"

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
