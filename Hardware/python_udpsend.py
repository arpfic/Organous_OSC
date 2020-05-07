"""pip install oscpy et python python_udpsend.py
"""

import time
from oscpy.client import OSCClient

address = "172.24.1.143"
port = 12345

osc = OSCClient(address, port)
for i in range(100000):
#  osc.send_message(b'/tools/count', [1])
  osc.send_message(b'/main/motor', [2, 3, -1.0])
#  time.sleep(100 * 10**(-6))
#  osc.send_message(b'/main/coil', [0, 0])
  time.sleep(2000 * 10**(-6))
