#!/bin/bash
screen -dmS ttymidi  /bin/ttymidi -s /dev/ttyAMA0 -b 31250
screen -dmS puredata /usr/bin/puredata -nogui /media/rpi_pd_osc_F767ZI.pd
sleep 1
/usr/bin/aconnect 128:0 129:0
