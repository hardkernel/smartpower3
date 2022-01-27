#!/bin/bash
# ----------------------------------------------------------------------------
# Read data from Smart-Power-3
#
# The script reads UDP-data sent from the SP3 and filters it:
#   - tr  removes unwanted chars
#   - sed filters all data with both channels turned off
#
# The tee-command will output the data to a csv-file for later analysis
# and pipe it to the screen for further processing or viewing.
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


PREFIX="${1:-sp3}"

netcat -u -l 6000 | \
  tr -d '\015\000' | \
    sed -u -ne '/^\([^,]*,\)\{8\}1\|^\([^,]*,\)\{13\}1/p' | \
      tee "$PREFIX-$(date +'%Y%m%d-%H%M%S').csv"
