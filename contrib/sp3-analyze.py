#!/usr/bin/env python
# ----------------------------------------------------------------------------
# Analyze data from Smart-Power-3
#
# The script expects a csv-file with the data logged via serial or udp.
#
# This script is user-contributed and not maintained by Hardkernel
#
# Please create pull requests against
#   https://github.com/bablokb/smartpower3
#
# Author: Bernhard Bablok
# License: GPL3
#
# ----------------------------------------------------------------------------

import sys, pathlib
import pandas as pd

# read data from csv
infile = sys.argv[1]
data = pd.read_csv(infile,header=None,sep=',')

# add column labels
data.columns = ['ts','I_V','I_C','I_P','I_on',
                'CH0_V','CH0_C','CH0_P','CH0_on','CH0_Int',
                'CH1_V','CH1_C','CH1_P','CH1_on','CH1_Int','cs1','cs2']

# normalize and scale data
data['ts']    -= data['ts'][0]
data['ts']    *= 0.001
data['CH0_V'] *= 0.001
data['CH1_V'] *= 0.001
data['CH0_P'] *= 0.001
data['CH1_P'] *= 0.001

# shortcut to ts-column
t = data['ts']

# calculate cumulative sum of energy-consumption
esum0 = (t.diff()*data['CH0_P']).cumsum()
esum1 = (t.diff()*data['CH1_P']).cumsum()

# print basic statistics (assume sampling is constant for avg)
print("\nmeasurement-duration: %5.1f sec" % t.max())

print("\n------- channel 0 ----------")
print("  min current %6.3f mA" % data['CH0_C'].min())
print("  avg current %6.3f mA" % data[data.CH0_on!=0].CH0_C.mean())
print("  max current %6.3f mA" % data['CH0_C'].max())

print("\n  min voltage %6.3f V" % data['CH0_V'].min())
print("  avg voltage %6.3f V" % data[data.CH0_on!=0].CH0_V.mean())
print("  max voltage %6.3f V" % data['CH0_V'].max())

print("\n  min power %6.3f W" % data['CH0_P'].min())
print("  avg power %6.3f W" % data[data.CH0_on!=0].CH0_P.mean())
print("  max power %6.3f W" % data['CH0_P'].max())

total = esum0.max()/3600
print("  tot energy %6.3f Wh" % total)
print("----------------------------\n")

print("------- channel 1 ----------")
print("  min current %6.3f mA" % data['CH1_C'].min())
print("  avg current %6.3f mA" % data[data.CH1_on!=0].CH1_C.mean())
print("  max current %6.3f mA" % data['CH1_C'].max())

print("\n  min voltage %6.3f V" % data['CH1_V'].min())
print("  avg voltage %6.3f V" % data[data.CH1_on!=0].CH1_V.mean())
print("  max voltage %6.3f V" % data['CH1_V'].max())

print("\n  min power %6.3f W" % data['CH1_P'].min())
print("  avg power %6.3f W" % data[data.CH1_on!=0].CH1_P.mean())
print("  max power %6.3f W" % data['CH1_P'].max())

total = esum1.max()/3600
print("  tot energy %6.3f Wh" % total)
print("----------------------------\n")

# append to data
data['CH0_E_SUM'] = esum0
data['CH1_E_SUM'] = esum1

# write to new csv
ipath = pathlib.Path(infile)
outfile = str(ipath.parent / ipath.stem)+'_new.csv'
data.to_csv(outfile,index=False)
