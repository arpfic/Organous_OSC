#!/bin/bash
export LANG=en_US.UTF-8
SCRIPT_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $SCRIPT_PATH
sleep 2
python3 midi_connect.py
