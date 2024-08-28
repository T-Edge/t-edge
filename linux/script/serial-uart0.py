#!/usr/bin/env python3
import os
from datetime import datetime
from pathlib import Path

cp210x = Path("/sys/bus/usb/drivers/cp210x")
assert cp210x.exists(), "cp210x driver not loaded"


for dev in cp210x.glob("*:*"):
    if (dev / "interface").read_text().strip().endswith("Interface 0"):
        ttyUSB = next(dev.glob("ttyUSB*")).stem
        break
else:
    raise RuntimeError("Port 0 on cp210x not found")

device = os.path.join("/dev", ttyUSB)
outfile = "{}-{}.log".format(ttyUSB, datetime.now().strftime("%Y%m%d%H%M%S"))

os.execvp(
    "picocom",
    [
        "picocom",
        "--baud=115200",
        "--databits=8",
        "--parity=n",
        "--stopbits=1",
        "--logfile={}".format(outfile),
        device,
    ],
)
