Contributed Software
====================

Note that files in this directory are contributed by users and are not maintained
by Hardkernel.


sp3-datalogger.py
-----------------

This script logs data to csv and/or stdout using UDP or the serial interface.
UDP-logging uses pure python3, serial-logging requires pyserial.

Feedback from Windows-users would be appreciated!

UDP-logging requires a post 1.8 firmware. The master-branch has all the
necessary patches, but currently you have to compile your own firmware.

The script will not only capture the data, but allows also to add a
timestamp (iso-format,unix-format,whatever) as an additional column.

After the script is finished (i.e. interrupted by CTRL-C), it displays a
short summary of the data-collection:

```
Protocol
========

Start:    19.02.2022 10:58:34
End:      19.02.2022 10:58:43
Duration: 00:08

Messages:
  total:      1772
  success:    1772
  filtered:   0
  failed:     0
  incomplete: 0
  interval:   4.996 ms
```


sp3-analyze.py
--------------

Create summary analysis for collected data. Outputs a report like:

```
measurement-duration: 276.4 sec

------- channel 0 ----------
  min current  0.000 mA
  avg current 291.916 mA
  max current 471.000 mA

  min voltage  0.052 V
  avg voltage  5.196 V
  max voltage  5.223 V

  min power  0.000 W
  avg power  1.515 W
  max power  2.444 W
  tot energy 0.027 Wh
----------------------------

------- channel 1 ----------
  min current  0.000 mA
  avg current 144.796 mA
  max current 204.000 mA

  min voltage  0.000 V
  avg voltage  5.193 V
  max voltage  5.224 V

  min power  0.000 W
  avg power  0.752 W
  max power  1.059 W
  tot energy 0.058 Wh
----------------------------
```


sp3-reader.sh
-------------

This is an obsolete script, use `sp3-datalogger.py` instead.
