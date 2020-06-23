"""pip install oscpy et python python_udpsend.py
"""

import time
from oscpy.client import OSCClient

address = "172.24.1.143"
port = 12345

osc = OSCClient(address, port)
for j in range(10):
  for i in range(1000):
    osc.send_message(b'/main/motor', [0, 1, 1.0])
    osc.send_message(b'/main/motor', [2, 3, 1.0])
    osc.send_message(b'/main/motor', [4, 5, 1.0])
    osc.send_message(b'/main/motor', [6, 7, 1.0])
    time.sleep(1 * 10**(-4))
    osc.send_message(b'/main/motor', [8, 9, 1.0])
    osc.send_message(b'/main/motor', [10, 11, 1.0])
    osc.send_message(b'/main/motor', [12, 13, 1.0])
    osc.send_message(b'/main/motor', [14, 15, 1.0])
    time.sleep(1 * 10**(-4))
    osc.send_message(b'/main/motor', [16, 17, 1.0])
    osc.send_message(b'/main/motor', [18, 19, 1.0])
    osc.send_message(b'/main/motor', [20, 21, 1.0])
    osc.send_message(b'/main/motor', [22, 23, 1.0])
    time.sleep(i * 10**(-4 + j))
    osc.send_message(b'/main/motor', [0, 1, -1.0])
    osc.send_message(b'/main/motor', [2, 3, -1.0])
    osc.send_message(b'/main/motor', [4, 5, -1.0])
    osc.send_message(b'/main/motor', [6, 7, -1.0])
    time.sleep(1 * 10**(-4))
    osc.send_message(b'/main/motor', [8, 9, -1.0])
    osc.send_message(b'/main/motor', [10, 11, -1.0])
    osc.send_message(b'/main/motor', [12, 13, -1.0])
    osc.send_message(b'/main/motor', [14, 15, -1.0])
    time.sleep(1 * 10**(-4))
    osc.send_message(b'/main/motor', [16, 17, -1.0])
    osc.send_message(b'/main/motor', [18, 19, -1.0])
    osc.send_message(b'/main/motor', [20, 21, -1.0])
    osc.send_message(b'/main/motor', [22, 23, -1.0])
    time.sleep(i * 10**(-4 + j))
