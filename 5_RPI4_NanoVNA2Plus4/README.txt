This is a Python program that implements communication with a VNA,
specifically, "NanoVNA2 Plus4" to read S11 and S21 data. This specific
VNA is the original one: https://nanorfe.com/nanovna-v2.html
The program then plots the data. It can also save touchstone files as well. 
It sweeps a frequency range split into segments, with each segment then read-in
iteratively. This is a trick to be able to get more data-points,
because by default only 101 poinst are collectd (tough, we could
set that to any value between 2-201 points).

This code is meant to be the simplest way to connect, control and 
read data from the VNA device, using Python. Of course, you can
use NanoVNA-QT or NanoVNA Saver complex GUI interfaces to achieve
the same thing. But, this simple program here is intended to allow one 
to easily integrate this into larger projects.

There are two versions of this program:

1-- Windows version.
Implemented and tested in Anaconda Spyder on Windows 11. 
To use, you only need to edit nanovna.py and change the hard-coded 
COM port to what you have on your computer assigned as COM port to 
the VNA device.

2-- Linux Raspberry Pi 4 (RPI4) version.
Implemented and tested on RPI4. You can simply run as:
> python3 NanoVNA2_Reader_Segments.py

Finally, for additional details, including credits, please see the 
description, comments, and source code inside the python files. 